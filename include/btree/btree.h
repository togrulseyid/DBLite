//
// Created by mahammad on 6/8/17.
//

#ifndef DBLITE_BTREE_H
#define DBLITE_BTREE_H

#include <slottedpages/tid.h>
#include <slottedpages/segment.h>
#include <iostream>
#include <algorithm>
#include <string.h>
#include <stack>

template<class K, class CMP>
class BTree : Segment {
    CMP cmp;
    uint32_t num_elements;
    uint64_t root_pg_id;

    std::stack<uint64_t> free_pgs;

    uint64_t get_new_page(){
        if(free_pgs.size() > 0){
            uint64_t ret = free_pgs.top();
            free_pgs.pop();
            return ret;
        }
        return ++size;
    }

    class Node {
    protected:
        CMP cmp;
        uint32_t cnt;
        bool leaf;
    public:
        bool is_leaf() { return leaf; };

        virtual bool is_full() = 0;

        Node(bool leaf) : leaf(leaf) { this->cnt = 0; };
    };

    class InnerNode : Node {
        static const int inner_size = (FRAME_SIZE - sizeof(Node) - 8) / (sizeof(K) + 8);
        K keys[inner_size];
        uint64_t child_pg_ids[inner_size + 1];

    public:
        InnerNode() : Node(false) {};

        InnerNode(uint64_t fir_child, uint64_t sec_child, K middle) : Node(false) {
            keys[0] = middle;
            child_pg_ids[0] = fir_child;
            child_pg_ids[1] = sec_child;
            this->cnt = 1;
        }

        void insert_key(K key, uint64_t fir, uint64_t sec);

        bool is_full() { return this->cnt == inner_size; }

        void split(BufferFrame *&fir, BufferFrame *&sec, K &key);

        uint64_t get_child(K key) {
            return child_pg_ids[get_idx(key)];
        };

        uint32_t get_idx(K key) {
            return std::lower_bound(keys, keys + this->cnt, key, this->cmp) - keys;
        };

        void print_dbg() {
            std::cout << "Inner printing: ";
            for (int i = 0; i < this->cnt; ++i)
                std::cout << keys[i] << " ";
            std::cout << std::endl;
        };

    };

    class LeafNode : Node {
        static const int leaf_size = (FRAME_SIZE - sizeof(Node)) / (sizeof(K) + sizeof(TID));
        K keys[leaf_size];
        TID tids[leaf_size];


    public:
        LeafNode() : Node(true) {};

        uint32_t get_cnt() { return this->cnt; };

        bool is_full() { return leaf_size == this->cnt; };

        K get_key(int idx) { return keys[idx]; };

        TID get_tid(int idx) {
            return tids[idx];
        }

        uint32_t get_idx(K key) {
            return std::lower_bound(keys, keys + this->cnt, key, this->cmp) - keys;
        }

        void insert_key(K key, TID tid);

        void split(BufferFrame *&fir, BufferFrame *&sec, K &key);

        bool remove(K key);

        void print_dbg() {
            std::cout << "Leaf printing: ";
            for (int i = 0; i < this->cnt; ++i)
                std::cout << keys[i] << " ";
            std::cout << std::endl;
        };

    };

public:
    uint32_t get_size();

    bool lookup(K key, TID &tid);

    void insert(K key, TID tid);

    bool erase(K key);

    BTree(BufferManager &bm, uint16_t segment_id) : Segment(segment_id, bm) {
        num_elements = 0;
        root_pg_id = 0;
        BufferFrame &bf = bm.fixPage(root_pg_id, true);
        new(bf.getData()) LeafNode();
        bm.unfixPage(bf, true);
    };

    void update_par(K key, K mid_element, BufferFrame *&l_child, BufferFrame *&r_child, BufferFrame *&par_node_bf,
                    BufferFrame *&cur_node_bf);

    void search_leaf(K key, LeafNode *&leaf_node, BufferFrame *&bf);

};

#endif //DBLITE_BTREE_H
