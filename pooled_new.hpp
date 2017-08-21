#include "pooled_new.h"

#include <exception>

#include<functional> //std::function, std::bind()
namespace c11 {
    using namespace std;
}

namespace ldl {

    //---------------------
    template<typename T>
    void PooledNew<T>::ResizePool(std::size_t num_blocks)
    {
        pool_.Resize(num_blocks);
    }

    //---------------------
    template<typename T>
    void PooledNew<T>::IncreasePoolSize(std::size_t num_blocks)
    {
        pool_.IncreaseSizeBy(num_blocks);
    }

    //---------------------
    template<typename T>
    void PooledNew<T>::SetPoolGrowthStep(int growth_step)
    {
        pool_.SetGrowthStep(growth_step);
    }

    //---------------------
    template<typename T>
    int PooledNew<T>::GetPoolGrowthStep()
    {
        return pool_.GetGrowthStep();
    }

    //---------------------
    template<typename T>
    std::size_t PooledNew<T>::GetPoolBlockSize()
    {
        return block_size;
    }

    //---------------------
    template<typename T>
    std::size_t PooledNew<T>::GetPoolFree()
    {
        return pool_.GetFree();
    }

    //---------------------
    template<typename T>
    std::size_t PooledNew<T>::GetPoolCapacity()
    {
        return pool_.GetCapacity();
    }

    //---------------------
    template<typename T>
    void* PooledNew<T>::operator new(std::size_t nbytes)
    {
        if (nbytes != pool_.GetBlockSize()) { throw std::bad_alloc(); }
        return pool_.Pop();
    }

    //---------------------
    template<typename T>
    void PooledNew<T>::operator delete(void* ptr)
    {
        pool_.Push(ptr);
    }

    //---------------------
    template<typename T>
    Pool PooledNew<T>::pool_(sizeof(T), 0, 0); // empty buffer

} //namespace ldl
