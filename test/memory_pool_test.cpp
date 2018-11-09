// Copyright (c) 2018, Wonter. All rights reserved.
// Use of this source code is governed by the BSD 3-Clause License,
// that can be found in the LICENSE file.

#include "memory_pool.h"

#include "gtest/gtest.h"

using namespace std;
using namespace table;

static const size_t BLOCK_SIZE  = 4096;

class MemoryPoolTest : public ::testing::Test {
public:
    MemoryPoolTest() : _pool(500) {  }
    ~MemoryPoolTest() override = default;

protected:
    MemoryPool _pool;
};

TEST_F(MemoryPoolTest, EMPTY) {
}

TEST_F(MemoryPoolTest, ALLOC) {
    int num = 10000;
    srand(27836745);

    vector<pair<size_t, char*>> blocks;
    for (int i = 0; i < num; ++i) {
        size_t size = rand() % 512 + 1;
        char *p = _pool.alloc(size);
        for (size_t i = 0; i < size; ++i) {
            p[i] = i % 256;
        }
        blocks.emplace_back(size, p);
    }

    for (auto &block : blocks) {
        for (size_t i = 0; i < block.first; ++i) {
            // Check the "i"th allocation for the known bit pattern
            ASSERT_EQ(static_cast<int>(block.second[i]) & 0xff, static_cast<int>(i) % 256);
        }
        _pool.dealloc(block.second, block.first);
    }

    sleep(1);

    // free the expired block
    _pool.alloc(1);
    ASSERT_TRUE(_pool._block_persist.empty());
}

TEST_F(MemoryPoolTest, REUSE) {
    // NBLOCK at here should equal to NBLOCK in Pool::refill
    static constexpr int NBLOCK = 20;

    vector<pair<size_t, char*>> blocks;
    for (int i = 8; i <= 256; i += 8) {
        for (int j = 0; j < NBLOCK; ++j) {
            blocks.emplace_back(i, _pool.alloc(i));
        }
    }
    for (auto &block : blocks) {
        _pool.dealloc(block.second, block.first);
    }

    sleep(1);

    vector<pair<size_t, char*>> new_blocks;
    for (int i = 8; i <= 256; i += 8) {
        for (int j = 0; j < NBLOCK; ++j) {
            new_blocks.emplace_back(i, _pool.alloc(i));
        }
    }
    for (size_t i = 0; i < blocks.size(); ++i) {
        ASSERT_EQ(blocks[i].second, new_blocks[i].second);
    }
}

TEST_F(MemoryPoolTest, DUP) {
    char p[BLOCK_SIZE];
    for (size_t i = 0; i < BLOCK_SIZE; ++i) {
        p[i] = i % 256;
    }

    char *new_p = _pool.dup(p, BLOCK_SIZE);
    for (size_t i = 0; i < BLOCK_SIZE; ++i) {
        ASSERT_EQ(new_p[i], p[i]);
        new_p[i] = (p[i] + 1) % 256;
        ASSERT_EQ(static_cast<int>(new_p[i]) & 0xff, ((static_cast<int>(p[i]) & 0xff) + 1) % 256);
    }
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}