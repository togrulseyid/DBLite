//
// Created by mahammad on 6/8/17.
//

#include <btree/btree.h>

template<class K, class CMP>
uint32_t BTree<K, CMP>::get_size() { return num_elements; };

template<class K, class CMP>
void BTree<K, CMP>::search_leaf(K key, LeafNode *&leaf_node, BufferFrame *&bf) {
    BufferFrame *cur_node_bf = &bm.fixPage(root_pg_id, true);
    Node *cur_node = static_cast<Node *>(cur_node_bf->getData());
    while (!cur_node->is_leaf()) {
        InnerNode *inner_node = reinterpret_cast<InnerNode *>(cur_node);
        uint64_t next_pg_id = inner_node->get_child(key);
        bm.unfixPage(*cur_node_bf, true);
        cur_node_bf = &bm.fixPage(next_pg_id, true);
        cur_node = static_cast<Node *>(cur_node_bf->getData());
    }

    leaf_node = reinterpret_cast<LeafNode *>(cur_node);
    bf = cur_node_bf;
}

template<class K, class CMP>
void BTree<K, CMP>::update_par(K key, K mid_element, BufferFrame *&new_frame_l, BufferFrame *&new_frame_r,
                               BufferFrame *&par_node_bf,
                               BufferFrame *&cur_node_bf) {
    if (par_node_bf == NULL) {
        root_pg_id = get_new_page();
        BufferFrame *root_bf = &bm.fixPage(root_pg_id, true);
        new(root_bf->getData()) InnerNode(new_frame_l->getPageId(), new_frame_r->getPageId(), mid_element);
        par_node_bf = root_bf;
    } else {
        InnerNode *par_node = static_cast<InnerNode *>(par_node_bf->getData());
        par_node->insert_key(mid_element, new_frame_l->getPageId(), new_frame_r->getPageId());
    }

    bm.unfixPage(*cur_node_bf, true);
    free_pgs.push(cur_node_bf->getPageId());

    if (cmp(mid_element, key)) {
        bm.unfixPage(*new_frame_l, true);
        cur_node_bf = new_frame_r;
    } else {
        bm.unfixPage(*new_frame_r, true);
        cur_node_bf = new_frame_l;
    }
}

template<class K, class CMP>
bool BTree<K, CMP>::lookup(K key, TID &tid) {
    LeafNode *leaf_node;
    BufferFrame *bf;

    search_leaf(key, leaf_node, bf);
    uint32_t idx = leaf_node->get_idx(key);
    bm.unfixPage(*bf, true);
    if (idx == leaf_node->get_cnt() || cmp(key, leaf_node->get_key(idx))) {
        return false;
    }
    tid = leaf_node->get_tid(idx);
    return true;
}

template<class K, class CMP>
bool BTree<K, CMP>::erase(K key) {
    LeafNode *leaf_node;
    BufferFrame *bf;

    search_leaf(key, leaf_node, bf);
    uint32_t idx = leaf_node->get_idx(key);
    bm.unfixPage(*bf, true);
    if (idx == leaf_node->get_cnt() || this->cmp(key, leaf_node->get_key(idx)))
        return false;
    leaf_node->remove(key);
    --this->num_elements;
    return true;
}

template<class K, class CMP>
void BTree<K, CMP>::insert(K key, TID tid) {
    BufferFrame *cur_node_bf = &this->bm.fixPage(this->root_pg_id, true);
    Node *node = static_cast<Node *>(cur_node_bf->getData());
    BufferFrame *par_node_bf = NULL;

    while (!node->is_leaf()) {
        if (node->is_full()) {
            BufferFrame *new_frame_l = &this->bm.fixPage(get_new_page(), true);
            BufferFrame *new_frame_r = &this->bm.fixPage(get_new_page(), true);
            new(new_frame_l->getData()) InnerNode();
            new(new_frame_r->getData()) InnerNode();

            InnerNode *inner_node = reinterpret_cast<InnerNode *>(node);

            K mid_element;
            inner_node->split(new_frame_l, new_frame_r, mid_element);
            update_par(key, mid_element, new_frame_l, new_frame_r, par_node_bf, cur_node_bf);

            node = static_cast<Node *>(cur_node_bf->getData());
        } else {
            InnerNode *inner_node = reinterpret_cast<InnerNode *>(node);

            BufferFrame *next_child = &this->bm.fixPage(inner_node->get_child(key), true);

            if (par_node_bf != NULL)
                this->bm.unfixPage(*par_node_bf, true);
            par_node_bf = cur_node_bf;
            cur_node_bf = next_child;
            node = static_cast<Node *>(cur_node_bf->getData());
        }

    }

    if (node->is_full()) {
        BufferFrame *new_frame_l = &this->bm.fixPage(get_new_page(), true);
        BufferFrame *new_frame_r = &this->bm.fixPage(get_new_page(), true);
        new(new_frame_l->getData()) LeafNode();
        new(new_frame_r->getData()) LeafNode();
        LeafNode *leaf_node = reinterpret_cast<LeafNode *>(node);
        K mid_element;
        leaf_node->split(new_frame_l, new_frame_r, mid_element);

        update_par(key, mid_element, new_frame_l, new_frame_r, par_node_bf, cur_node_bf);
        node = static_cast<Node *>(cur_node_bf->getData());
    }

    LeafNode *leaf_node = reinterpret_cast<LeafNode *>(node);
    if (par_node_bf != NULL)
        this->bm.unfixPage(*par_node_bf, true);

    leaf_node->insert_key(key, tid);
    ++this->num_elements;
    this->bm.unfixPage(*cur_node_bf, true);
}

template<class K, class CMP>
void BTree<K, CMP>::InnerNode::insert_key(K key, uint64_t fir, uint64_t sec) {
    int idx = get_idx(key);
    if (idx < this->cnt && !this->cmp(key, keys[idx])) {
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
    int mid_idx = (this->cnt + 1) / 2 - 1;
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
    if (idx < this->cnt && !this->cmp(key, keys[idx])) {
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
    int mid_idx = (this->cnt + 1) / 2 - 1;
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
bool BTree<K, CMP>::LeafNode::remove(K key) {
    uint32_t idx = get_idx(key);
    if (idx == this->get_cnt() || this->cmp(key, keys[idx]))
        return false;
    memmove(keys + idx, keys + idx + 1, sizeof(K) * (this->cnt - idx - 1));
    memmove(tids + idx, tids + idx + 1, sizeof(TID) * (this->cnt - idx - 1));
    this->cnt--;
    //std::cout<<"remove ";
    //for(int i = 0; i < this->cnt; ++i)
    //    std::cout<<keys[i]<<" ";
    //std::cout<<std::endl;
    return true;
}