// Copyright (c) 2018, Wonter. All rights reserved.
// Use of this source code is governed by the BSD 3-Clause License,
// that can be found in the LICENSE file.

#include "skiplist.h"
#include "comparator.h"
#include "memory_pool.h"

#include "gtest/gtest.h"

using namespace std;
using namespace table;

class ByteWiseComparator : public Comparator {
public:
    int compare(const ByteArray& lhs, const ByteArray& rhs) const override {
        size_t size = std::min(lhs.size(), rhs.size());
        int cmp = memcmp(lhs.data(), rhs.data(), size);
        if (cmp != 0) {
            return cmp;
        }
        return static_cast<int>(lhs.size()) - static_cast<int>(rhs.size());
    }
};

static  MemoryPool          pool(1000);
static  ByteWiseComparator  cmp;

class SkipListTest : public ::testing::Test {
public:
    SkipListTest() : _list(&cmp, &pool) {  }
    ~SkipListTest() override = default;
protected:
    SkipList _list;
};

TEST_F(SkipListTest, EMPTY) {
}

TEST_F(SkipListTest, INSERT) {
    SkipList::Iterator it;

    it = _list.insert("f", "f");
    ASSERT_TRUE(it.good());
    ASSERT_EQ(it.key(), "f");
    ASSERT_EQ(it.value(), "f");

    // insert at the head
    it = _list.insert("a", "a");
    ASSERT_TRUE(it.good());
    ASSERT_EQ(it.key(), "a");
    ASSERT_EQ(it.value(), "a");

    // insert at the tail
    it = _list.insert("z", "z");
    ASSERT_TRUE(it.good());
    ASSERT_EQ(it.key(), "z");
    ASSERT_EQ(it.value(), "z");

    // insert into the middle
    it = _list.insert("b", "b");
    ASSERT_TRUE(it.good());
    ASSERT_EQ(it.key(), "b");
    ASSERT_EQ(it.value(), "b");

    // double-insert
    it = _list.insert("b", "b");
    ASSERT_FALSE(it.good());
}

TEST_F(SkipListTest, UPDATE) {
    SkipList::Iterator it;

    it = _list.update("a", "a");
    ASSERT_FALSE(it.good());

    it = _list.insert("a", "a");
    ASSERT_TRUE(it.good());
    it = _list.update("a", "b");
    ASSERT_TRUE(it.good());
    ASSERT_EQ(it.key(), "a");
    ASSERT_EQ(it.value(), "b");
}

TEST_F(SkipListTest, LOOKUP) {
    SkipList::Iterator it;

    it = _list.lookup("a");
    ASSERT_FALSE(it.good());

    it = _list.insert("a", "a");
    ASSERT_TRUE(it.good());
    it = _list.lookup("a");
    ASSERT_TRUE(it.good());
    ASSERT_EQ(it.key(), "a");
    ASSERT_EQ(it.value(), "a");
}

TEST_F(SkipListTest, REMOVE) {
    ASSERT_FALSE(_list.remove("a"));

    auto it = _list.insert("a", "a");
    ASSERT_TRUE(it.good());
    ASSERT_TRUE(_list.remove("a"));
}

TEST_F(SkipListTest, ITERATION) {
    std::vector<std::string> keys = {"a", "b", "c", "d"};

    SkipList::Iterator it;
    for (const std::string &k : keys) {
        it = _list.insert(k, k);
        ASSERT_TRUE(it.good());
    }

    size_t i = 0;
    it = _list.begin();
    while (it.good() && i < keys.size()) {
        ASSERT_EQ(
            std::string(it.key().data(), it.key().size()),
            keys[i]
        );
        ++i;
        it.next();
    }
    ASSERT_FALSE(it.good());
    ASSERT_FALSE(i < keys.size());
}

TEST_F(SkipListTest, CRUD_LOOP) {
    static constexpr int NUM = 10000;

    // insert NUM key
    for (int i = 0; i < NUM; ++i) {
        string key   = to_string(i);
        string value = to_string(i);
        SkipList::Iterator it = _list.insert(key, value);
        ASSERT_TRUE(it.good());
    }

    // do we have NUM nodes?
    auto it = _list.begin();
    int i = 0;
    while (i < NUM && it.good()) {
        ++i;
        it.next();
    }
    ASSERT_FALSE(i < NUM);
    ASSERT_FALSE(it.good());

    // update them
    for (int i = 0; i < NUM; ++i) {
        string key   = to_string(i);
        string value = to_string(i + 1);
        SkipList::Iterator it = _list.update(key, value);
        ASSERT_TRUE(it.good());
        it = _list.lookup(key);
        ASSERT_EQ(it.value(), value);
    }

    // remove them
    for (int i = 0; i < NUM; ++i) {
        string key = to_string(i);
        ASSERT_TRUE(_list.remove(key));
    }

    ASSERT_FALSE(_list.begin().good());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
