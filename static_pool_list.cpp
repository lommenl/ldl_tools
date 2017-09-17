#include "static_pool_list.h"

#include <mutex> // lock_guard
namespace c11 {
    using namespace std;
}

namespace ldl {

    //--------------
    void StaticPoolList::Reset()
    {
        c11::lock_guard<c11::mutex> lock(mutex_);
        pool_list_.Reset();
    }

    //--------------
    bool StaticPoolList::HasPool(size_t block_size)
    {
        c11::lock_guard<c11::mutex> lock(mutex_);
        return pool_list_.HasPool(block_size);
    }

    //--------------
    Pool& StaticPoolList::GetPool(size_t block_size)
    {
        c11::lock_guard<c11::mutex> lock(mutex_);
        return pool_list_.GetPool(block_size);
    }

    //--------------
    void StaticPoolList::IncreasePoolSize(size_t block_size, size_t num_blocks)
    {
        c11::lock_guard<c11::mutex> lock(mutex_);
        pool_list_.IncreasePoolSize(block_size, num_blocks);
    }

    //--------------
    void StaticPoolList::SetPoolGrowthStep(size_t block_size, int growth_step)
    {
        c11::lock_guard<c11::mutex> lock(mutex_);
        pool_list_.SetPoolGrowthStep(block_size, growth_step);
    }

    //--------------
    int StaticPoolList::GetPoolGrowthStep(size_t block_size)
    {
        c11::lock_guard<c11::mutex> lock(mutex_);
        return pool_list_.GetPoolGrowthStep(block_size);
    }

    //--------------
    size_t StaticPoolList::GetPoolFree(size_t block_size)
    {
        c11::lock_guard<c11::mutex> lock(mutex_);
        return pool_list_.GetPoolFree(block_size);
    }

    //--------------
    size_t StaticPoolList::GetPoolSize(size_t block_size)
    {
        c11::lock_guard<c11::mutex> lock(mutex_);
        return pool_list_.GetPoolSize(block_size);
    }

    //--------------
    bool StaticPoolList::PoolIsEmpty(size_t block_size)
    {
        c11::lock_guard<c11::mutex> lock(mutex_);
        return pool_list_.PoolIsEmpty(block_size);
    }

    //--------------
    size_t StaticPoolList::GetMaxPoolBlockSize()
    {
        c11::lock_guard<c11::mutex> lock(mutex_);
        return pool_list_.GetMaxPoolBlockSize();
    }

    //--------------
    void* StaticPoolList::Pop(size_t block_size)
    {
        c11::lock_guard<c11::mutex> lock(mutex_);
        return pool_list_.Pop(block_size);
    }

    //--------------
    void StaticPoolList::Push(size_t block_size, void* ptr)
    {
        c11::lock_guard<c11::mutex> lock(mutex_);
        pool_list_.Push(block_size, ptr);
    }

    //--------------
    c11::mutex StaticPoolList::mutex_;

    //--------------
    PoolList StaticPoolList::pool_list_;

} //namespace ldl
