#include "pooled_array.h"

#include <algorithm>

namespace ldl {

    //---------------
    template<typename T, size_t N>
    void PooledArray<T, N>::fill(const T& val)
    {
        std::fill(begin(), end(), val);
    }

    //---------------
    template<typename T, size_t N>
    void PooledArray<T, N>::swap(PooledArray& other)
    {
        if (this != &other) {
            std::swap(array_, other.array_);
        }
    }

    //---------------
    template<typename T, size_t N>
    bool PooledArray<T, N>::empty() const
    {
        return (N == 0);
    }

    //---------------
    template<typename T, size_t N>
    size_t PooledArray<T, N>::size() const
    {
        return N;
    }

    //---------------
    template<typename T, size_t N>
    size_t PooledArray<T, N>::max_size() const
    {
        return N;
    }


    //---------------
    template<typename T, size_t N>
    T& PooledArray<T, N>::operator[](size_t n)
    {
        return array_[n];
    }

    //---------------
    template<typename T, size_t N>
    const T& PooledArray<T, N>::operator[](size_t n) const
    {
        return array_[n];
    }

    //---------------
    template<typename T, size_t N>
    T& PooledArray<T, N>::at(size_t n)
    {
        if (n >= N) {
            throw std::runtime_error("Index out of range");
        }
        return array_[n];
    }

    //---------------
    template<typename T, size_t N>
    const T& PooledArray<T, N>::at(size_t n) const
    {
        if (n >= N) {
            throw std::runtime_error("Index out of range");
        }
        return array_[n];
    }

    //---------------
    template<typename T, size_t N>
    T& PooledArray<T, N>::front() {
        return array_[0];
    }

    //---------------
    template<typename T, size_t N>
    const T & PooledArray<T, N>::front() const
    {
        return array_[0];
    }

    //---------------
    template<typename T, size_t N>
    T& PooledArray<T, N>::back() {
        return array_[N - 1];
    }

    //---------------
    template<typename T, size_t N>
    const T& PooledArray<T, N>::back() const
    {
        return array_[N - 1];
    }

    //---------------
    template<typename T, size_t N>
    T* PooledArray<T, N>::data() {
        return array_;
    }

    //---------------
    template<typename T, size_t N>
    const T* PooledArray<T, N>::data() const
    {
        return array_;
    }

    //---------------
    template<typename T, size_t N>
    typename PooledArray<T, N>::iterator PooledArray<T, N>::begin()
    {
        return array_;
    }

    //---------------
    template<typename T, size_t N>
    typename PooledArray<T, N>::iterator PooledArray<T, N>::end()
    {
        return array_ + N;
    }

    //---------------
    template<typename T, size_t N>
    typename PooledArray<T, N>::const_iterator PooledArray<T, N>::begin() const
    {
        return array_;
    }

    //---------------
    template<typename T, size_t N>
    typename PooledArray<T, N>::const_iterator PooledArray<T, N>::end() const
    {
        return array_ + N;
    }

    //---------------
    template<typename T, size_t N>
    typename PooledArray<T, N>::const_iterator PooledArray<T, N>::cbegin()
    {
        return array_;
    }

    //---------------
    template<typename T, size_t N>
    typename PooledArray<T, N>::const_iterator PooledArray<T, N>::cend()
    {
        return array_ + N;
    }

    //---------------
    template<typename T, size_t N>
    typename PooledArray<T, N>::reverse_iterator PooledArray<T, N>::rbegin()
    {
        return std::reverse_allocator<allocator>(end());
    }

    //---------------
    template<typename T, size_t N>
    typename PooledArray<T, N>::reverse_iterator PooledArray<T, N>::rend()
    {
        return std::reverse_allocator<allocator>(begin());
    }

    //---------------
    template<typename T, size_t N>
    typename PooledArray<T, N>::const_reverse_iterator PooledArray<T, N>::rbegin() const
    {
        return std::const_reverse_allocator<allocator>(end());
    }

    //---------------
    template<typename T, size_t N>
    typename PooledArray<T, N>::const_reverse_iterator PooledArray<T, N>::rend() const
    {
        return std::const_reverse_allocator<allocator>(begin());
    }

    //---------------
    template<typename T, size_t N>
    typename PooledArray<T, N>::const_reverse_iterator PooledArray<T, N>::crbegin()
    {
        return std::const_reverse_allocator<allocator>(cend());
    }

    //---------------
    template<typename T, size_t N>
    typename PooledArray<T, N>::const_reverse_iterator PooledArray<T, N>::crend()
    {
        return std::const_reverse_allocator<allocator>(cbegin());
    }

    //-----------------
    template<typename T, size_t N>
    const size_t PooledArray<T, N>::element_size_ = sizeof(PooledArray<T, N>);

    //-----------------
    template<typename T, size_t N>
    bool operator==(const PooledArray<T, N>& lhs, const PooledArray<T, N>& rhs)
    {
        return std::equal(lhs.begin(), lhs.end(), rhs.begin());
    }

    //-----------------
    template<typename T, size_t N>
    bool operator!=(const PooledArray<T, N>& lhs, const PooledArray<T, N>& rhs)
    {
        return !(lhs == rhs);
    }

    //-----------------
    template<typename T, size_t N>
    bool operator<(const PooledArray<T, N>& lhs, const PooledArray<T, N>& rhs)
    {
        return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
    }

    //-----------------
    template<typename T, size_t N>
    bool operator>(const PooledArray<T, N>& lhs, const PooledArray<T, N>& rhs)
    {
        return (rhs < lhs);
    }

    //-----------------
    template<typename T, size_t N>
    bool operator<=(const PooledArray<T, N>& lhs, const PooledArray<T, N>& rhs)
    {
        return !(rhs < lhs);
    }

    //-----------------
    template<typename T, size_t N>
    bool operator>=(const PooledArray<T, N>& lhs, const PooledArray<T, N>& rhs)
    {
        return !(lhs < rhs);
    }

    //-----------------
    template<typename T,size_t N>
    std::ostream& operator<<(std::ostream& os, const PooledArray<T, N>& obj)
    {
        return os << "PooledArray<T," << N << ">";
    }
} //namespasce ldl
