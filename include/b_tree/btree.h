//
// Created by mahammad on 6/8/17.
//

#ifndef DBLITE_BTREE_H
#define DBLITE_BTREE_H

#include <cstdint>
#include <slottedpages/tid.h>
#include <slottedpages/segment.h>

template<class K, class CMP>
class BTree : Segment {
    void search_leaf();

    uint32_t num_elements;
    uint64_t root_pg_id;

public:
    uint32_t get_size() { return num_elements; };

    void lookup(K key, TID &tid);

    void insert(K key);

    void erase(K key);

    BTree(BufferManager &bm, uint16_t segment_id) : Segment(segment_id, bm) {
        num_elements = 0;
        root_pg_id = 0;
    }
};

template<class K, class CMP>
class Node {
protected:
    CMP cmp;
    uint32_t cnt;
    bool leaf;
public:
    bool is_leaf();

    Node(bool leaf) : leaf(leaf) { cnt = 0; };
};

template<class K, class CMP>
class InnerNode : Node {
    static const int inner_size = (FRAME_SIZE - sizeof(Node) - 8) / (sizeof(K) + 8);
    K keys[inner_size];
    uint64_t child_pg_ids[inner_size + 1];

public:
    InnerNode() : Node(false) {};
    void insert();

    void remove();

    uint32_t get_idx(K key);

};

template<class K, class CMP>
class LeafNode : Node {
    static const int leaf_size = (FRAME_SIZE - sizeof(Node)) / (sizeof(K) + sizeof(TID));
    K keys[leaf_size];
    TID tids[leaf_size];


public:
    LeafNode() : Node(true) {};
    TID get_tid(K key);
    uint32_t get_idx(K key);
    void insert(K key);
    K split();
    bool remove(K key);

};

#endif //DBLITE_BTREE_H
