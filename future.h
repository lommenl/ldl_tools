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
    // return type for Future::wait_for() and Future::wait_until()
    struct FutureStatus {
        enum type {
            timeout,
            ready,
        };
    };

    //-------------
    struct FutureState : public PooledNew<FutureState> {
        c11::mutex mutex;
        c11::condition_variable cv;
        bool promise_error;
        void* value_ptr;
        FutureState();
    };

    //-------------
    class FutureBase : public PooledNew<FutureBase> {
    protected:
        FutureBase();
        // shared pointer to FutureState shared with promise that created this
        SharedPointer<FutureState> state_ptr_;
    };

    //-------------
    /// A class that can block execution until it receives a shared value set by a Promise object.
    template<typename T>
    class Future : public FutureBase {
    public:
        // Default constructor (no shared state).
        Future();

        // move constructor 
        Future(Future&);

        // Destructor
        ~Future();

        // Move assignment operator
        Future& operator=(Future&);

        // swap object states
        void swap(Future& other);

        // reset object to a default constructed state
        void reset();

        // Return true if this object has a shared state.
        bool valid() const;

        // call wait() and then return the value set by the Promise that shares state with object.
        // If the Promise object is destroyed before setting value, behavior is undefined.
        // Calling when this object is invalid results in undefined behavior.
        T get();

        // Wait until the promise object that shares state with this object sets value
        // and notifies the shared condition variable.
        // Calling when this object is invalid results in undefined behavior.
        void wait();

        // Wait until the promise that shares state with this object sets value and notifies
        // the shared condition variable,
        // or until the specified time_point is reached.
        // Calling when this object is invalid results in undefined behavior.
        template<typename Clock, typename Duration>
        FutureStatus::type wait_until(const c11::chrono::time_point<Clock, Duration >& abs_time);

        // Wait until the promise that shares state with this object sets value and notifies
        // the shared condition variable,
        // or until the specified duration elapses.
        // Calling when this object is invalid results in undefined behavior.
        template< typename Rep, typename Period>
        FutureStatus::type wait_for(const c11::chrono::duration<Rep, Period>& rel_time);

    private:

        // only Promise objects can construct valid Future objects.
        template<typename U>
        friend class Promise;

        // Construct a Future with the specified shared state
        Future(SharedPointer<FutureState>& state_ptr);

    };

    //-------------
    class PromiseBase : public PooledNew<PromiseBase> {
    protected:
        PromiseBase();
       
        bool future_constructed_;

        SharedPointer<FutureState> state_ptr_;

    };

    //-------------
    template<typename T>
    class Promise : public PromiseBase {
    public:

        // default constructor
        Promise();

        // move constructor
        Promise(Promise&);

        // destroy a promise object
        ~Promise();

        // move assignment
        Promise& operator=(Promise&);

        // swap states of two objects.
        // allows us to move-assign and move-construct Future objects with a pre-C11 compiler.
        void swap(Promise& other);

        // reset object to a default constructed state
        void reset();

        // construct a Future object with the same shared state as this object.
        Future<T> get_future();

        // set the value of the shared state and notify the shared condition variable.
        void set_value(const T& value);

    }; //promise<T>

} //namespace ldl

#include "future.hpp"

#endif //!PROMISE_FUTURE_H_
