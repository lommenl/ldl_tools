#pragma once
#ifndef LDL_POOLED_NEW_H_
#define LDL_POOLED_NEW_H_

namespace ldl {

    //-----------------------
    // Base class that will allocate buffers for an object from a Pool of memory.
    template<typename T>
    class PooledNew {
    public:

        // Get a pointer to a block containing n bytes.
        // This function is called by new to allocate memory for a single object.
        static void* operator new(size_t n);

        // Return a block containing a single element of type T to the pool
        // This function is called by delete to deallocate memory for a single object.
        static void operator delete(void* ptr);

        //----

        // Increase size of pool holding single objects by num_blocks.
        static void IncreasePoolSize(size_t num_blocks);

        /// Set number of elements to automatically add to single object pool if it becomes empty.
        // setting growth_step > 0 increases the pool size by  +growth_step elements when it is empty
        // setting growth_step < 0 increases the pool size by current_capacity/(-growth_step) when it is empty.
        // setting growth_step = 0 disables automatic growth of the pool.
        static void SetPoolGrowthStep(int growth_step);

        /// Return the current value of growth_step for the single object pool.
        static int GetPoolGrowthStep();

        // return number of unallocated blocks in the single object pool
        static size_t GetPoolFree();

        // return total number (allocated & unallocated) of blocks in the single object pool.
        static size_t GetPoolSize();

        // return true if the single object pool is empty
        static bool PoolIsEmpty();

        //----------------------

        // Get a pointer to a block containing n bytes. (plus 4 bytes to hold its block size)
        // This function is called to allocate memory for an array of objects.
        static void* operator new[](size_t n);

        // Return a block containing a T[numel] array that was allocated by new[]() to its pool.
        // This function is called by delete to deallocate memory for an array of objects.
        static void operator delete[](void* ptr);

        // Increase size of T[numel] pool by num_blocks.
        static void IncreaseArrayPoolSize(size_t numel, size_t num_blocks);

        /// Set number of elements to add T[numel] pool if it becomes empty.
        // setting growth_step > 0 automatically increases the pool size by  +growth_step elements when it is empty
        // setting growth_step < 0 automatically increases the pool size by current_capacity/(-growth_step) when it is empty.
        // setting growth_step = 0 disables automatic growth of the pool.
        static void SetArrayPoolGrowthStep(size_t numel, int growth_step);

        /// Return the current value of growth_step for T[numel] pool.
        static int GetArrayPoolGrowthStep(size_t numel);

        // return number of unallocated blocks in T[numel] pool
        static size_t GetArrayPoolFree(size_t numel);

        // return total number (allocated & unallocated) of blocks in T[numel] pool
        static size_t GetArrayPoolSize(size_t numel);

        // return true if T[numel] pool is empty.
        static bool GetArrayPoolIsEmpty(size_t numel);

    private:

        /// Size, in bytes, of a single element.
        static size_t element_size_;

    }; //class PooledNew

} //namespace ldl

#include "pooled_new.hpp"

#endif //! LDL_POOLED_NEW_H_