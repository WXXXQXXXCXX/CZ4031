#include "bptree.h"

void BPTree::del(int key) {
    Node *newNode = deleteAndBalance(key,root,NULL,NULL,NULL,NULL,NULL);
    if(newNode!=NULL){
        collapseRoot(root, newNode);
    }
}

Node* BPTree::deleteAndBalance(int key, Node *cur, Node *left, Node *right, Node *l_parent, Node *r_parent, Node * cur_parent){
    // node to do delete
    Node* newCur;
    Node* l;
    Node* r;
    Node* lp;
    Node* rp;
    Node* p;
    bool r_min;
    bool l_min;
    bool no_l;
    bool no_r;
    int pos;
    bool may_need_balance = false;
    if(nodeHasMinKey(cur)){
        may_need_balance = true;
    }

    int idx = findSlotInNode(cur, key);
    Node *newNode = reinterpret_cast<Node *>(cur->ptr[idx]);

    // if not the leaf, call delete and balance on child
    if(!cur->is_leaf){

        //if target child is first one, immediate left should be the last child in current left
        if(idx == 0){
            l = left == NULL?NULL: getLastNode(left);
            lp = l_parent;
        } else {
            l = reinterpret_cast<Node *>(cur->ptr[idx-1]);
            lp = cur;
        }

        //similar for the last child
        if(idx == cur->num_keys){
            r = right == NULL? NULL: getFirstNode(left);
            rp = r_parent;
        } else {
            r = reinterpret_cast<Node *>(cur->ptr[idx+1]);
            rp = cur;
        }
        newCur = deleteAndBalance(key, newNode, l, r, lp, rp, cur);
    } else if(idx<cur->num_keys && cur->key[idx]==key){
        //is leaf and found target
        newCur = newNode;
    } else {
        newCur = NULL;
        may_need_balance = false;
    }

    if(newCur !=  NULL) {
        // merge return the right node, if merge with left,
        if(newCur == newNode){
            removeKey(cur, idx);
        } else {
            removeKey(cur, idx+1);
        }
    }

    if(may_need_balance){
        no_l = left == NULL;
        no_r = right == NULL;
        l_min = nodeHasMinKey(l);
        r_min = nodeHasMinKey(r);

        //1. if node is root:
        if(no_l && no_r){
            if(cur->is_leaf){
                newNode = NULL;
            } else {
                newNode = getFirstNode(cur);
            }
        }
        //2. when borrow from both sides are not possible, then must merge
        else if((no_l||l_min) && (no_l||r_min)){
            if(l_parent != cur_parent){
                newNode = mergeNode(cur, right, r_parent);
            } else {
                newNode = mergeNode(left, cur, l_parent);
            }
        }

        //3. can merge with left or borrow from right
        else if(!no_l && l_min && !no_r && !r_min) {
            if(r_parent != cur_parent){
                newNode =mergeNode(left, cur, l_parent);
            } else {
                newNode =mergeNode(cur, right, r_parent);
            }
        }
        //4. can merge with right or borrow from left
        else if(!no_l && !l_min && !no_r && r_min) {
            if(l_parent != cur_parent) {
                newNode =mergeNode(cur, right, r_parent);
            } else {
                newNode =mergeNode(left, cur, l_parent);
            }
        }
        //5. can merge with both left and right
        else if(l_parent == r_parent){
            if(l->num_keys<=r->num_keys){
                newNode =shift(cur, r, r_parent);
            } else {
                newNode =shift(l, cur, l_parent);
            }
        }
    }
    return newNode;
}

void BPTree::removeKey(Node *node, int idx){
    node -> key[idx] = NULL;
    for(int i=idx; i<node->num_keys; i++){
        node -> key[idx] = node -> key[idx+1];
        node -> ptr[idx] = node -> ptr[idx+1];
        node -> num_keys --;
    }
}

Node *BPTree::shift(Node *left, Node *right, Node *parent) {
    //shift from right to left
    int num_shifts;
    int new_anchor;
    int anchor_slot;
    bool is_leaf = left->is_leaf;
    if(left->num_keys<=right->num_keys){
        num_shifts = (right->num_keys - left->num_keys) >> 1;
        int new_left_key_num = left->num_keys + num_shifts;
        new_anchor = right->key[num_shifts+1];
        if(is_leaf){
            new_anchor = right->key[num_shifts+1];
        } else {
            new_anchor = right->key[num_shifts];
        }
        anchor_slot = findSlotInNode(parent, new_anchor);
        if(!is_leaf){
            left->num_keys ++;
            right->num_keys --;
            left->key[left->num_keys] = new_anchor;
            left->ptr[left->num_keys] = getFirstNode(right);
            right -> ptr[0] = right->ptr[num_shifts];
        }
        parent->key[anchor_slot] = new_anchor;
        int num_to_left = num_shifts;
        if(!is_leaf){
            num_shifts --;
        }
        for(int i=num_to_left; num_shifts>0; num_shifts--, i--) {
            left->num_keys++;
            right->num_keys--;
            left->key[i] = right->key[num_shifts];
            left->ptr[i] = right->ptr[num_shifts];
        }

        for(int i=1; i<=right->num_keys; i++){
            right->key[i] = right->key[i+num_shifts];
            right->ptr[i] = right->ptr[i+num_shifts];
        }
    } else {
        // shift from left to right
        num_shifts = (left->num_keys - right->num_keys) >> 1;
        int new_left_key_num = left->num_keys - num_shifts + 1;
        for(int i=right->num_keys; i>0; i--){
            right->key[i+num_shifts] = right->key[i];
            right->ptr[i+num_shifts] = right->ptr[i];
        }
        new_anchor = parent->key[new_left_key_num];
        anchor_slot = findSlotInNode(parent, new_anchor) + 1;
        if(!is_leaf) {
            left->num_keys -= 1;
            right->num_keys += 1;
            right->key[num_shifts] = parent->key[anchor_slot];
            right->ptr[num_shifts] = getFirstNode(right);
        }
        int L = is_leaf?left->num_keys:left->num_keys+1;
        int R = is_leaf?num_shifts:num_shifts+1;

        for(; R>0; L--, R--){
            left->num_keys--;
            right->num_keys++;
            right->key[R] = left->key[L];
            right->ptr[R] = left->ptr[L];
        }
    }
    return NULL;
}
Node* mergeNode(Node *left, Node *right, Node *parent) {
   if(!left->is_leaf){
       int boundary_key = right->key[0];
       int idx_in_parent = findSlotInNode(parent, boundary_key);
       left->key[left->num_keys] = parent->key[idx_in_parent];
       left->ptr[left->num_keys] = getFirstNode(right);
       left->num_keys++;

   } else {
       left->ptr[MAX] = getNextNode(right);
   }

   for(int i=left->num_keys, j=0; j<right->num_keys; i++, j++){
       left->key[i] = right->key[j];
       left->ptr[i] = right->ptr[j];
       left->num_keys++;
   }
   return right;
}

void BPTree::collapseRoot(Node *prev, Node *cur) {
    root = cur;
    height--;
}