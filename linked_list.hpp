#include "linked_list.h"

namespace ldl {

    //---------------
    template<typename T>
    LinkedList<T>::LinkedList()
        : begin_(0)
    {}

    //---------------
    template<typename T>
    LinkedList<T>::LinkedList(const LinkedList& other)
        : begin_(0)
    {
        operator=(other);
    }

    //---------------
    template<typename T>
    LinkedList<T>::~LinkedList()
    {
        clear();
    }

    //---------------
    template<typename T>
    LinkedList<T>& LinkedList<T>::operator=(const LinkedList& other)
    {
        if (this != &other) {
            clear();
            LinkedList tmp;
            //copy elements from other into tmp. (tmp is in reverse order)
            for (const_iterator it = other.begin(); it != other.end(); ++it) {
                tmp.push_front( *it );
            }
            // move pointers in tmp into *this (*this is in original order)
            for (LinkedList* ptr = tmp.begin_; ptr != 0; ptr = next_linkable) {
                // push_front(ptr) clobbers ptr->next_linkable_
                LinkedList* next_linkable = ptr->next_linkable_;
                push_front(ptr);
            }
        }
        return *this;
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
            pop_front(); // remove and delete
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
        for ( const_iterator it = begin(); it != end(); ++it) {
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
        return *static_cast<T*>(begin_);
    }

    //---------------
    template<typename T>
    T const& LinkedList<T>::front() const
    {
        return *static_cast<T*>(begin_);
    }

    //---------------
    template<typename T>
    void LinkedList<T>::push_front(T* ptr)
    {
        if (!ptr) {
            throw std::runtime_error("invalid pointer argument");
        }
        ptr->next_linkable_ = begin_;
        begin_ = ptr;
    }

    //---------------
    template<typename T>
    void LinkedList<T>::push_front(const T& ptr)
    {
        push_front(new T(ptr));
    }

    //---------------
    template<typename T>
    void LinkedList<T>::pop_front()
    {
        if (begin_) {
            T* ptr = static_cast<T*>(begin_);
            begin_ = begin_->next_linkable_;
            delete ptr;
        }
    }

    //-----------------
    template<typename T>
    const size_t LinkedList<T>::element_size_ = sizeof(LinkedList<T>);

} //namespasce ldl
