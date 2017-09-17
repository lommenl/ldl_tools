#pragma once
#ifndef LDL_POOL_H_
#define LDL_POOL_H_

#include <vector>

namespace ldl {

    // Class defining a stack of pointers to memory blocks allocated from the heap.
    class Pool {
    public:
        // default constructor
        Pool();

        // construct and initialize object that holds num_blocks block_size byte blocks.
        Pool(size_t block_size, size_t num_blocks = 0, int growth_step = 0);

        // destructor
        ~Pool();

        // swap state with other.
        void swap(Pool& other);

        // initialize a default constructed object
        void Initialize(size_t block_size, size_t num_blocks, int growth_step);

        // reset pool to a default state.
        void Reset();

        void IncreaseSize(size_t num_blocks);

        /// Set number of blocks to automatically add to stack_ if it becomes empty.
        // setting growth_step > 0 automatically increases the pool size by  +growth_step blocks when it is empty
        // setting growth_step < 0 automatically increases the pool size by current_capacity/(-growth_step) when it is empty.
        // setting growth_step = 0 disables automatic growth of the pool.
        void SetGrowthStep(int growth_step);

        /// Return the current value of growth_step.
        int GetGrowthStep() const;

        /// return number of bytes in a block
        size_t GetBlockSize() const;

        // return number of unallocated blocks in pool.
        size_t GetFree() const;

        // return total number (allocated+ and unallocated) blocks in pool.
        size_t GetSize() const;

        // return true if pool has no free elements
        bool IsEmpty() const;

        // Pop a pointer to a single block off of stack_.
        // If stack is empty and growth_step!=0, call increaseSize() to add more blocks according to the value of growth_step.
        // If stack is empty and growth_step==0, throw an exception.
        void* Pop();

        // Push a block pointer onto stack_.
        // If stack is full and growth_step != 0, increase the size of stack_ by 1.
        // If stack is full and growth_step==0, throw an exception.
        void Push(void* ptr);

    private:

        /// Increase stack_ size by num_blocks blocks.
        // no copies allowed
        Pool(const Pool&) = delete;
        Pool& operator=(const Pool&) = delete;

        //----

        // number of bytes in each block
        size_t block_size_;

        // Number of blocks to automatically add to stack_ if it becomes empty.
        int growth_step_;

        // top of stack
        size_t tos_;

        // Stack of pointers to allocated block_size_ byte memory blocks
        std::vector<void*> stack_;

    }; // class Pool

} //namespace ldl

#endif //! LDL_POOL_H_
