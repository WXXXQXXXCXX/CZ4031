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
    blocks[new_blk/blk_size] = 0;
    cur_ptr = new_blk/blk_size;
    return true;

}

uint Memory::get_num_blks(){
    return num_blks_used;
}

uint Memory::get_mem_used(){
    map<long, int>::iterator itr;

    cout<<"=========memory use in total========="<<"\n";
    cout<<"number of blocks: "<<num_blks_used<<"\n";
    cout<<"total occupied bytes: "<<num_rec*REC_LENGTH<<"\n";
    cout<<"memory occupied by all blocks: "<<blk_size*num_blks_used<<"bytes = "<<blk_size*num_blks_used*1.0e-6<<"MB\n";
    cout<<"number of records inserted: "<<num_rec;
    return blk_size*num_blks_used;
}

bool Memory::rec_insert(RecPtr* rec, Record data){

    if (garbage.size()>0){
        RecPtr * free_space = garbage.back();
        int pos = free_space->pos;

        int blk = pos / blk_size;
        int offs = pos % blk_size;

        memcpy(&mem.at(pos), &data, REC_LENGTH);
        memcpy(&mem.at(pos), &data.tconst, REC_LENGTH);

        memcpy(&mem.at(pos+RATING_OFFS), &data.avg_rating, sizeof(data.avg_rating));
        memcpy(&mem.at(pos+VOTE_OFFS), &data.num_votes, sizeof(data.avg_rating));
        garbage.pop_back();
        blocks[blk] += REC_LENGTH;
        rec->pos = pos;
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
    memcpy(&mem.at(cur_ptr*blk_size+in_blk_offs), &data.tconst, REC_LENGTH);

    memcpy(&mem.at(cur_ptr*blk_size+in_blk_offs+RATING_OFFS), &data.avg_rating, sizeof(data.avg_rating));
    memcpy(&mem.at(cur_ptr*blk_size+in_blk_offs+VOTE_OFFS), &data.num_votes, sizeof(data.avg_rating));
    blocks[cur_ptr] += REC_LENGTH;
    num_rec ++;
    rec->pos = cur_ptr * blk_size + in_blk_offs;
    return true;
}

bool Memory::rec_delete(RecPtr* rec){
    garbage.push_back(rec);
    int blk = rec->pos / blk_size;
    int offs = rec->pos%blk_size;
    blocks[blk] -= REC_LENGTH;
    num_rec --;
    return true;
}

bool Memory::rec_read(RecPtr *rec, Record *data) {
    for(int i=0; i<TCONST_LEN-1; i++){
        data->tconst[i] = mem.at(rec->pos+i);
    }
    data->tconst[TCONST_LEN-1]='\0';
    float rating;
    byte b_rating[] = {mem.at(rec->pos+RATING_OFFS),
                       mem.at(rec->pos+RATING_OFFS+1),
                       mem.at(rec->pos+RATING_OFFS+2),
                       mem.at(rec->pos+RATING_OFFS+3) };
    int votes;
    byte b_votes[] = {
        mem.at(rec->pos+VOTE_OFFS),
        mem.at(rec->pos+VOTE_OFFS+1),
        mem.at(rec->pos+VOTE_OFFS+2),
        mem.at(rec->pos+VOTE_OFFS+3),
    };
    memcpy(&rating, &b_rating, sizeof(rating));
    memcpy(&votes, &b_votes, sizeof(votes));
    data->num_votes = votes;
    data->avg_rating = rating;

    if(counting){
        int blk = rec->pos/blk_size;
        if(vis.count(blk)==0){
            vis.insert(blk);
        }
    }
    return true;
}

void Memory::start_access_count() {
    counting = true;
}

int Memory::end_access_count() {
    counting = false;
    int vis_cnt = vis.size();
    for(int pos: vis){
        int blk_start = pos*blk_size;
        cout<<"Block content"<<"("<<pos<<")\n";
        for(int i=0; i+REC_LENGTH<blk_size; i+=REC_LENGTH){
            Record rec;
            rec_read(new RecPtr{.pos=i+blk_start},&rec);
            if(rec.avg_rating==0 && rec.num_votes==0){
                cout<<"Record offset: "<<i<<": empty"<<"\n";
            }else {
                cout<<rec.tconst<<" ";
            }
            if((i/REC_LENGTH+1)%3 == 0) cout<<"\n";
        }
        cout<<"\n";
    }
    vis.clear();
    return vis_cnt;
}

Memory::~Memory() {

}

