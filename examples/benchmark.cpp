// Copyright (c) 2018, Wonter. All rights reserved.
// Use of this source code is governed by the BSD 3-Clause License,
// that can be found in the LICENSE file.

#include <vector>
#include <chrono>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <functional>

#include "table.h"

using namespace std;
using namespace table;
using namespace std::chrono;

static string random_string(size_t length) {
    auto randchar = []() -> char
    {
        const char charset[] =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";
        size_t max_index = sizeof(charset) - 1;
        return charset[rand() % max_index];
    };
    string str(length, 0);
    generate_n(str.begin(), length, randchar);
    return str;
}

static string ordinal(int n) {
    ostringstream out;
    out << n;
    if (n % 10 == 1 && n % 100 != 11) {
        out << "st";
    } else if (n % 10 == 2 && n % 100 != 12) {
        out << "nd";
    } else if (n % 10 == 3 && n % 100 != 13) {
        out << "rd";
    } else {
        out << "th";
    }
    return out.str();
}

static inline void assert_fatal(const Status& s) {
    if (!s.good()) {
        cerr << s.string() << endl;
        exit(-1);
    }
}

static void put_benchmark(int entry_num, int test_times) {
    vector<string> keys;
    vector<string> values;
    keys.resize(entry_num);
    values.resize(entry_num);
    generate_n(keys.begin(), keys.size(), bind(random_string, 16));
    generate_n(values.begin(), values.size(), bind(random_string, 100));

    cout << "put: " << entry_num << " entries" << endl;
    for (int times = 1; times <= test_times; ++times) {
        Options options;
        options.create_if_missing = true;
        options.dump_when_close = false;
        Table table(options, "table_benchmark");
        Status s = table.open();
        assert_fatal(s);

        high_resolution_clock::time_point start = high_resolution_clock::now();
        for (size_t i = 0; i < keys.size(); ++i) {
            Status s = table.put(keys[i], values[i]);
            assert_fatal(s);
        }
        high_resolution_clock::time_point end = high_resolution_clock::now();

        cout << ordinal(times) << ": spend " << duration_cast<milliseconds>(end - start).count() <<
            "ms" << endl;
    }
}

void get_benchmark(int entry_num, int get_times, int test_times) {
    vector<string> keys;
    vector<string> values;
    keys.resize(entry_num);
    values.resize(entry_num);
    generate_n(keys.begin(), keys.size(), bind(random_string, 16));
    generate_n(values.begin(), values.size(), bind(random_string, 100));

    Options options;
    options.create_if_missing = true;
    options.dump_when_close = false;
    Table table(options, "table_benchmark");
    Status s = table.open();
    assert_fatal(s);
    for (int i = 0; i < entry_num; ++i) {
        s = table.put(keys[i], values[i]);
        assert_fatal(s);
    }

    cout << "get: " << entry_num << " entries, get " << get_times << " times" << endl;
    for (int times = 1; times <= test_times; ++times) {
        int random_index[get_times];
        srand(time(nullptr));
        for (int i = 0; i < get_times; ++i) {
            random_index[i] = rand() % entry_num;
        }

        high_resolution_clock::time_point start = high_resolution_clock::now();
        std::string value;
        for (int i = 0; i < get_times; ++i) {
            Status s = table.get(keys[random_index[i]], &value);
            assert_fatal(s);
        }
        high_resolution_clock::time_point end = high_resolution_clock::now();

        cout << ordinal(times) <<  ": spend " << duration_cast<milliseconds>(end - start).count()
            << "ms" << endl;
    }
}

int main() {
    put_benchmark(100000, 5);
    put_benchmark(1000000, 5);

    get_benchmark(100000, 10000, 5);
    get_benchmark(1000000, 10000, 5);
    return 0;
}