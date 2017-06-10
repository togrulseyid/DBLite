//
// Created by mahammad on 6/8/17.
//
#include <btree.h>
#include <vector>

template<class K, class CMP>
void BTree::insert(K key) {
    std::vector<BufferFrame*> frames;

    BufferFrame *cur_node_bf = this->bm.fixPage(root_pg_id, true);
    while(true){
        Node *node = static_cast<Node*>(cur_node_bf->getData());
        frames.push_back(cur_node_bf);
        if(node->is_leaf()){
            break;
        }
        InnerNode *inner_node = reinterpret_cast<InnerNode*>(node);
        cur_node_bf = bm.fixPage(inner_node->get_child(key), true);
    }
    bool full = false;
    int len = frames.size();
    LeafNode* leaf_node = static_cast<LeafNode*>(frames[len - 1]->getData());
    BufferFrame *fir_child;
    BufferFrame *sec_child;
    K middle;
    if(leaf_node->is_full()){
        full = true;
        fir_child = bm.fixPage(++size, true);
        sec_child = bm.fixPage(++size, true);

        middle = leaf_node->split(fir_child, sec_child, key);
        bm.unfixPage(frames[len-1]);
    }

    for(int i = len - 2; i > -1 && full; --i){
        InnerNode *inner_node = static_cast<InnerNode*>(frames[i]->getData());
        if(inner_node->is_full()){

        } else{
            full = false;
            inner_node->
        }
    }





    ++num_elements;
}

