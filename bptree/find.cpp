#include "bptree.h"
#include <iostream>

using namespace std;

int findSlotInNode(Node *node, int key) {
    int idx = 0;
    while (idx < node->num_keys && key > node->key[idx]) {
        idx++;
    }
    return idx;
}

int findChildForKey(Node *node, int key){
    int idx = 0;
    while(idx<node->num_keys){
        if(key < node->key[idx]){
            return idx;
        }

        if(key == node->key[idx]){
            return idx+1;
        }
        idx++;
    }
    return node->num_keys;
}

list<int> BPTree::find(int key) {
    int depth = height;
    Node * cur = root;
    while(depth--!=0){
        int idx = findChildForKey(cur, key);
        cur = reinterpret_cast<Node *> (cur->ptr[idx]);
    }
    int idx = findSlotInNode(cur, key);
    list<int> ans;
    if(key == cur->key[idx]){
        list<int>* l = reinterpret_cast<list<int>*>(cur->ptr[idx]);
        cout<<"found record: "<<l->size()<<"\n";
        for(int pos: *l){

            ans.push_back(pos);
        }
    }
    return ans;
}

list<int> BPTree::findRange(int low, int high) {
    int depth = height;
    Node * cur = root;
    while(depth--!=0){
        int idx = findSlotInNode(cur, low);
        cur = reinterpret_cast<Node *> (cur->ptr[idx]);
    }
    int idx = findSlotInNode(cur, low);
    assert(cur->key[idx]>=low);
    int cur_idx = idx;
    Node * cur_node = cur;
    list<int> ans;
    while(cur_node && cur_node->key[cur_idx]<=high){
        list<int> *cur_data = reinterpret_cast<list<int>*>(cur->ptr[idx]);
        cout<<"found record: "<<cur_data->size()<<", ele = "<<cur_node->key[cur_idx]<<"\n";
        for(int pos: *cur_data){
            ans.push_back(pos);
            cout<<pos<<"\t";
        }
        if(cur_idx == cur_node->num_keys-1){
            cur_node = reinterpret_cast<Node*> (cur_node->ptr[MAX]);
            cur_idx = 0;
        } else {
            cur_idx ++;
        }
    }
    return ans;
}
