#pragma once
#ifndef  PROMISE_FUTURE_H_
#define  PROMISE_FUTURE_H_

#include "pooled_new.h"
#include "shared_pointer.h"

#include <mutex>
#include <condition_variable>
#include <chrono>
namespace c11 {
    using namespace std;
}

namespace ldl {

    //-------------
    template<typename T>
    struct FutureState : public PooledNew<UnknownElementSize> {
        // FIXME need to call PooledNew::SetElementSize(sizeof(FutureState<T>)) manually

        // mutex for cv.wait()
        c11::mutex mutex;

        // synchronization condition_variable
        c11::condition_variable cv;

        // flag for detecting spurious cv wake ups.
        bool notified;

        // flag indicating that promise has set value (or is being destroyed).
        bool value_set;

        // shared value
        T value;

        // default constructor
        FutureState();

    };

    //-------------
    // return type for Future::wait_for() and Future::wait_until()
    struct FutureStatus {
        enum type {
            timeout,
            ready,
        };
    };

    //-------------
    /// A class that can block execution until it receives a shared value set by a Promise object.
    template<typename T>
    class Future : public PooledNew<int> {
        // FIXME need to call PooledNew::SetElementSize(sizeof(Future<T>)) manually
    public:

        // construct an empty object (no shared state).
        Future();

        // move constructor 
        Future(Future&);

        // move assignment operator
        Future& operator=(Future&);

        // allows us to move-assign and move-construct Future objects with a pre-C11 compiler.
        void swap(Future& other);

        // Return true if this object has a shared state.
        bool valid() const;

        // call wait() and then return the value set by the Promise that shares state with object.
        // If the Promise object is destroyed before setting value, behavior is undefined.
        // Calling when this object is invalid results in undefined behavior.
        T get();

        // Wait until the promise object that shares state with this object sets value and notifies the shared condition variable.
        // Calling when this object is invalid results in undefined behavior.
        void wait();

        // Wait until the promise that shares state with this object sets value and notifies the shared condition variable,
        // or until the specified time_point is reached.
        // Calling when this object is invalid results in undefined behavior.
        template<typename Clock, typename Duration>
        FutureStatus::type wait_until(const c11::chrono::time_point<Clock, Duration >& abs_time);

        // Wait until the promise that shares state with this object sets value and notifies the shared condition variable,
        // or until the specified duration elapses.
        // Calling when this object is invalid results in undefined behavior.
        template< typename Rep, typename Period>
        FutureStatus::type wait_for(const c11::chrono::duration<Rep, Period>& rel_time);

    private:

        // only Promise objects can construct valid Future objects.
        template<typename U>
        friend class Promise;

        // Construct an object with the specified shared state.
        Future(SharedPointer<FutureState<T>>& state_ptr);

        //---

        SharedPointer<FutureState<T>> state_ptr_;

    };

    //-------------
    template<typename T>
    class Promise : public PooledNew<int> {
        // FIXME need to call PooledNew::SetElementSize(sizeof(Promie<T>)) manually

    public:

        // default constructor
        Promise();

        // destroy a promise object
        ~Promise();

        // swap states of two objects.
        // allows us to move-assign and move-construct Future objects with a pre-C11 compiler.
        void swap(Promise& other);

        // construct a Future object with the same shared state as this object.
        Future<T> get_future();

        // set the value of the shared state and notify the shared condition variable.
        void set_value(const T& value);

    private:
        // no copying
        Promise(const Promise&); // = delete;
        Promise& operator=(const Promise&); // = delete;

        //---

        SharedPointer<FutureState<T>> state_ptr_;

        // true if a Future object with the same shared state as this object has been constructed.
        bool future_constructed_;
    };

} //namespace ldl

#include "future.hpp"

#endif //!PROMISE_FUTURE_H_
