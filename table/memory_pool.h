// Copyright (c) 2018, Wonter. All rights reserved.
// Use of this source code is governed by the BSD 3-Clause License,
// that can be found in the LICENSE file.

#ifndef TABLE_POOL_H
#define TABLE_POOL_H

#include "common.h"

namespace table {

class MemoryPool {
TABLE_PUBLIC:
    explicit MemoryPool(int ttl_msec);
    ~MemoryPool();

    char* alloc(size_t size);
    void  dealloc(char* p, size_t size);
    char* dup(const char* p, size_t size);

    // Non-copying
    MemoryPool(const MemoryPool&) = delete;
    MemoryPool& operator=(const MemoryPool&) = delete;

TABLE_PRIVATE:
    struct Block {
        char   *addr;
        std::chrono::time_point<std::chrono::steady_clock>  expire_at;
    };

    enum {
        ALIGN          = 8,
        MAX_BLOCK_SIZE = 256,
    };

    int _ttl_msec;
    // we align all size to ALIGN
    std::deque<Block> _block_queue[MAX_BLOCK_SIZE / ALIGN];
    std::queue<Block> _block_persist;
    std::unordered_set<char*> _blocks;

    char* alloc_small(size_t size);
    char* alloc_large(size_t size);
    void dealloc_small(char* p, size_t size);
    void dealloc_large(char* p, size_t size);
    void refill(std::deque<Block>* que, size_t size);
    void free_expired_block();
};

} // namespace table

#endif