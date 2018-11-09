// Copyright (c) 2018, Wonter. All rights reserved.
// Use of this source code is governed by the BSD 3-Clause License,
// that can be found in the LICENSE file.

#ifndef TABLE_SKIPLIST_H
#define TABLE_SKIPLIST_H

#include "common.h"
#include "random.h"
#include "byte_array.h"
#include "comparator.h"
#include "memory_pool.h"

namespace table {

class SkipList {
TABLE_PRIVATE:
    struct Node;

TABLE_PUBLIC:
    class Iterator {
    TABLE_PUBLIC:
        Iterator();
        Iterator(Node* node);
        ~Iterator();

        // Returns true if the iterator point to a valid node.
        bool good();

        // Advances to the next position.
        // REQUIRES: good()
        void next();

        // Returns the key at the current position.
        // REQUIRES: good()
        const ByteArray& key();

        // Returns the value at the current position.
        // REQUIRES: good()
        const ByteArray& value();
    TABLE_PRIVATE:
        Node  *_node;
    };

    SkipList(Comparator* cmp, MemoryPool *pool);
    ~SkipList() = default;

    // Returns a iterator point to the first node.
    Iterator begin();

    // Returns a iterator point to the new node.
    // Returns a bad iterator if there is a duplicate key.
    Iterator insert(const ByteArray& key, const ByteArray& value);

    // Returns a iterator point to the node with node.key == key.
    // Returns a bad iterator if there is no such node.
    Iterator update(const ByteArray& key, const ByteArray& new_value);

    // Returns a iterator point to the node with node.key == key.
    // Returns a bad iterator if there is no such node.
    Iterator lookup(const ByteArray& key);

    // Returns false if there is no such node.
    bool remove(const ByteArray& key);

    // Non-copying
    SkipList(const SkipList&) = delete;
    SkipList& operator=(const SkipList&) = delete;

#ifdef TABLE_DEBUG
    std::string serialize();
#endif

TABLE_PRIVATE:
    struct Node {
        Node(int h, const ByteArray& k, const ByteArray& v) : height(h), key(k), value(v) {
            std::fill_n(next, h, nullptr);
        }

        int   height;
        ByteArray key;
        ByteArray value;
        Node* next[1];
    };

    enum {
        MAX_HEIGHT         = 16,
        RANDOM_SEED        = 0xBADC0FFE,
        GROWTH_PROBABILITY = 4,
    };

    int          _height;
    Node        *_head;
    Random       _rand;
    Comparator  *_cmp;
    MemoryPool  *_pool;

    int random_height();
    Node* first_greater_or_equal(const ByteArray& key, Node **prev);

    Node* new_node(const ByteArray& key, const ByteArray& value, int height);
    void  delete_node(Node* node);

    void publish_node(Node* node, Node** prev);
    void remove_node(Node* node, Node** prev);
};

} // namespace table

#endif