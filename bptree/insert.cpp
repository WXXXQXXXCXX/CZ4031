#include "bptree.h"
#include <cmath>
#include <list>
#include <iostream>

using namespace std;


BPTree::BPTree() {
    height = 0;
    root = new Node(true);
}

void BPTree::insert(RecPtr rec, int key) {
    count_node =0;

    InsertResult res;
    bool split = false;
    if(height == 0) {
        split = insertInLeaf(root, key, &res, rec);
    } else {
        split = insertInInner(root, key, &res, height, rec);
    }
    if(split){
        height++;
        Node *oldRoot = root;
        root = new Node(false);
        root->num_keys = 1;
        root->key[0] = res.move_up;
        root->ptr[0] = oldRoot;
        root->ptr[1] = res.newNode;
        root->ptr[MAX] = nullptr;
    }
}

bool BPTree::insertInLeaf(Node *cur, int key,InsertResult* res, RecPtr rec_ptr){
    count_node++;
    bool split = false;
    int pos = findSlotInNode(cur, key);
    if(key == cur->key[pos]){
        list<int>* vals=reinterpret_cast<list<int>*>(cur->ptr[pos]);
        vals->push_back(rec_ptr.pos);
        return false;
    }
    if(cur->num_keys<MAX){
        insertToLeafNoSplit(cur, key, pos, rec_ptr);
    } else {
        split = true;

        int threshold = ceil((MAX+1)/2.0);
        bool insert_to_left = false;
        if(pos<threshold){
            threshold -= 1;
            insert_to_left = true;
        }
        res->newNode = new Node(true);
        res->newNode->num_keys = cur->num_keys-threshold;

        cur->num_keys = threshold;
        for(int i=0; i<res->newNode->num_keys; i++){
            res->newNode->key[i] = cur->key[threshold + i];
            res->newNode->ptr[i] = cur->ptr[threshold + i];
            cur->ptr[threshold + i] = nullptr;
        }
        res->newNode->ptr[MAX] = cur->ptr[MAX];
        cur->ptr[MAX] = res->newNode;

        if(insert_to_left){
            insertToLeafNoSplit(cur, key, pos, rec_ptr);
        } else {
            insertToLeafNoSplit(res->newNode, key, pos-threshold, rec_ptr);
        }
    }
    if(split){
        res->move_up = res->newNode->key[0];
    }

    return split;
}

bool BPTree::insertInInner(Node *cur, int key,InsertResult* res, int depth, RecPtr rec_ptr){
    count_node++;
    bool split = false;
    int pos = findChildForKey(cur, key);
    InsertResult child_res;
    bool child_split;
    if(depth == 1){
        child_split = insertInLeaf(reinterpret_cast<Node *>(cur->ptr[pos]), key, &child_res, rec_ptr);
    } else {
        child_split = insertInInner(reinterpret_cast<Node *>(cur->ptr[pos]), key, & child_res, depth-1, rec_ptr);
    }

    if(child_split) {
        int insert_pos = findSlotInNode(cur, child_res.move_up);
        if (cur->num_keys == MAX) {
            split = true;
            int threshold = ceil((MAX) / 2.0);
            Node *newNode = new Node(false);

            bool in_middle = threshold == insert_pos;

            int l, r;
            if(insert_pos>threshold){
                l = threshold +1;
            } else{
                l = threshold;
            }
            r = cur->num_keys - l;
            //newNode->num_keys = cur->num_keys - threshold;
            //cur->num_keys = threshold;
            newNode->num_keys = r;

            res->newNode = newNode;

            for(int i=0; i<r; i++){
                newNode->key[i] = cur->key[l+i];
                newNode->ptr[i] = cur->ptr[l+i];
            }

            newNode->ptr[newNode->num_keys] = cur->ptr[cur->num_keys];
            cur->num_keys = l;
            res -> newNode = newNode;
            if(in_middle){
                res->move_up = child_res.move_up;
                newNode->ptr[0] = child_res.newNode;
            } else if(insert_pos < threshold){
                res->move_up = cur->key[l-1];
                cur->num_keys -= 1;
                insertToInnerNoSplit(cur, child_res.move_up, insert_pos, child_res.newNode);
            } else {
                res->move_up = cur->key[l-1];
                cur->num_keys -= 1;
                insertToInnerNoSplit(newNode, child_res.move_up, insert_pos-l, child_res.newNode);
            }
        } else {
            insertToInnerNoSplit(cur, child_res.move_up, insert_pos, child_res.newNode);
        }

    }

    return split;
}



void insertToLeafNoSplit(Node *cur, int key, int pos, RecPtr ptr) {

    for(int i=cur->num_keys; i>pos; --i){
        cur->key[i] = cur->key[i-1];
        cur->ptr[i] = cur->ptr[i-1];
    }
    cur->num_keys ++;
    cur->key[pos] = key;
    auto newList = new list<int>;
    newList->push_back(ptr.pos);
    cur->ptr[pos] = newList;

}

void insertToInnerNoSplit(Node* cur, int key, int pos, Node *newNode){
    if(pos == cur->num_keys){
        cur->key[pos] = key;
        cur->ptr[pos+1] = newNode;
        cur->num_keys++;
    } else {
        cur->ptr[cur->num_keys+1] = cur->ptr[cur->num_keys];
        for(int i=cur->num_keys; i>pos; i--){
            cur->ptr[i] = cur->ptr[i-1];
            cur->key[i] = cur->key[i-1];
        }
        cur->key[pos] = key;
        cur->ptr[pos+1] = newNode;
        cur->num_keys++;
    }
}