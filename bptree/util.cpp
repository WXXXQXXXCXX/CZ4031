#include "bptree.h"
#include <iostream>
#include <queue>
#include <list>
#include <cmath>

using namespace std;

void printInnerNode(Node *node){
    for(int i=0; i<MAX; i++){
        int n;
        if(node->ptr[i] && i<=node->num_keys){
            n = reinterpret_cast<Node*>(node->ptr[i])->num_keys;
        } else {
            n=-1;
        }
        int k=-1;
        if(node->num_keys>i){
            cout<< "[" <<n<< "]"<<node->key[i];
        } else {
            cout<< "[" <<n<< "]"<<"-";
        }
    }
    int n=-1;
    if(node->ptr[MAX] && node->num_keys == MAX){
        n = reinterpret_cast<Node*>(node->ptr[MAX])->num_keys;
    }
    cout<< "[" << n << "]";

}
void printLeafNode(Node *node){
    for(int i=0; i<MAX; i++){
        int s=-1;
        if(node->ptr[i] && i<=node->num_keys){
            list<int> *n = reinterpret_cast<list<int>*>(node->ptr[i]);
            s = n->size();
        }
        int k=-1;
        if(node->num_keys>i){
            cout<< "[n=" <<s<< "]"<<node->key[i];
        } else {
            cout<< "[n=" <<s<< "]"<<"-";
        }

    }
    int s=-1;
    if(node->ptr[MAX]){
        Node* n = reinterpret_cast<Node *>(node->ptr[MAX]);
        s = n->key[0];
    }
    cout<< "[next->" << s << "]";
}

void BPTree::printTree() {
    queue<Node> q;
    q.push(*root);
    int level = 0;
    int num_nodes = 0;
    while(level<=height){
        num_nodes += q.size();
        queue<Node> newQ;
        cout<<"level"<<level<<"\t\n";
        int i=0;
        while (!q.empty()){
            i++;
            if(i%MAX==1) cout<<"      \t";
            Node n = q.front();
            q.pop();
            if(level==height){
                printLeafNode(&n);
            } else{
                printInnerNode(&n);
            }
            for(int i=0; i<=n.num_keys; i++){
                if(n.ptr[i]!= NULL){
                    newQ.push(*reinterpret_cast<Node*>(n.ptr[i]));
                }
            }
            if (i%MAX==0) cout<<"\n";
            else cout<<"\t";
        }
        cout<<"\n";
        level++;
        q = newQ;
    }
    cout<<"number of nodes: "<<num_nodes<<"\n";
    cout<<"height: "<<height<<"\n";

}

bool nodeHasMinKey(Node *node){
    if(node==NULL){
        return false;
    }
    if(node->is_leaf){
        return node->num_keys <= floor((MAX+1)/2.0);
    } else {
        return node->num_keys <= floor((MAX)/2.0);
    }
}


Node * getLastNode(Node *node){
    return reinterpret_cast<Node *>(node->ptr[node->num_keys]);
}

Node * getFirstNode(Node *node){
    return reinterpret_cast<Node *>(node->ptr[0]);
}

Node * getNextNode(Node *node){
    return reinterpret_cast<Node *>(node->ptr[MAX]);
}

int BPTree::getCountNode() {
    return count_node;
}

int BPTree::getHeight() {
    return height;
}