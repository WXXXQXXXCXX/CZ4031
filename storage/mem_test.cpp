#include "blks.h"
#include <iostream>
#include <vector>

using namespace std;

void test_alloc(){
    Memory mem = Memory(100000000,200);
    vector<RecPtr> ptrs;
    for(int i=0; i<30; i++){
        RecPtr ptr;
        Record rec = { .tconst="tt0000000"};
        rec.avg_rating = 1.1+0.1*i;
        rec.num_votes = 100+i;
        mem.rec_insert(&ptr, rec);
        ptrs.push_back(ptr);
    }

    mem.rec_delete(&ptrs.at(2));
    ptrs.erase(ptrs.begin() + 2);
    mem.rec_delete(&ptrs.at(5));
    ptrs.erase(ptrs.begin() + 5);
    mem.rec_delete(&ptrs.at(10));
    ptrs.erase(ptrs.begin() + 10);
    mem.rec_delete(&ptrs.at(20));
    ptrs.erase(ptrs.begin() + 20);
    for(int i=31; i<50; i++){
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
//
//int main(){
//    test_alloc();
//    return 0;
//}