#include "pooled_array.h"

#include <algorithm> // std::copy

#include "Pool.h" //StaticPoolList

namespace ldl {

    //-------------------
    template<typename T>
    PooledArray<T>::PooledArray()
        : numel_(0)
        , buffer_(0)
    {
    }

    //-------------------
    template<typename T>
    PooledArray<T>::PooledArray(const PooledArray& other)
        : numel_(0)
        , buffer_(0)
    {
        operator=(other);
        PooledNew::IncreasePoolSize(1);
    }

    //-------------------
    template<typename T>
    PooledArray<T>::PooledArray(size_t numel)
        : numel_(0)
        , buffer_(0)
    {
        Initialize(numel);
    }

    //-------------------
    template<typename T>
    PooledArray<T>::~PooledArray()
    {
        DeleteBuffer();
    }

    //-------------------
    template<typename T>
    PooledArray<T>& PooledArray<T>::operator=(const PooledArray& other)
    {
        if (this != &other) {
            DeleteBuffer();
            NewBuffer(other.numel_);
            std::copy(begin(), end(), other.begin()); // copy elements
        }
        return *this;
    }

    //-------------------
    template<typename T>
    void PooledArray<T>::Initialize(size_t numel)
    {
        if (numel_ != 0 || buffer_ != 0) {
            throw std::runtime_error("PooledArray already initialized");
        }
        NewBuffer(numel);
    }

    //-------------------
    template<typename T>
    void PooledArray<T>::swap(PooledArray& other)
    {
        if (this != &other) {
            std::swap(numel_, other.numel_);
            std::swap(buffer_, other.buffer_);
            std::swap(buffer_element_size_, other.buffer_element_size_);
        }
    }

    //-------------------
    template<typename T>
    typename PooledArray<T>::iterator PooledArray<T>::begin()
    {
        return buffer_;
    }

    //-------------------
    template<typename T>
    typename PooledArray<T>::const_iterator PooledArray<T>::begin() const
    {
        return buffer_;
    }

    //-------------------
    template<typename T>
    typename PooledArray<T>::const_iterator PooledArray<T>::cbegin() const
    {
        return buffer_;
    }

    //-------------------
    template<typename T>
    typename PooledArray<T>::iterator PooledArray<T>::end()
    {
        return buffer_ + numel_;
    }

    //-------------------
    template<typename T>
    typename PooledArray<T>::const_iterator PooledArray<T>::end() const
    {
        return buffer_ + numel_;
    }

    //-------------------
    template<typename T>
    typename PooledArray<T>::const_iterator PooledArray<T>::cend() const
    {
        return buffer_ + numel_;
    }

    //-------------------
    template<typename T>
    size_t PooledArray<T>::size() const
    {
        return numel_;
    }

    //-------------------
    template<typename T>
    size_t PooledArray<T>::max_size() const
    {
        return numel_;
    }

    //-------------------
    template<typename T>
    PooledArray<T>::operator bool() const
    {
        return (numel_ != 0);
    }

    //-------------------
    template<typename T>
    void PooledArray<T>::fill(const value_type& val)
    {
        std::fill(begin(), end(), val);
    }

    //-------------------
    template<typename T>
    typename PooledArray<T>::value_type& PooledArray<T>::operator[](size_t n)
    {
        return buffer_[n];
    }

    //-------------------
    template<typename T>
    typename PooledArray<T>::value_type const& PooledArray<T>::operator[](size_t n) const
    {
        return buffer_[n];
    }

    //-------------------
    template<typename T>
    typename PooledArray<T>::value_type& PooledArray<T>::at(size_t n)
    {
        return buffer_[n];
    }

    //-------------------
    template<typename T>
    typename PooledArray<T>::value_type const& PooledArray<T>::at(size_t n) const
    {
        return buffer_[n];
    }

    //-------------------
    template<typename T>
    typename PooledArray<T>::value_type& PooledArray<T>::front()
    {
        return buffer_[0];
    }

    //-------------------
    template<typename T>
    typename PooledArray<T>::value_type const& PooledArray<T>::front() const
    {
        return buffer_[0];
    }

    //-------------------
    template<typename T>
    typename PooledArray<T>::value_type& PooledArray<T>::back()
    {
        return buffer_[numel_ - 1];
    }

    //-------------------
    template<typename T>
    typename PooledArray<T>::value_type const& PooledArray<T>::back() const
    {
        return buffer_[numel_ - 1];
    }

    //-------------------
    template<typename T>
    typename PooledArray<T>::value_type* PooledArray<T>::data()
    {
        return buffer_;
    }

    //-------------------
    template<typename T>
    typename PooledArray<T>::value_type const* PooledArray<T>::data() const
    {
        return buffer_;
    }

    //-------------------
    template<typename T>
    void PooledArray<T>::NewBuffer(size_t numel)
    {
        if (buffer_ == 0 || buffer_element_size_ == 0 || numel == 0) {
            throw std::bad_alloc();
        }
        numel_ = numel;
        // get memory from pool
        size_t block_size = numel_ * buffer_element_size_;
        buffer_ = static_cast<value_type*>(StaticPoolList::Pop(block_size));
        buffer_ = new(static_cast<void*>(buffer_)) value_type[numel_];
    }

    //-------------------
    template<typename T>
    void PooledArray<T>::DeleteBuffer()
    {
        if (buffer_ && numel_) {
            // call destructor on each element
            for (iterator it = begin(); it != end(); ++it) {
                it->~value_type();
            }
            // return memory to pool
            size_t block_size = numel_ * buffer_element_size_;
            StaticPoolList::Push(block_size, static_cast<void*>(buffer_));
        }
        numel_ = 0;
        buffer_ = 0;
    }

    //-------------------
    template<typename T>
    void PooledArray<T>::IncreaseBufferPoolSize(size_t numel, size_t num_blocks)
    {
        size_t block_size = numel * buffer_element_size_;
        StaticPoolList::IncreasePoolSize(block_size, num_blocks);
    }

    //-------------------
    template<typename T>
    void PooledArray<T>::SetBufferPoolGrowthStep(size_t numel, int growth_step)
    {
        size_t block_size = numel * buffer_element_size_;
        StaticPoolList::SetPoolGrowthStep(block_size, growth_step);
    }

    //-------------------
    template<typename T>
    int PooledArray<T>::GetBufferPoolGrowthStep(size_t numel)
    {
        size_t block_size = numel * buffer_element_size_;
        return StaticPoolList::GetPoolGrowthStep(block_size);
    }

    //-------------------
    template<typename T>
    size_t PooledArray<T>::GetBufferPoolFree(size_t numel)
    {
        size_t block_size = numel * buffer_element_size_;
        return StaticPoolList::GetPoolFree(block_size);
    }

    //-------------------
    template<typename T>
    size_t PooledArray<T>::GetBufferPoolSize(size_t numel)
    {
        size_t block_size = numel * buffer_element_size_;
        return StaticPoolList::GetPoolSize(block_size);
    }

    //-------------------
    template<typename T>
    bool PooledArray<T>::BufferPoolIsEmpty(size_t numel)
    {
        size_t block_size = numel * buffer_element_size_;
        return StaticPoolList::PoolIsEmpty(block_size);
    }

    //-------------------
    template<typename T>
    bool operator==(const PooledArray<T>& lhs, const PooledArray<T>& rhs)
    {
        return std::equal(lhs.begin(), lhs.end(), rhs.begin());
    }

    //-------------------
    template<typename T>
    bool operator!=(const PooledArray<T>& lhs, const PooledArray<T>& rhs)
    {
        return !(lhs == rhs);
    }

    //-------------------
    template<typename T>
    bool operator<(const PooledArray<T>& lhs, const PooledArray<T>& rhs)
    {
        return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
    }

    //-------------------
    template<typename T>
    bool operator>(const PooledArray<T>& lhs, const PooledArray<T>& rhs)
    {
        return (rhs < lhs);
    }

    //-------------------
    template<typename T>
    bool operator>=(const PooledArray<T>& lhs, const PooledArray<T>& rhs)
    {
        return !(lhs < rhs);
    }

    //-------------------
    template<typename T>
    bool operator<=(const PooledArray<T>& lhs, const PooledArray<T>& rhs)
    {
        return !(rhs < lhs);
    }

} //namespace ldl