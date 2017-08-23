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
    /// The provided mutex is unlocked, but NOT deleted when the last owner deletes the mangaged object.
    /// The mutex is passed in as an argument to avoid having to allocate one internally.
    template<typename T>
    class SharedPointer {
    public:

        /// Type of the managed object.
        typedef T element_type;

        /// Type of mutex.
        typedef c11::mutex Mutex;

        // type signature of deleter function
        typedef c11::function<void(element_type*)> Deleter;

        //default constructor
        SharedPointer();

        //construct w/ mutex
        SharedPointer(Mutex& mx);

        // Construct from pointer and optional mutex
        template<typename U>
        SharedPointer(U* ptr, Mutex& mx = *static_cast<Mutex*>(nullptr) );

        // construct from null pointer and optional mutex
        SharedPointer(nullptr_t, Mutex& mx = *static_cast<Mutex*>(nullptr) );

        // construct from pointer, deleter, and optional mutex
        template<typename U>
        SharedPointer(U* ptr, Deleter del, Mutex& mx = *static_cast<Mutex*>(nullptr));

        // construct from null pointer, deleter, and optional mutex
        SharedPointer(nullptr_t, Deleter del, Mutex& mx = *static_cast<Mutex*>(nullptr));

        // copy constructor
        SharedPointer(SharedPointer& other);

        // copy constructor from different managed object type
        template<typename U>
        SharedPointer(SharedPointer<U>& other);

        // destructor
        ~SharedPointer();

        // copy assignment operator
        SharedPointer& operator=(SharedPointer& other);

        // copy assignment from different managed object type
        template<typename U>
        SharedPointer& operator=(SharedPointer<U>& other);

        // swap state of two objects.
        void swap(SharedPointer& other);

        // reset state to be as if object were default constructed.
        // releases ownership of any currently owned object.
        void reset();

        // reset state to be as if object were constructed by SharedPointer(mx)
        // releases ownership of any currently owned object.
        void reset(Mutex& mx);

        // reset object and then swap it with a new object constructed by SharedPointer(ptr,mx)
        template<typename U>
        void reset(U* ptr, Mutex& mx = *static_cast<Mutex*>(0) );

        // reset object and then swap it with a new object constructed by SharedPointer(ptr,del,mx)
        template<typename U>
        void reset(U* ptr, Deleter del, Mutex& mx = *static_cast<Mutex*>(0) );

        // return a plain pointer to managed object
        element_type* get() const;

        // dereference managed object
        element_type& operator*() const;

        // dereference managed object for calling a member function.
        element_type* operator->() const;

        // return number of objects sharing the managed object. (0 if this object is empty)
        long int use_count();

        // return true if this object is managing an object and it is the only owner.
        bool unique();

        // return true if this object is managing an object.
        explicit operator bool() const;

    private:

        /// Function class with operator()(T* ptr) member function that calls del( static_cast<U*>(ptr) )
        /// Used to delete an object of type U, that is managed by a SharedPointer<T>.
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

        // Function to make this class usable as an argument to the ListLock constructor.
        // This allows the use of a mutex to be optional.
        // if mutex_ptr_ == 0 , it does nothing, otherise it calls mutex_ptr_.lock()
        void lock();

        // Function to make this class usable as an argument to the ListLock constructor.
        // This allows the use of a mutex to be optional.
        // if mutex_ptr_ == 0 , it does nothing, otherise it calls mutex_ptr_.unlock()
        void unlock();

        //------------

        // pointer to optional mutex (pointer makes it copyable/swapable.)
        Mutex* mutex_ptr_;

        // pointer to previous object in linked list of owners.
        SharedPointer* prev_ptr_;

        // pointer to next object in linked list of owners.
        SharedPointer* next_ptr_;

        // pointer to managed object.
        element_type* obj_ptr_;

        // function object called to delete obj_ptr_
        Deleter deleter_;

    };

    // swap states of two objects.
    template<typename T>
    void swap(SharedPointer<T>& lhs, SharedPointer<T>& rhs);

    // Equality comparison operators
    template<typename T>
    bool operator==(const SharedPointer<T>& lhs, const SharedPointer<T>& rhs);
    template<typename T>
    bool operator==(nullptr_t, const SharedPointer<T>& rhs);
    template<typename T>
    bool operator==(const SharedPointer<T>& lhs, nullptr_t);

    // inequality comparison operators
    template<typename T>
    bool operator!=(const SharedPointer<T>& lhs, const SharedPointer<T>& rhs);
    template<typename T>
    bool operator!=(nullptr_t, const SharedPointer<T>& rhs);
    template<typename T>
    bool operator!=(const SharedPointer<T>& lhs, nullptr_t);

    // less than comparison operators
    template<typename T>
    bool operator<(const SharedPointer<T>& lhs, const SharedPointer<T>& rhs);
    template<typename T>
    bool operator<(nullptr_t, const SharedPointer<T>& rhs);
    template<typename T>
    bool operator<(const SharedPointer<T>& lhs, nullptr_t);

    // greater than comparison operators
    template<typename T>
    bool operator>(const SharedPointer<T>& lhs, const SharedPointer<T>& rhs);
    template<typename T>
    bool operator>(nullptr_t, const SharedPointer<T>& rhs);
    template<typename T>
    bool operator>(const SharedPointer<T>& lhs, nullptr_t);

    // greater than or equal to comparison operators.
    template<typename T>
    bool operator>=(const SharedPointer<T>& lhs, const SharedPointer<T>& rhs);
    template<typename T>
    bool operator>=(nullptr_t, const SharedPointer<T>& rhs);
    template<typename T>
    bool operator>=(const SharedPointer<T>& lhs, nullptr_t);

    // less than or equal to comparison operators.
    template<typename T>
    bool operator<=(const SharedPointer<T>& lhs, const SharedPointer<T>& rhs);
    template<typename T>
    bool operator<=(nullptr_t, const SharedPointer<T>& rhs);
    template<typename T>
    bool operator<=(const SharedPointer<T>& lhs, nullptr_t);

    // ostream operator
    template<typename charT, typename traits, typename T>
    std::basic_ostream<charT, traits>& operator<<(std::basic_ostream<charT, traits>& os, const SharedPointer<T>& rhs);

} //namespace ldl

#include "shared_pointer.hpp"

#endif //! SHARED_POINTER_H
