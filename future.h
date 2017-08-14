#pragma once
#ifndef  PROMISE_FUTURE_H_
#define  PROMISE_FUTURE_H_

#include <mutex>
#include <condition_variable>
#include <chrono>
namespace c11 {
    using namespace std;
}

namespace ldl {

    //-------------
    // Class that defines the state shared between a Promise object and a Future object.
    // These objects can be statically allocated and re-used.
    template<typename T>
    struct SharedState {

        // synchronization condition_variable
        c11::condition_variable cv;

        // mutex for cv.wait()
        c11::mutex; mutex;

        // flag indicating that promise set value (as opposed to being destroyed)
        bool value_set;

        // shared value
        T value;
    };

    //-------------
    struct FutureStatus {
        enum type {
            timeout,
            ready,
        };
    };

    //-------------
    /// A class that will receive a shared value from a promise object.
    template<typename T>
    class Future {
    public:

        // construct an empty future object.
        Future();

        // swap states of this with other.
        void swap(Future& other);

        // Return true if this future has a shared state.
        bool valid() const;

        // call wait() and then return the value set by the Promise object.
        // If the Promise this object is sharing state with is destroyed before setting value,
        // behavior is undefined.
        T get();

        // Wait until the promise object unlocks the shared mutex.
        // Calling with an invalid future results in undefined behavior.
        void wait();

        // Wait until the promise object unlocks the shared mutex or until the specified time_point.
        // Calling with an invalid future results in undefined behavior.
        template<typename Clock, typename Duration>
        FutureStatus::type wait_until(const c11::chrono::time_point<Clock, Duration >& abs_time);

        // wait until the promise object unlocks the shared mutex or the specified duration elapses.
        // Calling with an invalid future results in undefined behavior.
        template< typename Rep, typename Period>
        FutureStatus::type wait_for(const c11::chrono::duration<Rep, Period>& rel_time);

    private:

        // no copying
        Future(const Future&); // = delete;
        Future& operator=(const Future&); // = delete;

        // only Promise objects can construct valid Future objects.
        friend class Promise<T>;

        // Construct an object that uses the specified shared state.
        Future(SharedState* shred_state_ptr);

        //---

        SharedState* shred_state_ptr_;

    };



    //-------------
    template<typename T>
    class Promise {

    public:

        // default consructor
        Promise();

        // construct a Promise that uses the specified SharedState object.
        Promise(SharedState* shared_state_ptr);

        // destroy a promise object
        ~Promise();

        // swap states of two objects.
        void swap(Promise& other);

        // construct a Future object that shares state with this promise.
        Future<T> get_future();

        // set the value of the shared state.
        void set_value(const T& value);

    private:
        // no copying
        Promise(const Promise&); // = delete;
        Promise& operator=(const Promise&); // = delete;

        //---

        SharedState* shared_state_ptr_;

        bool future_exists_;
    };

} //namespace ldl

#endif //!PROMISE_FUTURE_H_
