#include "../bptree/bptree.h"
#include "../storage/blks.h"
#include <sstream>
#include "iostream"
#include "fstream"

void range(int low, int high, BPTree *tree, Memory *mem){
    cout<<"========index node content========\n";
    list<int> ans = tree->findRange(low, high);
    cout<<"range search between "<<low<<" and "<<high<<" inclusive\n";
    cout<<"number of nodes accessed: "<<tree->getCountNode()<<"\n";
    cout<<"number of records found: "<<ans.size()<<"\n";
    cout<<"==========Record values==========\n";
    double avg_rating = 0;
    mem->start_access_count();
    for (int pos:ans){
        Record rec = Record();
        mem->rec_read(new RecPtr{.pos=pos}, &rec);
        avg_rating += rec.avg_rating;
        cout<<rec.tconst<<", "<<rec.avg_rating<<", "<<rec.num_votes<<"\n";
    }
    cout<<"==========Block access info==========\n";
    int cnt = mem->end_access_count();
    cout<<"total blocks accessed: "<< cnt<<"\n";
    cout<<"mean of averageRating: "<<avg_rating/ans.size()<<"\n";
}

void find(int num, BPTree *tree, Memory *mem){
    cout << "==========EXPERIMENT 3==========" << endl;
    cout << "Retrieve the record with numVotes = " << num << endl;
    list<int> ans = tree->find(num);
    cout << endl;

    cout << "Number of nodes accessed: " << tree->getCountNode() << endl;
    cout << "Number of records found: " << ans.size() << endl;

    cout << "==========Record Values==========" << endl;
    mem->start_access_count();
    double averageRatings = 0.0;

    for (int idx: ans) {
        Record rec = Record();
        mem->rec_read(new RecPtr{.pos=idx}, &rec);
        averageRatings += rec.avg_rating;
        cout << rec.tconst << ", " << rec.avg_rating << ", " << rec.num_votes << endl;
    }

    cout << "==========Block Access Information==========" << endl;
    int accessCount = mem->end_access_count();
    cout<<"Number of blocks accessed: "<< accessCount << endl;

    cout << "==========Average Rating of Records==========" << endl;
    averageRatings /= ans.size();
    cout<<"Average Rating: "<< averageRatings << endl;
}

void del(int num, BPTree *tree, Memory *mem){
    list<int> ans = tree->del(num);
    for (int pos:ans){
        Record rec = Record();
        mem->rec_delete(new RecPtr{.pos=pos});
        cout<<rec.tconst<<", "<<rec.avg_rating<<", "<<rec.num_votes<<"\n";
    }
    cout<<"number of records found: "<<ans.size()<<"\n";
    cout<<"=========B+Tree parameters=========\n";
    tree->getTreeInfo();
}

void insert(BPTree *tree, Memory *mem){
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
        Record rec = {
                .tconst="",
                .avg_rating = static_cast<float>(atof(a.c_str())),
                .num_votes = static_cast<uint>(atoi(b.c_str()))
        };
        strcpy(rec.tconst, tconst.c_str());
        RecPtr ptr;
        mem->rec_insert(&ptr, rec);
        //cout<<"RecPtr, i="<<i<<", pos="<<ptr.pos<<"\n";
        tree->insert(ptr, atoi(b.c_str()));
        i++;
    }
    cout<<"=========B+Tree parameters=========\n";
    cout<<"n = "<<MAX<<"\n";
    tree->getTreeInfo();
    mem->get_mem_used();
    cout<<"\n";


}

int main(int argc,char* argv[]){
    int blk_size = atoi(argv[1]);
    Memory mem = Memory(100000000, blk_size);
    BPTree tree = BPTree();
    insert(&tree, &mem);
    //del(1000, &tree, &mem);
    //range(30000,40000,&tree,&mem);
    find(500, &tree, &mem);
}