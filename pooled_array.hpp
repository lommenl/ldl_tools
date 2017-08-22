#include "pooled_array.h"

#include <algorithm>

namespace ldl {

    //-------------------
    template<typename T>
    PooledArray<T>::PooledArray()
        : numel_(0)
        , data_(0)
    {
    }

    //-------------------
    template<typename T>
    PooledArray<T>::PooledArray(const PooledArray& other)
        : numel_(0)
        , data_(0)
    {
        operator=(other);
    }

    //-------------------
    template<typename T>
    PooledArray<T>::PooledArray(std::size_t numel)
        : numel_(0)
        , data_(0)
    {
        SetSize(numel);
    }

    //-------------------
    template<typename T>
    PooledArray<T>::~PooledArray()
    {
        DestroyData();
    }

    //-------------------
    template<typename T>
    PooledArray<T>& PooledArray<T>::operator=(const PooledArray& other)
    {
        if (this != &other) {
            DestroyData();
            ConstructData(other.numel_);
            std::copy(begin(), end(), other.begin()); // copy elements
        }
        return *this;
    }

    //-------------------
    template<typename T>
    void PooledArray<T>::SetSize(std::size_t numel)
    {
        if (numel_ != 0 || data_ != 0) { throw std::runtime_error("PooledArray already initialized"); }
        ConstructData(numel);
    }

    //-------------------
    template<typename T>
    void PooledArray<T>::swap(PooledArray& other)
    {
        if (this != &other) {
            std::swap(numel_, other.numel_);
            std::swap(data_, other.data_);
        }
    }

    //-------------------
    template<typename T>
    typename PooledArray<T>::iterator PooledArray<T>::begin()
    {
        return data_;
    }

    //-------------------
    template<typename T>
    typename PooledArray<T>::const_iterator PooledArray<T>::begin() const
    {
        return data_;
    }

    //-------------------
    template<typename T>
    typename PooledArray<T>::const_iterator PooledArray<T>::cbegin() const
    {
        return data_;
    }

    //-------------------
    template<typename T>
    typename PooledArray<T>::iterator PooledArray<T>::end()
    {
        return data_ + numel_;
    }

    //-------------------
    template<typename T>
    typename PooledArray<T>::const_iterator PooledArray<T>::end() const
    {
        return data_ + numel_;
    }

    //-------------------
    template<typename T>
    typename PooledArray<T>::const_iterator PooledArray<T>::cend() const
    {
        return data_ + numel_;
    }

    //-------------------
    template<typename T>
    std::size_t PooledArray<T>::size() const
    {
        return numel_;
    }

    //-------------------
    template<typename T>
    std::size_t PooledArray<T>::max_size() const
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
    typename PooledArray<T>::value_type& PooledArray<T>::operator[](std::size_t n)
    {
        return data_[n];
    }

    //-------------------
    template<typename T>
    typename PooledArray<T>::value_type const& PooledArray<T>::operator[](std::size_t n) const
    {
        return data_[n];
    }

    //-------------------
    template<typename T>
    typename PooledArray<T>::value_type& PooledArray<T>::at(std::size_t n)
    {
        return data_[n];
    }

    //-------------------
    template<typename T>
    typename PooledArray<T>::value_type const& PooledArray<T>::at(std::size_t n) const
    {
        return data_[n];
    }

    //-------------------
    template<typename T>
    typename PooledArray<T>::value_type& PooledArray<T>::front()
    {
        return data_[0];
    }

    //-------------------
    template<typename T>
    typename PooledArray<T>::value_type const& PooledArray<T>::front() const
    {
        return data_[0];
    }

    //-------------------
    template<typename T>
    typename PooledArray<T>::value_type& PooledArray<T>::back()
    {
        return data_[numel_ - 1];
    }

    //-------------------
    template<typename T>
    typename PooledArray<T>::value_type const& PooledArray<T>::back() const
    {
        return data_[numel_ - 1];
    }

    //-------------------
    template<typename T>
    typename PooledArray<T>::value_type* PooledArray<T>::data()
    {
        return data_;
    }

    //-------------------
    template<typename T>
    typename PooledArray<T>::value_type const* PooledArray<T>::data() const
    {
        return data_;
    }

    //-------------------
    template<typename T>
    void PooledArray<T>::ConstructData(std::size_t numel)
    {
        if (data_!=0 || numel_!=0 || numel == 0) { throw std::bad_alloc(); }
        numel_ = numel;
        // get memory from pool
        data_ = static_cast<value_type*>(pool_list_.Pop(numel_ * sizeof(value_type)));
        // call default constructor on each element.
        data_ = new(static_cast<void*>(data_)) value_type[numel_];
    }

    //-------------------
    template<typename T>
    void PooledArray<T>::DestroyData()
    {
        if (data_ && numel_) {
            // call destructor on each element
            for (iterator it = begin(); it != end(); ++it) {
                it->~value_type();
            }
            // return memory to pool
            pool_list_.Push(numel_ * sizeof(value_type), static_cast<void*>(data_));
        }
        numel_ = 0;
        data_ = 0;
    }

    //-------------------
    template<typename T>
    void PooledArray<T>::IncreasePoolSize(std::size_t numel, std::size_t num_blocks)
    {
        pool_list_.IncreasePoolSize(numel * sizeof(value_type), num_blocks);
    }

    //-------------------
    template<typename T>
    void PooledArray<T>::SetPoolGrowthStep(std::size_t numel, int growth_step)
    {
        pool_list_.SetPoolGrowthStep(numel * sizeof(value_type), growth_step);
    }

    //-------------------
    template<typename T>
    int PooledArray<T>::GetPoolGrowthStep(std::size_t numel)
    {
        return pool_list_.GetPoolGrowthStep(numel * sizeof(value_type));
    }

    //-------------------
    template<typename T>
    std::size_t PooledArray<T>::GetPoolFree(std::size_t numel)
    {
        return pool_list_.GetPoolFree(numel * sizeof(value_type));
    }

    //-------------------
    template<typename T>
    std::size_t PooledArray<T>::GetPoolSize(std::size_t numel)
    {
        return pool_list_.GetPoolSize(numel * sizeof(value_type));
    }

    //-------------------
    template<typename T>
    PoolList PooledArray<T>::pool_list_;

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