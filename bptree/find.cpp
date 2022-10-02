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
    count_node = 0;
    int depth = height;
    Node * cur = root;
    while(depth--!=0){
        count_node ++;
        printInnerNode(cur);
        cout<<"\n";
        int idx = findChildForKey(cur, key);
        cur = reinterpret_cast<Node *> (cur->ptr[idx]);
    }
    count_node ++;
    printLeafNode(cur);
    cout<<"\n";
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
    count_node = 0;
    int depth = height;
    Node * cur = root;
    while(depth--!=0){
        if(!cur->is_leaf) printInnerNode(cur);
        else printLeafNode(cur);
        cout<<"\n\n";
        count_node ++;
        int idx = findSlotInNode(cur, low);
        cur = reinterpret_cast<Node *> (cur->ptr[idx]);
    }
    count_node ++;
    int idx = findSlotInNode(cur, low);
    assert(cur->key[idx]>=low);
    int cur_idx = idx;
    Node * cur_node = cur;
    list<int> ans;
    while(cur_node && cur_node->key[cur_idx]<=high){
        list<int> *cur_data = reinterpret_cast<list<int>*>(cur_node->ptr[cur_idx]);
        //cout<<"found record: "<<cur_data->size()<<", ele = "<<cur_node->key[cur_idx];
        for(int pos: *cur_data){
            ans.push_back(pos);
            //cout<<"\t"<<pos<<"\n";
        }
        if(cur_idx == cur_node->num_keys-1){
            count_node++;
            cur_node = reinterpret_cast<Node*> (cur_node->ptr[MAX]);
            printLeafNode(cur_node);
            cout<<"\n\n";
            cur_idx = 0;
        } else {
            cur_idx ++;
        }
    }
    return ans;
}
