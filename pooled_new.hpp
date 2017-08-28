#include "pooled_new.h"

#include "pool_allocator.h" //PoolAllocator<void>

#include <exception>

namespace ldl {

    //---------------------
    template<typename T>
    void* PooledNew<T>::operator new(size_t n)
    {
        // make sure we get expected size, otherwise, operator delete() won't work.
        if (n != element_size_) {
            throw std::bad_alloc();
        }
        return PoolAllocator<void>::Pop(element_size_);
    }

    //---------------------
    template<typename T>
    void PooledNew<T>::operator delete(void* ptr)
    {
        PoolAllocator<void>::Push(element_size_, ptr);
    }

    //---------------------
    template<typename T>
    void PooledNew<T>::IncreasePoolSize(size_t num_blocks)
    {
        PoolAllocator<void>::IncreasePoolSize(element_size_, num_blocks);
    }

    //---------------------
    template<typename T>
    void PooledNew<T>::SetPoolGrowthStep(int growth_step)
    {
        PoolAllocator<void>::SetPoolGrowthStep(element_size_, growth_step);
    }

    //---------------------
    template<typename T>
    int PooledNew<T>::GetPoolGrowthStep()
    {
        return PoolAllocator<void>::GetPoolGrowthStep(element_size_);
    }

    //---------------------
    template<typename T>
    size_t PooledNew<T>::GetPoolFree()
    {
        return PoolAllocator<void>::GetPoolFree(element_size_);
    }

    //---------------------
    template<typename T>
    size_t PooledNew<T>::GetPoolSize()
    {
        return PoolAllocator<void>::GetPoolSize(element_size_);
    }

    //---------------------
    template<typename T>
    bool PooledNew<T>::PoolIsEmpty()
    {
        return PoolAllocator<void>::PoolIsEmpty(element_size_);
    }

    //==================

    //---------------------
    template<typename T>
    void* PooledNew<T>::operator new[](size_t n)
    {
        // increase block_size to include an extra size_t to hold block_size.
        size_t block_size = sizeof(size_t) + n;
        size_t* block_size_ptr = static_cast<size_t*>(PoolAllocator<void>::Pop(block_size));
        *block_size_ptr = block_size;
        // return pointer to memory after block_size.
        return static_cast<void*>(block_size_ptr + 1);
    }

        //---------------------
        template<typename T>
    void PooledNew<T>::operator delete[](void* ptr)
    {
        // set ptr to true start of block (before ptr)
        size_t* block_size_ptr = static_cast<size_t*>(ptr) - 1;
        PoolAllocator<void>::Push(*block_size_ptr, block_size_ptr);
    }

        //---------------------
        template<typename T>
    void PooledNew<T>::IncreaseArrayPoolSize(size_t numel, size_t num_blocks)
    {
        // increase external block_size to include block_size_ptr
        size_t block_size = sizeof(size_t) + numel * element_size_;
        PoolAllocator<void>::IncreasePoolSize(block_size, num_blocks);
    }

    //---------------------
    template<typename T>
    void PooledNew<T>::SetArrayPoolGrowthStep(size_t numel, int growth_step)
    {
        // increase external block_size to include block_size_ptr
        size_t block_size = sizeof(size_t) + numel * element_size_;
        PoolAllocator<void>::SetGrowthStep(block_size, growth_step);
    }

    //---------------------
    template<typename T>
    int PooledNew<T>::GetArrayPoolGrowthStep(size_t numel)
    {
        // increase external block_size to include block_size_ptr
        size_t block_size = sizeof(size_t) + numel * element_size_;
        return PoolAllocator<void>::GetPoolGrowthStep(block_size);
    }

    //---------------------
    template<typename T>
    size_t PooledNew<T>::GetArrayPoolFree(size_t numel)
    {
        // increase external block_size to include block_size_ptr
        size_t block_size = sizeof(size_t) + numel * element_size_;
        return PoolAllocator<void>::GetPoolFree(block_size);
    }

    //---------------------
    template<typename T>
    size_t PooledNew<T>::GetArrayPoolSize(size_t numel)
    {
        // increase external block_size to include block_size_ptr
        size_t block_size = sizeof(size_t) + numel * element_size_;
        return PoolAllocator<void>::GetPoolSize(block_size);
    }

    //---------------------
    template<typename T>
    bool PooledNew<T>::GetArrayPoolIsEmpty(size_t numel)
    {
        // increase external block_size to include block_size_ptr
        size_t block_size = sizeof(size_t) + numel * element_size_;
        return PoolAllocator<void>::GetPoolIsEmpty(block_size);
    }

    //---------------------
    template<typename T>
    size_t PooledNew<T>::element_size_ = sizeof(T);

} //namespace ldl
