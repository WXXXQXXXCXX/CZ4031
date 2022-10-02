#include "bptree.h"
#include <vector>
#include <algorithm>
#include <random>
#include <sstream>
#include "iostream"
#include "fstream"

using namespace std;

BPTree * test_insert(){
    BPTree tree = BPTree();
    vector<int> myvector;

    for (int i=1; i<=10; ++i) myvector.push_back(i);
    auto rng = default_random_engine {};
    shuffle(begin(myvector), end(myvector), rng);
    int v[] = {2,7,1,4,3,6,5,10,9,8};
    //for(int i=0; i<10; i++) cout<<myvector.at(i)<<" ";
    cout<<"\n";
    for(int i=0; i<10; i++){
        tree.insert(RecPtr{.pos=1}, v[i]);
    }
    tree.printTree();
    return &tree;
    //tree.del(9);

}

vector<int> test_read_file(int n) {
    ifstream f("../data.tsv");
    string line;
    int i=0;
    getline(f, line);
    vector<int> vals;
    while(getline(f, line) && i<n){
        i++;
        stringstream iss(line);
        string tconst; string a; string b;
        getline(iss, tconst, '\t');
        getline(iss, a, '\t');
        getline(iss, b, '\t');
        vals.push_back(atoi(b.c_str()));
    }
    return vals;
}

void read_and_insert_all(BPTree *tree){

    ifstream f("../data.tsv");
    string line;
    getline(f, line);
    int i=0;
    while(getline(f, line)){
        stringstream iss(line);
        string tconst; string a; string b;
        getline(iss, tconst, '\t');
        getline(iss, a, '\t');
        getline(iss, b, '\t');
        int votes = atoi(b.c_str());
        if(votes<=4000 && votes>=3000){
            i++;
        }
        tree->insert(RecPtr{.pos=1}, votes);
    }
    //tree.printTree();
    cout<<i<<"\n";
}
void test_read_and_insert(int n){
    BPTree tree = BPTree();
    vector<int> ans =test_read_file(n);
    for(int i=0; i<n; i++){
        if(i==15){
            int i;
        }
        tree.insert(RecPtr{.pos=1}, ans[i]);
    }
    tree.printTree();
}
void test_merge() {
    Node *parent=new Node(false);
    Node *c1 = new Node(false);
    Node *c2 = new Node(false);

    parent->num_keys = 2;
    c1->num_keys = 2;
    c2->num_keys = 1;
    parent->ptr[0] = c1;
    parent->ptr[1] = c2;
    parent->key[0] = 10;
    parent->key[1] = 20;
    c1->key[0] = 3;
    c1->key[1] = 7;
    c2->key[0] = 17;
    c1->ptr[MAX] = c2;
    mergeNode(c1, c2, parent);

}

void test_find(){
    BPTree tree = BPTree();
    int v[] = {2,7,1,4,3,6,5,10,9,8};
    for(int i=0; i<10; i++){
        RecPtr *p = new RecPtr;
        p->pos=1;
        tree.insert(*p, v[i]);
    }
    tree.printTree();
    tree.find(4);
    tree.findRange(1,5);
}

void test_delete(){
    BPTree tree = BPTree();
    int v[] = {1,2,4,5,3,7,8,6};
    for(int i=0; i<8; i++){
        RecPtr *p = new RecPtr;
        p->pos=1;
        tree.insert(*p, v[i]);
    }
    //tree.printTree();
    //tree.del(6);
    tree.del(3);
    tree.del(8);
    tree.printTree();
}

//int main(){
//    test_insert();
//    test_delete();
//    test_read_and_insert(4000000);
//    BPTree tree = BPTree();
//    read_and_insert_all(&tree);
//    list<int> ans = tree.findRange(3000, 4000);
//    cout<<"\nnode count: "<<tree.getCountNode()<<"\n";
//    cout<<"height: "<<tree.getHeight()<<"\n";
//    cout<<"number of records: "<<ans.size()<<"\n";
//    tree.del(1000);
//    cout<<"\nnode count: "<<tree.getCountNode()<<"\n";
//    return 0;
//}