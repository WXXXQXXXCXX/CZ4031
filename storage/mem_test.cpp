#include "blks.h"
#include <iostream>
#include <vector>

using namespace std;

void test_alloc(){
    Memory mem = Memory(1000,50);
    vector<RecPtr> ptrs;
    for(int i=0; i<10; i++){
        RecPtr ptr;
        Record rec = { .tconst="tt0000000"};
        rec.avg_rating = 1.1+0.1*i;
        rec.num_votes = 100+i;
        mem.rec_insert(&ptr, rec);
        ptrs.push_back(ptr);
    }

    mem.rec_delete(&ptrs.at(2));
    for(int i=10; i<12; i++){
        RecPtr ptr;
        Record rec = { .tconst="tt0000000"};
        rec.avg_rating = 1.1+0.1*i;
        rec.num_votes = 100+i;
        mem.rec_insert(&ptr, rec);
        ptrs.push_back(ptr);
    }
    for(int i=0; i<ptrs.size(); i++){
        Record rec;
        mem.rec_read(&ptrs.at(i), &rec);
        cout<<rec.tconst<<","<<rec.num_votes<<","<<rec.avg_rating<<"\n";
    }

     mem.get_mem_used();
     mem.get_num_blks();

}

int main(){
    test_alloc();
    return 0;
}