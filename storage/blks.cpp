#include "blks.h"
#include <iostream>

using namespace std;

Memory::Memory(uint mem_size, uint blk_size) {
    this->blk_size = blk_size;
    this->mem_size = mem_size;
    this->num_blks_used = 0;
    this->blk_avail = mem_size / blk_size;
    this->cur_ptr = 0;
    this->counting = false;
    this->num_rec = 0;
}

bool Memory::new_blk(){
    if (blk_avail <= 0){
        cout << "memory is full";
        return false;
    }

    blk_avail --;
    num_blks_used ++;
    int new_blk = mem.size();
    mem.insert(mem.end(), blk_size, 0x00);
    blocks[new_blk] = 0;
    cur_ptr = new_blk;
    return true;

}

uint Memory::get_num_blks(){
    return num_blks_used;
}

uint Memory::get_mem_used(){
    map<long, int>::iterator itr;
    for(itr = blocks.begin(); itr != blocks.end(); itr ++){
        cout<< itr->second << "\n";
    }
    cout<<"total occupied bytes: "<<num_rec*REC_LENGTH<<"\n";
    cout<<"memory occupied by all blocks: "<<blk_size*num_blks_used;
    return blk_size*num_blks_used;
}

bool Memory::rec_insert(RecPtr* rec, Record data){

    if (garbage.size()>0){
        RecPtr * free_space = garbage.back();
        long pos = free_space->rec;
        memcpy(&mem.at(pos), &data, REC_LENGTH);
        memcpy(&mem.at(pos), &data.tconst, REC_LENGTH);

        memcpy(&mem.at(pos+RATING_OFFS), &data.avg_rating, sizeof(data.avg_rating));
        memcpy(&mem.at(pos+VOTE_OFFS), &data.num_votes, sizeof(data.avg_rating));
        garbage.pop_back();
        blocks[free_space->blk] += REC_LENGTH;
        rec->blk = free_space->blk;
        rec->rec =free_space->rec;
        //rec = free_space;
        return true;
    }

    if (cur_ptr==0 && mem.empty()) {
        if(!new_blk()){
            return false;
        }
    }

    if(blocks[cur_ptr]+REC_LENGTH>blk_size){
        if(!new_blk()){
            return false;
        }
    }
    

    uint in_blk_offs = blocks[cur_ptr];
    memcpy(&mem.at(cur_ptr+in_blk_offs), &data.tconst, REC_LENGTH);

    memcpy(&mem.at(cur_ptr+in_blk_offs+RATING_OFFS), &data.avg_rating, sizeof(data.avg_rating));
    memcpy(&mem.at(cur_ptr+in_blk_offs+VOTE_OFFS), &data.num_votes, sizeof(data.avg_rating));
    blocks[cur_ptr] += REC_LENGTH;
    num_rec ++;
    rec->blk=cur_ptr;
    rec->rec=cur_ptr + in_blk_offs;
    return true;
}

bool Memory::rec_delete(RecPtr* rec){
    garbage.push_back(rec);
    blocks[rec->blk] -= REC_LENGTH;
    num_rec --;
    return true;
}

bool Memory::rec_read(RecPtr *rec, Record *data) {
    for(int i=0; i<TCONST_LEN-1; i++){
        data->tconst[i] = mem.at(rec->rec+i);
    }
    data->tconst[TCONST_LEN-1]='\0';
    float rating;
    byte b_rating[] = {mem.at(rec->rec+RATING_OFFS),
                       mem.at(rec->rec+RATING_OFFS+1),
                       mem.at(rec->rec+RATING_OFFS+2),
                       mem.at(rec->rec+RATING_OFFS+3) };
    int votes;
    byte b_votes[] = {
        mem.at(rec->rec+VOTE_OFFS),
        mem.at(rec->rec+VOTE_OFFS+1),
        mem.at(rec->rec+VOTE_OFFS+2),
        mem.at(rec->rec+VOTE_OFFS+3),
    };
    memcpy(&rating, &b_rating, sizeof(rating));
    memcpy(&votes, &b_votes, sizeof(votes));
    data->num_votes = votes;
    data->avg_rating = rating;

    if(counting){

    }
    return true;
}

void Memory::start_access_count() {
    counting = true;
}

int Memory::end_access_count() {
    counting = false;
    int vis_cnt = 0;
    for(map<long, bool>::iterator i=vis.begin(); i!=vis.end(); i++){
        vis_cnt++;
    }
    vis.clear();
    return vis_cnt;
}

Memory::~Memory() {

}

