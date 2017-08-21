#pragma once
#ifndef LDL_POOLED_NEW_H_
#define LDL_POOLED_NEW_H_

#include "Pool.h" // Pool

namespace ldl {

    // base class that will allocate objects of a class from a static memory pool.
    // does not allow arrays of objects to be allocated from a pool.
    template<typename T>
    class PooledNew {
    public:

        // Resize pool to hold num_blocks buffers
        static void SetPoolSize(std::size_t num_blocks);

        // increase pool size by num_blocks buffers
        static void IncreasePoolSize(std::size_t num_blocks);

        /// Set number of elements to add to Pool if it becomes empty.
        // setting growth_step > 0 automatically increases the pool size by  +growth_step elements when it is empty
        // setting growth_step < 0 automatically increases the pool size by current_capacity/(-growth_step) when it is empty.
        // setting growth_step = 0 disables automatic growth of the pool.
        static void SetPoolGrowthStep(int growth_step);

        /// Return the current value of growth_step.
        static int GetPoolGrowthStep();

        static std::size_t GetPoolBlockSize();

        // return number of unallocated blocks in pool.
        static std::size_t GetPoolFree();

        // return total number (allocated & unallocated) of blocks in pool.
        static std::size_t GetPoolSize();

        //get a pointer to a block containing of the specified size
        // this function is called to allocate memory for an object.
        static void* operator new(std::size_t);

        // return a block of the specified size back to its pool.
        // his function is called to deallocate memory for an object.
        static void operator delete(void* ptr);

    private:

        static Pool pool_;

    }; //class PooledNew

} //namespace ldl

#include "pooled_new.hpp"

#endif //! LDL_POOLED_NEW_H_