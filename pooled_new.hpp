#include "pooled_new.h"

#include <exception>

#include<functional> //std::function, std::bind()
namespace c11 {
    using namespace std;
}

namespace ldl {

    //---------------------
    template<typename T>
    void PooledNew<T>::IncreasePoolSize(std::size_t num_blocks)
    {
        pool_list_.IncreasePoolSize(sizeof(T),num_blocks);
    }

    //---------------------
    template<typename T>
    void PooledNew<T>::SetPoolGrowthStep(int growth_step)
    {
        pool_list_.SetPoolGrowthStep(sizeof(T),growth_step);
    }

    //---------------------
    template<typename T>
    int PooledNew<T>::GetPoolGrowthStep()
    {
        return pool_list_.GetPoolGrowthStep(sizeof(T));
    }

    //---------------------
    template<typename T>
    std::size_t PooledNew<T>::GetPoolFree()
    {
        return pool_list_.GetPoolFree(sizeof(T));
    }

    //---------------------
    template<typename T>
    std::size_t PooledNew<T>::GetPoolSize()
    {
        return pool_list_.GetPoolSize(sizeof(T));
    }

    //---------------------
    template<typename T>
    void* PooledNew<T>::operator new(std::size_t n)
    {
        if (n != sizeof(T)) { throw std::bad_alloc(); }
        return pool_list_.Pop(sizeof(T));
    }

    //---------------------
    template<typename T>
    void PooledNew<T>::operator delete(void* ptr)
    {
        pool_list_.Push(sizeof(T),ptr);
    }

    //==================

    //---------------------
    template<typename T>
    void PooledNew<T>::IncreaseArrayPoolSize(std::size_t numel, std::size_t num_blocks)
    {
        // increase external block_size to include block_size_ptr
        size_t block_size = sizeof(std::size_t) + numel * sizeof(T);
        pool_list_.IncreasePoolSize(block_size,num_blocks);
    }

    //---------------------
    template<typename T>
    void PooledNew<T>::SetArrayPoolGrowthStep(std::size_t numel, int growth_step)
    {
        // increase external block_size to include block_size_ptr
        size_t block_size = sizeof(std::size_t) + numel * sizeof(T);
        pool_list_.SetGrowthStep(block_size,growth_step);
    }

    //---------------------
    template<typename T>
    int PooledNew<T>::GetArrayPoolGrowthStep(std::size_t numel)
    {
        // increase external block_size to include block_size_ptr
        size_t block_size = sizeof(std::size_t) + numel * sizeof(T);
        return pool_list_.GetGrowthStep(block_size);
    }

    //---------------------
    template<typename T>
    std::size_t PooledNew<T>::GetArrayPoolFree(std::size_t numel)
    {
        // increase external block_size to include block_size_ptr
        size_t block_size = sizeof(std::size_t) + numel * sizeof(T);
        return pool_list_.GetPoolFree(block_size);
    }

    //---------------------
    template<typename T>
    std::size_t PooledNew<T>::GetArrayPoolSize(std::size_t numel)
    {
        // increase external block_size to include block_size_ptr
        size_t block_size = sizeof(std::size_t) + numel * sizeof(T);
        return pool_list_.GetPoolSize(block_size);
    }

    //---------------------
    template<typename T>
    void* PooledNew<T>::operator new[](std::size_t n)
    {
        // increase block_size to include block_size
        std::size_t block_size = sizeof(std::size_t) + n;
        std::size_t* block_size_ptr = static_cast<std::size_t*>(pool_list_.Pop(block_size));
        *block_size_ptr = block_size;
        // return pointer to memory after block_size.
        return static_cast<void*>(block_size_ptr + 1); 
    }

    //---------------------
    template<typename T>
    void PooledNew<T>::operator delete[](void* ptr)
    {
        // set ptr to true start of block (before ptr)
        std::size_t* block_size_ptr = static_cast<std::size_t*>(ptr) - 1;
        pool_list_.Push(*block_size_ptr,block_size_ptr);
    }

    //---------------------
    template<typename T>
    PoolList PooledNew<T>::pool_list_;

} //namespace ldl
