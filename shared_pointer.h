#pragma once
#ifndef SHARED_POINTER_H
#define SHARED_POINTER_H

#include <functional>
#include <mutex>
#include <ostream>

namespace c11 {
    using namespace std;
}

namespace ldl {

    /// Almost-duplicate of c11::shared_ptr that does not perform internal
    /// memory allocations. Instead it uses a linked list to identify the owners of a given pointer.
    /// If a mutex argument is provided in an object constructor, it is used to ensure thread-safe
    /// access to the list of pointer owners.
    /// The mutex is NOT deleted when the last owner deletes the mangaged object.
    template<typename T, typename M = c11::mutex>
    class SharedPointer {
    public:

        /// Type of the managed object.
        typedef T element_type;

        /// Type of mutex.
        typedef M mutex_type;

        // type signature of deleter function
        typedef c11::function<void(element_type*)> Deleter;

        //default constructor
        SharedPointer();

        // Construct from pointer and optional mutex
        template<typename U>
        SharedPointer(U* ptr, M& mx = *static_cast<M*>(0) );

        // construct from null pointer and optional mutex
        SharedPointer(nullptr_t, M& mx = *static_cast<M*>(0) );

        // construct from pointer, deleter, and optional mutex
        template<typename U>
        SharedPointer(U* ptr, Deleter del, M& mx = *static_cast<M*>(0));

        // construct from null pointer, deleter, and optional mutex
        SharedPointer(nullptr_t, Deleter del, M& mx = *static_cast<M*>(0));

        // copy constructor
        SharedPointer(SharedPointer& other);

        // copy constructor for alternate managed object type
        template<typename U>
        SharedPointer(SharedPointer<U,M>& other);

        // destructor
        ~SharedPointer();

        // copy assignment operator
        SharedPointer& operator=(SharedPointer& other);

        // copy assignment operator
        // copy assignment for alternate managed object type
        template<typename U>
        SharedPointer& operator=(SharedPointer<U,M>& other);

        // swap state of two objects.
        void swap(SharedPointer& other);

        // reset state to be as if object were default constructed.
        // releases ownership of any currently owned object.
        void reset();

        // reset object and then swap it with a new object constructed by SharedPointer(ptr,mx)
        template<typename U>
        void reset(U* ptr, M& mx = *static_cast<M*>(0) );

        // reset object and then swap it with a new object constructed by SharedPointer(ptr,del,mx)
        template<typename U>
        void reset(U* ptr, Deleter del, M& mx = *static_cast<M*>(0) );

        // return a plain pointer to managed object
        T* get() const;

        // dereference managed object
        T& operator*() const;

        // dereference managed object for calling a member function.
        T* operator->() const;

        // return number of objects sharing the managed object. (0 if this object is empty)
        long int use_count();

        // return true if this object is managing an object and it is the only owner.
        bool unique();

        // return true if this object is managing an object.
        explicit operator bool() const;

    private:

        // function class with operator()(T* ptr) member function that calls del( static_cast<U*>(ptr) )
        template< typename U>
        class CastDeleter {
            typedef c11::function<void(U*)> Deleter;
            Deleter deleter_;
        public:
            CastDeleter(Deleter del) : deleter_(del) {}
            void operator()(T* ptr) {
                deleter_(static_cast<U*>(ptr) )
            }
        };

        // type of lock guard used to control mutex.
        typedef c11::lock_guard<SharedPointer> ListLock;

        // allow ListLock class to access lock() and unlock()
        friend class ListLock;

        // function to make this class lockable, and usable as an argument to the ListLock constructor.
        // if mutex_ptr_ == 0 , it does nothing, otherise it calls mutex_ptr_.lock()
        void lock();

        // function to make this class lockable, and usable as an argument to the ListLock constructor.
        // if mutex_ptr_ == 0 , it does nothing, otherise it calls mutex_ptr_.unlock()
        void unlock();

        //------------

        // pointer to mutex
        M* mutex_ptr_;

        // pointer to previous object in linked list of owners.
        SharedPointer* prev_sp_;

        // pointer to next object in linked list of owners.
        SharedPointer* next_sp_;

        // pointer to managed object.
        element_type* obj_ptr_;

        // function object called to delete obj_ptr_
        Deleter deleter_;

    };

    // swap state of two objects.
    template<typename T, typename M>
    void swap(SharedPointer<T,M>& lhs, SharedPointer<T,M>& rhs);

    // Equality comparison operators
    template<typename T, typename M>
    bool operator==(const SharedPointer<T,M>& lhs, const SharedPointer<T,M>& rhs);
    template<typename T, typename M>
    bool operator==(nullptr_t, const SharedPointer<T,M>& rhs);
    template<typename T, typename M>
    bool operator==(const SharedPointer<T,M>& lhs, nullptr_t);

    // inequality comparison operators
    template<typename T, typename M>
    bool operator!=(const SharedPointer<T,M>& lhs, const SharedPointer<T,M>& rhs);
    template<typename T, typename M>
    bool operator!=(nullptr_t, const SharedPointer<T,M>& rhs);
    template<typename T, typename M>
    bool operator!=(const SharedPointer<T,M>& lhs, nullptr_t);

    // less than comparison operators
    template<typename T, typename M>
    bool operator<(const SharedPointer<T,M>& lhs, const SharedPointer<T,M>& rhs);
    template<typename T, typename M>
    bool operator<(nullptr_t, const SharedPointer<T,M>& rhs);
    template<typename T, typename M>
    bool operator<(const SharedPointer<T,M>& lhs, nullptr_t);

    // greater than comparison operators
    template<typename T, typename M>
    bool operator>(const SharedPointer<T,M>& lhs, const SharedPointer<T,M>& rhs);
    template<typename T, typename M>
    bool operator>(nullptr_t, const SharedPointer<T,M>& rhs);
    template<typename T, typename M>
    bool operator>(const SharedPointer<T,M>& lhs, nullptr_t);

    // greater than or equal to comparison operators.
    template<typename T, typename M>
    bool operator>=(const SharedPointer<T,M>& lhs, const SharedPointer<T,M>& rhs);
    template<typename T, typename M>
    bool operator>=(nullptr_t, const SharedPointer<T,M>& rhs);
    template<typename T, typename M>
    bool operator>=(const SharedPointer<T,M>& lhs, nullptr_t);

    // less than or equal to comparison operators.
    template<typename T, typename M>
    bool operator<=(const SharedPointer<T,M>& lhs, const SharedPointer<T,M>& rhs);
    template<typename T, typename M>
    bool operator<=(nullptr_t, const SharedPointer<T,M>& rhs);
    template<typename T, typename M>
    bool operator<=(const SharedPointer<T,M>& lhs, nullptr_t);

    // ostream operator
    template<typename charT, typename traits, typename T, typename M>
    std::basic_ostream<charT, traits>& operator<<(std::basic_ostream<charT, traits>& os, const SharedPointer<T, M>& rhs);

} //namespace ldl

#include "shared_pointer.hpp"

#endif //! SHARED_POINTER_H
