#include "pool_allocator.h"

#include "pool.h" // Pool, PoolList

#include <mutex> // lock_guard
namespace c11 {
    using namespace std;
}

namespace ldl {

    //--------------
    bool PoolAllocator<void>::HasPool(size_t block_size)
    {
        c11::lock_guard<c11::mutex> lock(mutex_);
        return pool_list_.HasPool(block_size);
    }

    //--------------
    Pool& PoolAllocator<void>::GetPool(size_t block_size)
    {
        c11::lock_guard<c11::mutex> lock(mutex_);
        return pool_list_.GetPool(block_size);
    }

    //--------------
    void PoolAllocator<void>::IncreasePoolSize(size_t block_size, size_t num_blocks)
    {
        c11::lock_guard<c11::mutex> lock(mutex_);
        pool_list_.IncreasePoolSize(block_size,num_blocks);
    }

    //--------------
    void PoolAllocator<void>::SetPoolGrowthStep(size_t block_size, int growth_step)
    {
        c11::lock_guard<c11::mutex> lock(mutex_);
        pool_list_.SetPoolGrowthStep(block_size,growth_step);
    }

    //--------------
    int PoolAllocator<void>::GetPoolGrowthStep(size_t block_size)
    {
        c11::lock_guard<c11::mutex> lock(mutex_);
        return pool_list_.GetPoolGrowthStep(block_size);
    }

    //--------------
    size_t PoolAllocator<void>::GetPoolFree(size_t block_size)
    {
        c11::lock_guard<c11::mutex> lock(mutex_);
        return pool_list_.GetPoolFree(block_size);
    }

    //--------------
    size_t PoolAllocator<void>::GetPoolSize(size_t block_size)
    {
        c11::lock_guard<c11::mutex> lock(mutex_);
        return pool_list_.GetPoolSize(block_size);
    }

    //--------------
    bool PoolAllocator<void>::PoolIsEmpty(size_t block_size)
    {
        c11::lock_guard<c11::mutex> lock(mutex_);
        return pool_list_.PoolIsEmpty(block_size);
    }

    //--------------
    size_t PoolAllocator<void>::GetMaxPoolBlockSize()
    {
        c11::lock_guard<c11::mutex> lock(mutex_);
        return pool_list_.GetMaxPoolBlockSize();
    }

    //--------------
    void* PoolAllocator<void>::Pop(size_t block_size)
    {
        c11::lock_guard<c11::mutex> lock(mutex_);
        return pool_list_.Pop(block_size);
    }

    //--------------
    void PoolAllocator<void>::Push(size_t block_size, void* ptr)
    {
        c11::lock_guard<c11::mutex> lock(mutex_);
        pool_list_.Push(block_size,ptr);
    }

    //--------------
    c11::mutex PoolAllocator<void>::mutex_;

    //--------------
    PoolList PoolAllocator<void>::pool_list_;

} //namespace ldl
