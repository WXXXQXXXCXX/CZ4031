#ifndef BPTREE_H
#define BPTREE_H

#include "blks.h"
#include <list>

const int MAX = 4;


struct Node {
    Node(bool isLeaf): is_leaf(isLeaf){}
    int num_keys;
    int key[MAX];
    bool is_leaf;
    void* ptr[MAX+1];
};

struct InsertResult {
    Node * newNode;
    int move_up;
};


class BPTree {
    Node* root;
    int height;
    void removeKey(Node *node, int idx);
    Node* deleteAndBalance(int key, Node *cur, Node *left, Node *right, Node *l_parent, Node *r_parent, Node * cur_parent);
    Node *shift(Node *left, Node *right, Node *parent);
    void collapseRoot(Node *prev, Node *cur);
    void redistributeKey(Node *cur, Node *newNode, int key, Node * newPtr, int pos);

public:
    BPTree();
    void insert(RecPtr, int);
    list<int> find(int);
    list<int> findRange(int, int);

    void del(int);
    Node* getRoot();
    void printKeys();
    void printTree();

};

int findSlotInNode(Node *node, int key);
void insertToLeafNoSplit(Node *cur, int key, int pos, RecPtr ptr);
void insertToInnerNoSplit(Node* cur, int key, int depth, RecPtr rec_ptr);
void insertToInnerNoSplit(Node* cur, int key, int pos, Node* newNode);
bool insertInLeaf(Node *cur, int key,InsertResult* res, RecPtr rec_ptr);
bool insertInInner(Node *cur, int key,InsertResult* res, int depth, RecPtr ptr);
Node* deleteAndBalance(int key, Node *cur, Node *left, Node *right, Node *l_parent, Node *r_parent, Node * cur_parent);
bool nodeHasMinKey(Node *node);
Node * getFirstNode(Node *node);
Node * getLastNode(Node *node);
Node * getNextNode(Node *node);
Node* mergeNode(Node *left, Node *right, Node *parent);
int findChildForKey(Node *node, int key);
void redistribute(Node *cur, Node *newNode, int insert_pos, int key, void *ptr, InsertResult *res);
#endif