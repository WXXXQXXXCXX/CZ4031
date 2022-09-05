#ifndef MEM_H
#define MEM_H

#include <vector>
#include <map>

typedef unsigned int uint;
typedef void * blk_ptr;
typedef byte rec[];
typedef byte blk[];
using namespace std;


class Memory {
private:
    /**
     * size of the memory
    */
    uint mem_size;
    /**
     * block size
    */
    uint blk_size;
    /**
     * currently allocated blocks and the bytes occupied
    */
    map<blk*, int> blocks;
    /**
     * record whether a block is visited during an insert/delete/search
    */
    map<blk*, bool> vis;
    /**
     * number of blocks occupied
    */
    uint num_blks_used;
    /**
     * pointer to the free position
    */
    byte *cur_ptr;
    /**
     * allocate a new free block
     */
    bool new_blk();

public:
    /**
     * @brief constructor
     * 
     * @param mem_size size of memory
     * @param blk_size block size
     */
    Memory(uint mem_size, uint blk_size);

    ~ Memory();

    /**
     * @brief Get the number of blocks occupied
     * 
     * @return uint 
     */
    uint get_num_blks();

    /**
     * @brief Get size of memory occupied (including the wasted space)
     * 
     * @return uint 
     */
    uint get_mem_used();

    /**
     * @brief insert record. Use non-clustered index, the records are inserted sequentially.
     * Check if current allocated block has enough space. If not, allocate a new block and. 
     * Then insert to the first available block
     * 
     * @param data fields and values of data
     * @param rec (out) pointer to the inserted record
     * @return true if the insert is successful
     */
    bool rec_insert(const rec* rec, string data);

    /**
     * @brief delete a record
     * 
     * @param rec record pointer
     * @return true if record pointer is valid and record is deleted
     */
    bool rec_delete(rec* rec);

    /**
     * @brief Find the byte array for the given record and read the field using the offset.
     * 
     * @param rec record pointer
     * @param data (out) return data
     * @param field the field to read, enum
     * @return true if record pointer exist and data is read successfully
     */
    bool rec_read(const rec* rec, int* data, int field);

    /**
     * @brief Get the bytes occupied and bytes wasted for all blocks
     * 
     */
    void get_blk_stats();

    /**
     * @brief when begin insert/delete/search, call this to mark the blocks accessed.
     * 
     */
    void start_access_count();

    /**
     * @brief used together with start_access_count(), which marks the end of an operation
     * this will also reset the count
     * 
     * @return int number of blocks accessed
     */
    int end_access_count();


};

#endif