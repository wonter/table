// Copyright (c) 2018, Wonter. All rights reserved.
// Use of this source code is governed by the BSD 3-Clause License,
// that can be found in the LICENSE file.

#include "skiplist.h"

namespace table {

SkipList::Iterator::Iterator() : _node(nullptr) {  }
SkipList::Iterator::Iterator(Node* node) : _node(node) {  }
SkipList::Iterator::~Iterator() {  }
void SkipList::Iterator::next() { _node = _node->next[0]; }
bool SkipList::Iterator::good() { return _node != nullptr; }
const ByteArray& SkipList::Iterator::key() { return _node->key; }
const ByteArray& SkipList::Iterator::value() { return _node->value; }

SkipList::SkipList(Comparator* cmp, MemoryPool *pool) :
        _height(1), _head(nullptr), _rand(RANDOM_SEED), _cmp(cmp), _pool(pool) {
    _head = new_node("head", "head", MAX_HEIGHT);
}

SkipList::Iterator SkipList::begin() {
    return Iterator(_head->next[0]);
}

SkipList::Iterator SkipList::insert(const ByteArray& key, const ByteArray& value) {
    Node *prev[MAX_HEIGHT] = {nullptr};
    Node *node = first_greater_or_equal(key, prev);

    if (node && _cmp->compare(node->key, key) == 0) {
        return Iterator(nullptr);
    }

    int new_height = random_height();
    Node *insert_node = new_node(key, value, new_height);

    if (new_height > _height) {
        for (int i = _height; i < new_height; ++i) {
            prev[i] = _head;
        }
        _height = new_height;
    }

    publish_node(insert_node, prev);

    return Iterator(insert_node);
}

SkipList::Iterator SkipList::update(const ByteArray& key, const ByteArray& new_value) {
    Node *prev[MAX_HEIGHT] = {nullptr};
    Node *node = first_greater_or_equal(key, prev);
    if (node && node != _head && _cmp->compare(node->key, key) == 0) {
        // insert a new node with new_value and remove the old node
        Node *insert_node = new_node(node->key, new_value, node->height);
        Node *temp[MAX_HEIGHT];
        std::fill_n(temp, node->height, node);
        publish_node(insert_node, temp);

        remove_node(node, prev);
        delete_node(node);
        return Iterator(insert_node);
    }
    return Iterator(nullptr);
}

SkipList::Iterator SkipList::lookup(const ByteArray& key) {
    Node *node = first_greater_or_equal(key, nullptr);
    if (node && _cmp->compare(node->key, key) == 0) {
        return Iterator(node);
    }
    return Iterator(nullptr);
}

bool SkipList::remove(const ByteArray& key) {
    Node *prev[MAX_HEIGHT] = {nullptr};
    Node *node = first_greater_or_equal(key, prev);
    if (node && _cmp->compare(node->key, key) == 0) {
        remove_node(node, prev);
        delete_node(node);
        return true;
    }
    return false;
}

int SkipList::random_height() {
    int height = 1;
    while (height < static_cast<int>(MAX_HEIGHT) &&
                (_rand.rand() & (GROWTH_PROBABILITY - 1)) == 0) {
        ++height;
    }
    return height;
}

SkipList::Node* SkipList::first_greater_or_equal(const ByteArray& key, Node** prev) {
    int height = _height - 1;
    Node *prev_node = _head;

    while (height >= 0) {
        Node *next_node = prev_node->next[height];
        while (next_node && _cmp->compare(next_node->key, key) < 0) {
            prev_node = next_node;
            next_node = next_node->next[height];
        }

        if (prev) {
            prev[height] = prev_node;
        }
        if (height == 0) {
            break;
        }
        --height;
    }

    return prev_node->next[0];
}

SkipList::Node* SkipList::new_node(const ByteArray& key, const ByteArray& value, int height) {
    ByteArray new_key(_pool->dup(key.data(), key.size()), key.size());
    ByteArray new_value(_pool->dup(value.data(), value.size()), value.size());

    void *p = _pool->alloc(sizeof(Node) + sizeof(Node*) * (height - 1));
    Node *node = new (p) Node(height, new_key, new_value);
    return node;
}

void  SkipList::delete_node(Node* node) {
    _pool->dealloc(node->key.data(), node->key.size());
    _pool->dealloc(node->value.data(), node->value.size());
    _pool->dealloc(reinterpret_cast<char*>(node),
        sizeof(Node) + sizeof(Node*) * (node->height - 1));
}

void SkipList::publish_node(Node* node, Node** prev) {
    for (int i = 0; i < node->height; ++i) {
        // ensure order consistency
        atomic_thread_fence(std::memory_order_acquire);

        node->next[i] = prev[i]->next[i];
        prev[i]->next[i] = node;

        atomic_thread_fence(std::memory_order_release);
    }
}

void SkipList::remove_node(Node* node, Node** prev) {
    for (int i = node->height - 1; i >= 0; --i) {
        prev[i]->next[i] = node->next[i];
    }
}

#ifdef TABLE_DEBUG
std::string SkipList::serialize() {
    std::stringstream sstr;

    int height = _height - 1;
    while (height >= 0) {
        sstr << "height " << height << ": ";
        Node *p = _head->next[height];
        while (p) {
            sstr << p->key.data() << "    ";
            p = p->next[height];
        }
        if (height) {
            sstr << std::endl;
        }
        --height;
    }

    return sstr.str();
}
#endif

} // namespace table
