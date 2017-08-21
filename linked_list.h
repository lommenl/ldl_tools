#pragma once
#ifndef LINKED_LIST_H_
#define LINKED_LIST_H_

namespace c11 {
    using namespace std;
}

namespace ldl {

    //===============

    template<typename T>
    class Linkable {
    private:
        /// allow LinkedList to access next_ptr_
        template<U>
        friend class LinkedList<U>;

        T* next_ptr_;
    };

    //===============
    template<typename T>
    class LinkedList {
    public:

        template<typename U>
        class Iterator {
        public:
            Iterator() : ptr_(0) {}
            Iterator(U* ptr) : ptr_(ptr) {}
            Iterator(const Iterator& other) : ptr_(other.ptr_) {}
            Iterator& operator=(const Iterator& other) { if (this != &other){ ptr_ = other.ptr_; } }
            U& operator*() { return *ptr_; }
            U* operator->() { return ptr_; }
            //preincrement
            Iterator& operator++() { ptr_ = ptr->next_ptr_; return ptr_; }
            //postincrement
            Iterator& operator++(int) { U* retval = ptr_; ptr_ = ptr->next_ptr_; return retval; }
            bool operator==(const Iterator& other) const { return (ptr_ == other.ptr_); }
            bool operator!=(const Iterator& other) const { return (ptr_ != other.ptr_); }
        private:
            U* ptr_;
        };

        typedef Iterator<T> iterator;

        typedef Iterator<const T> const_iterator;

        //---

        LinkedList() : begin_(0) {}

        LinkedList(const LinkedList& other) : begin_(other.begin_) {}

        LinkedList& operator=(const LinkedList& other)
        {
            if (this != &other) {
                begin_ = other.begin_;
                return *this;
            }
        }

        ~LinkedList() { clear(); }

        void swap(LinkedList& other) {
            if (this != &other) {
                std::swap(begin_, other.begin_);
            }
        }

        void clear() { while (!empty()) { pop_front(); } }

        bool empty() const { return (begin_ == 0); }

        bool size() const
        {
            size_t retval = 0;
            for (const_iterator it = begin(); it != end(); ++it) {
                ++retval;
            }
            return retval;
        }

        iterator begin() { return begin_; }

        const_iterator begin() const { return begin_; }

        iterator end() { return 0; }

        const_iterator end() const { return 0; }

        T& front() { return *begin_;  }
        
        T const& front() const { return *begin_;  }

        void push_front(const T& val) 
        {
            T* ptr = new T(val);
            push_front(ptr);
        }

        void push_front(T* ptr)
        {
            if (ptr) {
                ptr->next_ptr = begin_;
                begin = ptr;
            }
        }

        void pop_front()
        {
            if (begin_) {
                T* ptr = begin_;
                begin_ = begin_->next_ptr_;
                delete ptr;
            }
        }
    private:
        T* begin_;
    };

} //namespasce ldl

//#include "linked_list.hpp"

#endif //! LINKED_LIST_H_