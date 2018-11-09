// Copyright (c) 2018, Wonter. All rights reserved.
// Use of this source code is governed by the BSD 3-Clause License,
// that can be found in the LICENSE file.

#include "table.h"

#include "gtest/gtest.h"

using namespace std;
using namespace table;

static const string DEFAULT_NAME = "table_test";

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

TEST(TableTest, EMPTY) {
    Options options;
    Table table(options, DEFAULT_NAME);
}

TEST(TableTest, OPEN_AND_CLOSE) {
    Options options;
    options.create_if_missing = true;
    options.dump_when_close = false;
    Table table(options, DEFAULT_NAME);

    Status s = table.open();
    ASSERT_TRUE(s.good()) << s.string();

    s = table.close();
    ASSERT_TRUE(s.good()) << s.string();
}

TEST(TableTest, LOAD_AND_DUMP) {
    Options options;
    options.create_if_missing = true;
    options.dump_when_close = true;
    options.max_file_size = 4096;

    vector<string> keys(1000);
    generate_n(keys.begin(), keys.size(), bind(random_string, 16));
    vector<string> values(1000);
    generate_n(values.begin(), values.size(), bind(random_string, 16));

    string table_name = "table_" + random_string(16);

    {
        Table table(options, table_name);
        Status s = table.open();
        ASSERT_TRUE(s.good()) << s.string();
        for (size_t i = 0; i < keys.size(); ++i) {
            s = table.put(keys[i], values[i]);
            ASSERT_TRUE(s.good()) << s.string();
        }
        s = table.close();
        ASSERT_TRUE(s.good()) << s.string();
    }

    {
        Table table(options, table_name);
        Status s = table.open();
        ASSERT_TRUE(s.good()) << s.string();
        for (size_t i = 0; i < keys.size(); ++i) {
            string value;
            s = table.get(keys[i], &value);
            ASSERT_TRUE(s.good()) << s.string();
            ASSERT_EQ(value, values[i]);
        }
        s = table.close();
        ASSERT_TRUE(s.good()) << s.string();
    }

    {
        Table table(options, table_name);
        Status s = table.open();
        ASSERT_TRUE(s.good()) << s.string();
        for (size_t i = 0; i < keys.size(); ++i) {
            s = table.del(keys[i]);
            ASSERT_TRUE(s.good()) << s.string();
        }
        s = table.close();
        ASSERT_TRUE(s.good()) << s.string();
    }

    {
        Table table(options, table_name);
        Status s = table.open();
        ASSERT_TRUE(s.good()) << s.string();
        for (size_t i = 0; i < keys.size(); ++i) {
            string value;
            s = table.get(keys[i], &value);
            ASSERT_FALSE(s.good());
        }
    }
}

TEST(TableTest, CRUD) {
    Options options;
    options.create_if_missing = true;
    options.dump_when_close = false;
    Table table(options, DEFAULT_NAME);
    table.open();

    // Create
    Status s = table.put("key", "value");
    ASSERT_TRUE(s.good()) << s.string();

    // Read
    string value;
    s = table.get("key", &value);
    ASSERT_TRUE(s.good()) << s.string();
    ASSERT_EQ(value, "value");
    s = table.get("key", nullptr);
    ASSERT_TRUE(s.good()) << s.string();

    // Update
    s = table.put("key", "new-value");
    ASSERT_TRUE(s.good()) << s.string();
    s = table.get("key", &value);
    ASSERT_TRUE(s.good()) << s.string();
    ASSERT_EQ(value, "new-value");

    // Delete
    s = table.del("key");
    ASSERT_TRUE(s.good()) << s.string();
    s = table.get("key", nullptr);
    ASSERT_FALSE(s.good());
}

TEST(TableTest, IO_ERROR) {
    {
        // directory does not exist
        Options options;
        options.create_if_missing = false;
        Table table(options, random_string(128));
        Status s = table.open();
        ASSERT_FALSE(s.good());
    }

    {
        // prefix-directory does not exist
        Options options;
        options.create_if_missing = true;
        Table table(options, "no/such/directory/" + random_string(128));
        Status s = table.open();
        ASSERT_FALSE(s.good());
    }

    {
        // invalid max_file_size
        string table_name = "table_" + random_string(16);

        Options options;
        options.create_if_missing = true;
        options.dump_when_close = true;
        Table table1(options, table_name);
        Status s = table1.open();
        ASSERT_TRUE(s.good()) << s.string();
        s = table1.put("key", "value");
        ASSERT_TRUE(s.good()) << s.string();
        s = table1.close();
        ASSERT_TRUE(s.good()) << s.string();

        options.max_file_size = -1;
        Table table2(options, table_name);
        s = table2.open();
        ASSERT_FALSE(s.good());
    }
}

TEST(TableTest, INVALID_OPERATION) {
    {
        // double open/close
        Options options;
        options.dump_when_close = false;
        options.create_if_missing = true;
        Table table(options, DEFAULT_NAME);
        Status s = table.open();
        ASSERT_TRUE(s.good()) << s.string();
        s = table.open();
        ASSERT_FALSE(s.good());
        s = table.close();
        ASSERT_TRUE(s.good()) << s.string();
        s = table.close();
        ASSERT_FALSE(s.good());
    }

    {
        // get/put/del/dump before open
        Options options;
        options.create_if_missing = true;
        Table table(options, DEFAULT_NAME);
        Status s = table.get("key", nullptr);
        ASSERT_FALSE(s.good());
        s = table.put("key", "value");
        ASSERT_FALSE(s.good());
        s = table.del("key");
        ASSERT_FALSE(s.good());
        s = table.dump();
        ASSERT_FALSE(s.good());
    }

    {
        // size of entry is too large
        Options options;
        options.max_file_size = 1;
        options.dump_when_close = false;
        options.create_if_missing = true;
        Table table(options, "table_" + random_string(16));
        Status s = table.open();
        ASSERT_TRUE(s.good()) << s.string();
        s = table.put("key", "value");
        ASSERT_FALSE(s.good());
    }

    {
        // delete a key that does not exist
        Options options;
        options.dump_when_close = false;
        options.create_if_missing = true;
        Table table(options, DEFAULT_NAME);
        Status s = table.open();
        ASSERT_TRUE(s.good()) << s.string();
        s = table.del("no_such_key");
        ASSERT_FALSE(s.good());
    }

    {
        // directory is already exist and options.error_if_exist = true
        Options options;
        options.error_if_exists = true;
        Table table(options, DEFAULT_NAME);
        Status s = table.open();
        ASSERT_FALSE(s.good()) << s.string();
    }
}

int main(int argc, char **argv) {
    srand(21389892);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
