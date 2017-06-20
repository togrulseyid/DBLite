//
// Created by mahammad on 6/8/17.
//

#include <btree/btree.h>

template<class K, class CMP>
uint32_t BTree<K, CMP>::get_size() { return num_elements; };

template<class K, class CMP>
void BTree<K, CMP>::search_leaf(K key, LeafNode *&leaf_node, BufferFrame *&bf) {
    BufferFrame *cur_node_bf = &bm.fixPage(root_pg_id, true); // starting from root
    Node *cur_node = static_cast<Node *>(cur_node_bf->getData());
    while (!cur_node->is_leaf()) { // while it is InnerNode, we should go through till the leaf node
        InnerNode *inner_node = reinterpret_cast<InnerNode *>(cur_node);
        uint64_t next_pg_id = inner_node->get_child(key); // finds the page id of the corresponding following level node
        bm.unfixPage(*cur_node_bf, true);
        cur_node_bf = &bm.fixPage(next_pg_id, true);
        cur_node = static_cast<Node *>(cur_node_bf->getData());
    }

    leaf_node = reinterpret_cast<LeafNode *>(cur_node);
    bf = cur_node_bf;
}

template<class K, class CMP>
bool BTree<K, CMP>::lookup(K key, TID &tid) {
    LeafNode *leaf_node;
    BufferFrame *bf;

    search_leaf(key, leaf_node, bf); // getting the leaf node which is the last in the search path
    uint32_t idx = leaf_node->get_idx(key); // getting the index on that leaf node
    bm.unfixPage(*bf, true);
    if (idx == leaf_node->get_cnt() || cmp(key, leaf_node->get_key(idx))) // checks whether it is on the key list
        return false;

    tid = leaf_node->get_tid(idx);
    return true;
}

template<class K, class CMP>
bool BTree<K, CMP>::erase(K key) {
    LeafNode *leaf_node;
    BufferFrame *bf;

    search_leaf(key, leaf_node, bf); // getting the leaf node which is the last in the search path
    uint32_t idx = leaf_node->get_idx(key); // getting the index on that leaf node
    bm.unfixPage(*bf, true);
    if (idx == leaf_node->get_cnt() || this->cmp(key, leaf_node->get_key(idx)))// checks whether it is on the key list
        return false;
    leaf_node->remove(key); // it is on the key list, to remove the key from the leaf node
    --this->num_elements;
    return true;
}

template<class K, class CMP>
void BTree<K, CMP>::insert(K key, TID tid) {
    BufferFrame *cur_node_bf = &this->bm.fixPage(this->root_pg_id, true); // starting from the root
    Node *node = static_cast<Node *>(cur_node_bf->getData());
    BufferFrame *par_node_bf = NULL;

    while (!node->is_leaf()) { // it is Inner Node
        if (node->is_full()) { // it is full
            // to create two new InnerNode which are created for splitting
            BufferFrame *new_frame_l = &this->bm.fixPage(get_new_page(), true);
            BufferFrame *new_frame_r = &this->bm.fixPage(get_new_page(), true);
            new(new_frame_l->getData()) InnerNode();
            new(new_frame_r->getData()) InnerNode();

            InnerNode *inner_node = reinterpret_cast<InnerNode *>(node);

            K mid_element;
            inner_node->split(new_frame_l, new_frame_r, mid_element); // splits inner node and assigns to the new nodes
            update_par(key, mid_element, new_frame_l, new_frame_r, par_node_bf, cur_node_bf); // adding middle element to the upper level inner node

            node = static_cast<Node *>(cur_node_bf->getData());
        } else { // it is not full
            InnerNode *inner_node = reinterpret_cast<InnerNode *>(node);

            BufferFrame *next_child = &this->bm.fixPage(inner_node->get_child(key), true); // finds the corresponding next level node

            if (par_node_bf != NULL)
                this->bm.unfixPage(*par_node_bf, true);
            par_node_bf = cur_node_bf;
            cur_node_bf = next_child;
            node = static_cast<Node *>(cur_node_bf->getData());
        }

    }

    if (node->is_full()) { // leaf node is full, splitting
        BufferFrame *new_frame_l = &this->bm.fixPage(get_new_page(), true);
        BufferFrame *new_frame_r = &this->bm.fixPage(get_new_page(), true);
        new(new_frame_l->getData()) LeafNode();
        new(new_frame_r->getData()) LeafNode();
        LeafNode *leaf_node = reinterpret_cast<LeafNode *>(node);
        K mid_element;
        leaf_node->split(new_frame_l, new_frame_r, mid_element); // splits the leaf node

        update_par(key, mid_element, new_frame_l, new_frame_r, par_node_bf, cur_node_bf); // update the middle key
        node = static_cast<Node *>(cur_node_bf->getData());
    }

    LeafNode *leaf_node = reinterpret_cast<LeafNode *>(node);
    if (par_node_bf != NULL)
        this->bm.unfixPage(*par_node_bf, true);

    leaf_node->insert_key(key, tid); // inserting to the corresponding leaf node
    ++this->num_elements;
    this->bm.unfixPage(*cur_node_bf, true);
}

template<class K, class CMP>
void BTree<K, CMP>::update_par(K key, K mid_element, BufferFrame *&new_frame_l, BufferFrame *&new_frame_r,
                               BufferFrame *&par_node_bf,
                               BufferFrame *&cur_node_bf) {
    if (par_node_bf == NULL) { // previously BTree only contained one leaf node, where height was 1
        // getting new root
        root_pg_id = get_new_page();
        BufferFrame *root_bf = &bm.fixPage(root_pg_id, true);
        new(root_bf->getData()) InnerNode(new_frame_l->getPageId(), new_frame_r->getPageId(), mid_element);
        par_node_bf = root_bf;
    } else {
        // inserting
        InnerNode *par_node = static_cast<InnerNode *>(par_node_bf->getData());
        par_node->insert_key(mid_element, new_frame_l->getPageId(), new_frame_r->getPageId()); // inserting the middle element which refers left and right
    }

    bm.unfixPage(*cur_node_bf, true); // here to remove previous node, because already splitted
    free_pgs.push(cur_node_bf->getPageId()); // because we remove, it is a free page and we can use it afterwards

    // finds the way for the next level, if middle element is less than key, then should go right, otherwise left
    if (cmp(mid_element, key)) {
        bm.unfixPage(*new_frame_l, true);
        cur_node_bf = new_frame_r;
    } else {
        bm.unfixPage(*new_frame_r, true);
        cur_node_bf = new_frame_l;
    }
}

template<class K, class CMP>
void BTree<K, CMP>::InnerNode::insert_key(K key, uint64_t fir, uint64_t sec) {
    int idx = get_idx(key);
    if (idx < this->cnt && !this->cmp(key, keys[idx])) { // if it is already in the BTree, just update children page ids
        child_pg_ids[idx] = fir;
        child_pg_ids[idx + 1] = sec;
        return;
    }
    memmove(keys + idx + 1, keys + idx, sizeof(K) * (this->cnt - idx));
    memmove(child_pg_ids + idx + 1, child_pg_ids + idx, sizeof(uint64_t) * (this->cnt - idx + 1));
    keys[idx] = key;
    child_pg_ids[idx] = fir;
    child_pg_ids[idx + 1] = sec;
    ++this->cnt;
}

template<class K, class CMP>
void BTree<K, CMP>::InnerNode::split(BufferFrame *&fir, BufferFrame *&sec, K &key) {
    int mid_idx = (this->cnt + 1) / 2 - 1; // takes the middle element
    // create and fills two new nodes for splitting
    InnerNode *left = static_cast<InnerNode *>(fir->getData());
    InnerNode *right = static_cast<InnerNode *>(sec->getData());
    left->cnt = mid_idx;
    memcpy(left->keys, this->keys, sizeof(K) * left->cnt);
    memcpy(left->child_pg_ids, this->child_pg_ids, sizeof(uint64_t) * (left->cnt + 1));

    right->cnt = this->cnt - left->cnt - 1;
    memcpy(right->keys, this->keys + left->cnt + 1, sizeof(K) * (right->cnt));
    memcpy(right->child_pg_ids, this->child_pg_ids + left->cnt + 1, sizeof(uint64_t) * (right->cnt + 1));

    key = keys[left->cnt];
}

template<class K, class CMP>
void BTree<K, CMP>::LeafNode::insert_key(K key, TID tid) {
    uint32_t idx = get_idx(key);
    if (idx < this->cnt && !this->cmp(key, keys[idx])) { // if it is already in the BTree, just update tid
        tids[idx] = tid;
        return;
    }
    memmove(keys + idx + 1, keys + idx, sizeof(K) * (this->cnt - idx));
    memmove(tids + idx + 1, tids + idx, sizeof(TID) * (this->cnt - idx));
    keys[idx] = key;
    tids[idx] = tid;
    this->cnt++;
}

template<class K, class CMP>
void BTree<K, CMP>::LeafNode::split(BufferFrame *&fir, BufferFrame *&sec, K &key) {
    int mid_idx = (this->cnt + 1) / 2 - 1; // takes the middle element
    // create and fills two new nodes for splitting
    LeafNode *left = static_cast<LeafNode *>(fir->getData());
    LeafNode *right = static_cast<LeafNode *>(sec->getData());

    left->cnt = mid_idx + 1;
    memcpy(left->keys, this->keys, sizeof(K) * left->cnt);
    memcpy(left->tids, this->tids, sizeof(TID) * left->cnt);

    right->cnt = this->cnt - left->cnt;
    memcpy(right->keys, this->keys + left->cnt, sizeof(K) * right->cnt);
    memcpy(right->tids, this->tids + left->cnt, sizeof(TID) * right->cnt);
    key = left->keys[left->cnt - 1];
}

template<class K, class CMP>
bool BTree<K, CMP>::LeafNode::remove(K key) { // removes element from leaf node
    uint32_t idx = get_idx(key);
    if (idx == this->get_cnt() || this->cmp(key, keys[idx])) // key is not in the key list
        return false;
    memmove(keys + idx, keys + idx + 1, sizeof(K) * (this->cnt - idx - 1));
    memmove(tids + idx, tids + idx + 1, sizeof(TID) * (this->cnt - idx - 1));
    this->cnt--;
    return true;
}