#include "future.h"

#include <exception>
#include <algorithm> //swap
#include <mutex> //unique_lock

namespace ldl {

    //---------------
    template<typename T>
    Future<T>::Future()
        : state_ptr_(0)
    {}

    //---------------
    template<typename T>
    Future<T>::Future(FutureState<T>* state_ptr)
        : state_ptr_(state_ptr)
    {}

    //---------------
    template<typename T>
    void Future<T>::swap(Future& other) {
        if (this != &other) {
            std::swap(state_ptr_, other.state_ptr_);
        }
    }

    //---------------
    template<typename T>
    bool Future<T>::valid() const {
        return  (state_ptr_ != 0);
    }

    //---------------
    template<typename T>
    T Future<T>::get()
    {
        wait();
        return state_ptr_->value;
    }

    //---------------
    template<typename T>
    void Future<T>::wait()
    {
        if (!valid()) {
            throw std::runtime_error("future is not valid");
        }
        c11::unique_lock<c11::mutex> lock(state_ptr_->mutex);
        while (!state_ptr_->notified) { //ignore spurious wake-ups
            state_ptr_->cv.wait(lock);
        }
    }

    //---------------
    template<typename T>
    template<typename Clock, typename Duration>
    FutureStatus::type Future<T>::wait_until(const c11::chrono::time_point<Clock, Duration >& abs_time)
    {
        FutureStatus::type retval = FutureStatus::ready;
        if (!valid()) {
            throw std::runtime_error("future is not valid");
        }
        c11::unique_lock<c11::mutex> lock(state_ptr_->mutex);
        while (!state_ptr_->notified) { //ignore spurious wake-ups
            if (state_ptr_->cv.wait_until(lock, abs_time) == c11::cv_status::timeout) {
                retval = FutureStatus::timeout;
                break; //stop waiting
            }
        }
        return retval;
    }

    //---------------
    template<typename T>
    template< typename Rep, typename Period>
    FutureStatus::type Future<T>::wait_for(const c11::chrono::duration<Rep, Period>& rel_time) 
    {
        return wait_until(c11::chrono::steady_clock::now() + rel_time);
    }

    //---------------
    template<typename T>
    FutureState<T>* Future<T>::GetFutureState() const
    {
        return state_ptr_;
    }

    //---------------
    template<typename T>
    Promise<T>::Promise()
        : state_ptr_(0)
        , future_constructed_(false)
    {}

    //---------------
    template<typename T>
    Promise<T>::Promise(FutureState<T>* state_ptr)
        : state_ptr_(state_ptr)
        , future_constructed_(false)
    {
        if (!state_ptr_) {
            throw std::runtime_error("invalid state_ptr");
        }
        state_ptr_->value_set = false;
        state_ptr_->notified = false;
    }

    //---------------
    template<typename T>
    Promise<T>::~Promise()
    {
        if (state_ptr_ ) {
            // notify cv in case Future object is waiting.
            state_ptr_->notified = true;
            state_ptr_->cv.notify_one();
        }
    }

    //---------------
    template<typename T>
    void Promise<T>::swap(Promise& other)
    {
        if (this != &other) {
            std::swap(state_ptr_, other.state_ptr_);
            std::swap(future_constructed_, other.future_constructed_);
        }
    }

    //---------------
    template<typename T>
    Future<T> Promise<T>::get_future()
    {
        // if no shared state, or future already constructed
        if (!state_ptr_ || future_constructed_) {
            throw std::runtime_error("Can't construct Future object");
        }
        future_constructed_ = true;
        return Future<T>(state_ptr_);
    }

    //---------------
    template<typename T>
    void Promise<T>::set_value(const T& value) {
        // if no shared state, or value already set
        if (!state_ptr_ || state_ptr_->value_set) {
            throw std::runtime_error("Promise can't set value");
        }
        state_ptr_->value = value;
        state_ptr_->value_set = true;
        state_ptr_->notified = true;
        state_ptr_->cv.notify_one();
    }

} //namespace ldl
