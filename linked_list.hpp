#include "linked_list.h"

namespace ldl {

    //---------------
    template<typename T>
    LinkedList<T>::LinkedList()
        : LinkedListBase()
    {}

    //---------------
    template<typename T>
    LinkedList<T>::LinkedList(const LinkedList& other)
        : LinkedListBase()
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
            for (const_iterator it = other.begin(); it != other.end(); ++it) {
                push_front( it->front_ptr() );
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
        return (bool)begin_;
    }

    //---------------
    template<typename T>
    size_t LinkedList<T>::size() const
    {
        size_t retval = 0;
        // use pointer to SharedPointer to avoid making temporary copies of SharedPointers
        for (const LinkablePointer* ptrptr = &begin_; (bool)(*ptrptr) ; ptrptr = &(*ptrptr)->next_) {
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
        return *front_ptr();
    }

    //---------------
    template<typename T>
    T const& LinkedList<T>::front() const
    {
        return *front_ptr();
    }

    //---------------
    template<typename T>
    typename LinkedList<T>::Pointer LinkedList<T>::front_ptr()
    {
        return dynamic_cast<Pointer>(begin_);
    }

    //---------------
    template<typename T>
    void LinkedList<T>::push_front(Pointer ptr)
    {
        LinkablePointer tmp(ptr); //new non-empty LinkablePointer (tmp = ptr, ptr->next_= 0 )
        begin_.swap(tmp->next_); // (ptr->next_ = begin_; begin_ = 0
        begin_.swap(tmp); //( begin_ = ptr; tmp = 0
    }

    //---------------
    template<typename T>
    void LinkedList<T>::pop_front()
    {
        LinkablePointer tmp; // new empty LinkablePointer (tmp = 0)
        tmp.swap(begin_->next_); // (tmp = begin_->next; begin_->next = 0) 
        begin_.swap(tmp); // begin_ = begin_->next_; tmp = old_begin
        tmp.reset(); //(tmp = 0); // reset old_begin
    }

} //namespasce ldl