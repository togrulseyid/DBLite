//
// Created by mahammad on 6/8/17.
//

#ifndef DBLITE_BTREE_H
#define DBLITE_BTREE_H

#include <cstdint>
#include <algorithm>
#include <string.h>
#include <slottedpages/tid.h>
#include <slottedpages/segment.h>

template<class K, class CMP>
class BTree : Segment {
    void search_leaf();

    uint32_t num_elements;
    uint64_t root_pg_id;

    class Node {
    protected:
        CMP cmp;
        uint32_t cnt;
        bool leaf;
    public:
        bool is_leaf();

        virtual bool is_full() = 0;

        Node(bool leaf) : leaf(leaf) { cnt = 0; };
    };

    class InnerNode : Node {
        static const int inner_size = (FRAME_SIZE - sizeof(Node) - 8) / (sizeof(K) + 8);
        K keys[inner_size];
        uint64_t child_pg_ids[inner_size + 1];

    public:
        InnerNode() : Node(false) {};

        void insert();

        bool is_full();

        void remove();

        void split(K &middle, BufferFrame *&sec_part);

        uint64_t get_child(K key);

        uint32_t get_idx(K key);

    };

    class LeafNode : Node {
        static const int leaf_size = (FRAME_SIZE - sizeof(Node)) / (sizeof(K) + sizeof(TID));
        K keys[leaf_size];
        TID tids[leaf_size];


    public:
        LeafNode() : Node(true) {};

        bool is_full() { return leaf_size == cnt; };

        bool get_tid(K key, TID &tid) {
            int idx = get_idx(key);
            if (idx == cnt || cmp(key, keys[idx]))
                return false;
            tid = tids[idx];
            return true;
        }

        uint32_t get_idx(K key) {
            return std::lower_bound(keys, keys + this->cnt, key, this->cmp) - keys;
        }

        void insert(K key, TID tid) {
            int idx = get_idx(key);
            if (idx < this->cnt && !this->cmp(key, keys[idx])) {
                tids[idx] = tid;
                return;
            }

            memmove(keys + idx + 1, keys + idx, sizeof(K) * (cnt - idx));
            memmove(tids + idx + 1, tids + idx, sizeof(TID) * (cnt - idx));
            keys[idx] = key;
            tids[idx] = tid;
        };

        K split(BufferFrame &fir, BufferFrame &sec, K key) {
            K keys_temp[cnt + 1];
            TID tids_temp[cnt + 1];
            int idx = get_idx(key);
            std::move(keys, keys + idx, keys_temp);
            keys_temp[idx] = key;
            std::move(keys + idx, keys + cnt, keys_temp + idx + 1);

            std::move(tids, tids + idx, tids_temp);
            keys_temp[idx] = key;
            std::move(tids + idx, tids + cnt, tids_temp + idx + 1);

            LeafNode *left = static_cast<LeafNode *>(fir.getData());
            LeafNode *right = static_cast<LeafNode *>(sec.getData());
            cnt++;
            int mid = cnt / 2;
            left->cnt = cnt - mid;
            right->cnt = mid;

            std::move(keys_temp, keys_temp + left->cnt, left->keys);
            std::move(tids_temp, tids_temp + left->cnt, left->tids);

            std::move(keys_temp + left->cnt, keys_temp + left->cnt + mid, right->keys);
            std::move(tids_temp + left->cnt, tids_temp + left->cnt + mid, right->tids);
            return keys[left->cnt - 1];
        }

        bool remove(K key) {
            int idx = get_idx(key);
            if(idx == cnt || cmp(key, keys[idx]))
                return false;
            memmove(keys + idx, keys + idx + 1, sizeof(K) * (cnt - idx - 1));
            memmove(tids + idx, tids + idx + 1, sizeof(TID) * (cnt - idx - 1));
            cnt --;
            return true;
        }

    };

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


#endif //DBLITE_BTREE_H
