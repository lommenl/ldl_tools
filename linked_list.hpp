#include "linked_list.h"

namespace ldl {

    //---------------
    template<typename T>
    template<typename U>
    LinkedList<T>::Iterator<U>::Iterator()
        : ptr_(0)
    {}

    //---------------
    template<typename T>
    template<typename U>
    LinkedList<T>::Iterator<U>::Iterator(U* ptr)
        : ptr_(ptr)
    {}

    //---------------
    template<typename T>
    template<typename U>
    LinkedList<T>::Iterator<U>::Iterator(const Iterator& other)
        : ptr_(other.ptr_)
    {}

    //---------------
    template<typename T>
    template<typename U>
    LinkedList<T>::Iterator<U>& LinkedList<T>::Iterator<U>::operator=(const Iterator& other)
    {
        if (this != &other) {
            ptr_ = other.ptr_;
        }
    }

    //---------------
    template<typename T>
    template<typename U>
    U& LinkedList<T>::Iterator<U>::operator*()
    {
        return *ptr_;
    }

    //---------------
    template<typename T>
    template<typename U>
    U* LinkedList<T>::Iterator<U>::operator->()
    {
        return ptr_;
    }

    //---------------
    template<typename T>
    template<typename U>
    LinkedList<T>::Iterator<U>& LinkedList<T>::Iterator<U>::operator++()
    {
        ptr_ = ptr_->next_ptr_;
        return *this;
    }

    //---------------
    template<typename T>
    template<typename U>
    LinkedList<T>::Iterator<U>& LinkedList<T>::Iterator<U>::operator++(int)
    {
        Iterator retval = *this;
        ptr_ = ptr_->next_ptr_;
        return retval;
    }

    //---------------
    template<typename T>
    template<typename U>
    bool LinkedList<T>::Iterator<U>::operator==(const Iterator& other) const
    {
        return (ptr_ == other.ptr_);
    }

    //---------------
    template<typename T>
    template<typename U>
    bool LinkedList<T>::Iterator<U>::operator!=(const Iterator& other) const
    {
        return (ptr_ != other.ptr_);
    }

    //======================================

    //---------------
    template<typename T>
    LinkedList<T>::LinkedList()
        : begin_(0)
    {}

    //---------------
    template<typename T>
    LinkedList<T>::LinkedList(const LinkedList& other)
        : begin_(other.begin_)
    {}

    //---------------
    template<typename T>
    LinkedList<T>& LinkedList<T>::operator=(const LinkedList& other)
    {
        if (this != &other) {
            begin_ = other.begin_;
            return *this;
        }
    }

    //---------------
    template<typename T>
    LinkedList<T>::~LinkedList()
    {
        clear();
    }

    //---------------
    template<typename T>
    void LinkedList<T>::swap(LinkedList& other)
    {
        if (this != &other) {
            std::swap(begin_, other.begin_);
        }
    }

    //---------------
    template<typename T>
    void LinkedList<T>::clear()
    {
        while (!empty()) {
            pop_front();
        }
    }

    //---------------
    template<typename T>
    bool LinkedList<T>::empty() const
    {
        return (begin_ == 0);
    }

    //---------------
    template<typename T>
    size_t LinkedList<T>::size() const
    {
        size_t retval = 0;
        for (const_iterator it = begin(); it != end(); ++it) {
            ++retval;
        }
        return retval;
    }

    //---------------
    template<typename T>
    typename LinkedList<T>::iterator LinkedList<T>::begin()
    {
        return iterator(begin_);
    }

    //---------------
    template<typename T>
    typename LinkedList<T>::const_iterator LinkedList<T>::begin() const
    {
        return const_iterator(begin_);
    }

    //---------------
    template<typename T>
    typename LinkedList<T>::const_iterator LinkedList<T>::cbegin()
    {
        return const_iterator(begin_);
    }

    //---------------
    template<typename T>
    typename LinkedList<T>::iterator LinkedList<T>::end()
    {
        return iterator(0);
    }

    //---------------
    template<typename T>
    typename LinkedList<T>::const_iterator LinkedList<T>::end() const
    {
        return const_iterator(0);
    }

    //---------------
    template<typename T>
    typename LinkedList<T>::const_iterator LinkedList<T>::cend()
    {
        return const_iterator(0);
    }

    //---------------
    template<typename T>
    T& LinkedList<T>::front() {
        return *begin_;
    }

    //---------------
    template<typename T>
    T const& LinkedList<T>::front() const
    {
        return *begin_;
    }

    //---------------
    template<typename T>
    void LinkedList<T>::push_front(const T& val)
    {
        push_front(new T(val));
    }

    //---------------
    template<typename T>
    void LinkedList<T>::push_front(T* ptr)
    {
        if (ptr) {
            ptr->next_ptr_ = begin_;
            begin_ = ptr;
        }
    }

    //---------------
    template<typename T>
    void LinkedList<T>::pop_front()
    {
        if (begin_) {
            T* ptr = begin_;
            begin_ = begin_->next_ptr_;
            delete ptr;
        }
    }

} //namespasce ldl