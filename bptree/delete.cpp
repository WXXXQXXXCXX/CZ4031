#include <iostream>
#include "bptree.h"

list<int> BPTree::del(int key) {
    target = nullptr;
    int alt_idx = -1;
    bool del_after_child = false;
    list<int> ans;
    count_node = 0;
    Node *newNode = deleteAndBalance(key,&alt_idx,&del_after_child,&ans,root,nullptr,nullptr,nullptr,nullptr,nullptr);
    if(newNode!=nullptr && root->num_keys == 0){
        collapseRoot(root, newNode);
    }
    return ans;
}

Node* BPTree::deleteAndBalance(
        int key, int *alt_idx, bool * need_remove,list<int>* ans,
        Node *cur, Node *left, Node *right,
        Node *l_parent, Node *r_parent, Node * cur_parent){
    // current node, may or may not need to delete a key
    Node* newCur;
    count_node ++;
    bool need_balance = false;
    int alt_after_child = -1;
    bool del_after_child = false;

    int idx;
    if(cur->is_leaf){
        idx = findSlotInNode(cur, key);
    } else{
        idx = findChildForKey(cur, key);
        if(idx > 0 && cur->key[idx-1]==key){
            target = &cur->key[idx-1];
        }
    }

    Node *newNode = reinterpret_cast<Node *>(cur->ptr[idx]);

    // if not the leaf, call delete and balance on child
    if(!cur->is_leaf){
        Node* l;
        Node* r;
        Node* lp;
        Node* rp;
        //if target child is first one, immediate left should be the last child in current left
        if(idx == 0){
            l = left == nullptr ? nullptr : getLastNode(left);
            lp = l_parent;
        } else {
            l = reinterpret_cast<Node *>(cur->ptr[idx-1]);
            lp = cur;
        }

        //similar for the last child
        if(idx == cur->num_keys){
            r = right == nullptr ? nullptr: getFirstNode(right);
            rp = r_parent;
        } else {
            r = reinterpret_cast<Node *>(cur->ptr[idx+1]);
            rp = cur;
        }
        newCur = deleteAndBalance(key, &alt_after_child, &del_after_child, ans, newNode, l, r, lp, rp, cur);
    } else if(idx<cur->num_keys && cur->key[idx]==key){
        //is leaf and found target
        newCur = newNode;
        if(nodeHasMinKey(cur)){
            need_balance = true;
        }
    } else {
        newCur = nullptr;
        *need_remove = false;
        need_balance = false;
    }

    if(cur->is_leaf){
        if(nodeHasMinKey(cur)){
            need_balance = true;
        }

        if(cur->key[idx]==key){
            list<int> *l = reinterpret_cast<list<int>*>(cur->ptr[idx]);
            for(int pos: *l){
                ans->push_back(pos);
            }
            removeKey(cur, idx);
            *alt_idx = cur->key[0];
            if(target!= nullptr){
                *target = cur->key[0];
            }
        } else {
            need_balance = false;
        }

    } else if(newCur !=  nullptr) {

        // merge return the right node, if merge with left,
        if(newCur == newNode && del_after_child){
            if(nodeHasMinKey(cur)){
                need_balance = true;
            }
            removeKey(cur, idx - 1);
        } else if(newCur != newNode && del_after_child) {
            if(nodeHasMinKey(cur)){
                need_balance = true;
            }
            removeKey(cur, idx);
        }
        //printInnerNode(cur);

    }


    bool r_min;
    bool l_min;
    bool no_l;
    bool no_r;

    if(need_balance){

        no_l = left == nullptr;
        no_r = right == nullptr;
        l_min = nodeHasMinKey(left);
        r_min = nodeHasMinKey(right);

        //1. if node is root:
        if(no_l && no_r){
            if(cur->is_leaf){
                newNode = nullptr;
            } else {
                newNode = getFirstNode(cur);
            }
        }
        //2. when borrow from both sides are not possible, then must merge
        else if((no_l||l_min) && (no_r||r_min)){
            if(l_parent != cur_parent){
                newNode = mergeNode(cur, right, r_parent);
                count_merge ++;
            } else {
                newNode = mergeNode(left, cur, l_parent);
                count_merge++;
            }
            *need_remove = true;
        }

        //3. can merge with left or borrow from right
        else if(!no_l && l_min && !no_r && !r_min) {
            if(r_parent != cur_parent){
                newNode =mergeNode(left, cur, l_parent);
                count_merge++;
                *need_remove = true;
            } else {
                newNode =shift(cur, right, r_parent);
            }
        }
        //4. can merge with right or borrow from left
        else if(!no_l && !l_min && !no_r && r_min) {
            if(l_parent != cur_parent) {
                newNode =mergeNode(cur, right, r_parent);
                count_merge++;
                *need_remove = true;
            } else {
                newNode =shift(left, cur, l_parent);

            }
        }
        //5. can shift from both left and right
        else if(l_parent == r_parent){
            if(left->num_keys<=right->num_keys){
                newNode =shift(cur, right, r_parent);
            } else {
                newNode =shift(left, cur, l_parent);


            }

        }

        else {
            if(l_parent == cur_parent){
                newNode = shift(left, cur, l_parent);

            } else {
                newNode = shift(cur, right, r_parent);
            }
        }
    } else {
        newNode = nullptr;
    }

    return newNode;
}

void BPTree::removeKey(Node *node, int idx){
    node -> key[idx] = 0;
    bool is_internal = !node->is_leaf;
    for(int i=idx; i<node->num_keys; i++){
        node -> key[i] = node -> key[i+1];
        node -> ptr[i+(is_internal)] = node -> ptr[i+1+(is_internal)];

    }
    node -> num_keys --;
}

Node *BPTree::shift(Node *left, Node *right, Node *parent) {
    //shift from right to left
    int num_shifts;
    int new_anchor;
    int anchor_slot;
    bool is_leaf = left->is_leaf;
    //right to left
    if(left->num_keys<=right->num_keys){
        num_shifts = (right->num_keys - left->num_keys) >> 1;
        new_anchor = right->key[num_shifts-(!is_leaf)];

        anchor_slot = findSlotInNode(parent, new_anchor) - 1;

        if(!is_leaf){

            left->num_keys ++;
            right->num_keys --;
            left->key[left->num_keys-1] = parent->key[anchor_slot];
            left->ptr[left->num_keys+num_shifts-1] = right->ptr[num_shifts-1];

        }
        parent->key[anchor_slot] = new_anchor;
        int L = left->num_keys + num_shifts-1-(!is_leaf);
        int R = is_leaf?num_shifts-1:num_shifts-2;
        for(; R>=0; R--, L--){
            left->num_keys++;
            right->num_keys--;
            left->key[L] = right->key[R];
            left->ptr[L] = right->ptr[R];
        }


        for(int i=0; i<right->num_keys; i++){
            right->key[i] = right->key[i+num_shifts];
            right->ptr[i] = right->ptr[i+num_shifts];
        }
        if(!is_leaf){
            right->ptr[right->num_keys] = right->ptr[right->num_keys+num_shifts];
        }
    } else {
        // shift from left to right
        num_shifts = (left->num_keys - right->num_keys) >> 1;
        int new_left_key_num = left->num_keys - num_shifts;
        if(!left->is_leaf)
            right->ptr[right->num_keys+num_shifts] = right->ptr[right->num_keys];
        for(int i=right->num_keys-1; i>=0; i--){
            right->key[i+num_shifts] = right->key[i];
            right->ptr[i+num_shifts] = right->ptr[i];
        }
        new_anchor = left->key[new_left_key_num];
        anchor_slot = findSlotInNode(parent, new_anchor) ;
        if(!is_leaf) {
            left->num_keys --;
            right->num_keys ++;
            right->key[num_shifts - 1] = parent->key[anchor_slot];
            right->ptr[0] = left->ptr[new_left_key_num + 1];
        }
        parent->key[anchor_slot] = new_anchor;
        int L = is_leaf?left->num_keys-1:left->num_keys;
        int R = !is_leaf?num_shifts-1:num_shifts;

        for(; R>0; L--, R--){
            left->num_keys--;
            right->num_keys++;
            right->key[R-1] = left->key[L];
            right->ptr[R-1 + (!left->is_leaf)] = left->ptr[L + (!left->is_leaf)];
        }
    }
    return NULL;
}
Node* mergeNode(Node *left, Node *right, Node *parent) {

   if(!left->is_leaf){
       int boundary_key = right->key[0];
       int idx_in_parent = findSlotInNode(parent, boundary_key);
       left->key[left->num_keys] = parent->key[idx_in_parent-1];
       left->ptr[left->num_keys+1] = getFirstNode(right);
       left->num_keys++;

   } else {
       left->ptr[MAX] = getNextNode(right);
   }

   for(int i=left->num_keys, j=0; j<right->num_keys; i++, j++){
       left->key[i] = right->key[j];
       left->ptr[i+(!left->is_leaf)] = right->ptr[j+(!left->is_leaf)];
       left->num_keys++;
   }
   return right;
}

void BPTree::collapseRoot(Node *prev, Node *cur) {
    root = cur;
    height--;
}