#include "future.h"

#include <exception>

namespace ldl {

    //---------------
    template<typename T>
    Future<T>::Future()
        : shared_state_ptr_(0)
    {}

    //---------------
    template<typename T>
    Future<T>::Future(SharedState* shared_state_ptr)
        : shared_state_ptr_(shared_state_ptr)
    {}

    //---------------
    template<typename T>
    void Future<T>::swap(Future& other) {
        std::swap(shared_state_ptr_, other.shared_state_ptr_);
    }

    //---------------
    template<typename T>
    bool Future<T>::valid() const {
        return  (shared_state_ptr_ != 0);
    }

    //---------------
    template<typename T>
    T Future<T>::get()
    {
        wait();
        return shared_state_ptr_->value;
    }

    //---------------
    template<typename T>
    void Future<T>::wait()
    {
        if (!valid()) {
            throw std::runtime_error("future is not valid");
        }
        std::unique_lock<std::mutex> lck(shared_state_ptr_->mutex_ptr_);
        while (!shared_state_ptr_->ready) { //ignore spurious wake-ups
            shared_state_ptr_->cv.wait(lck);
        }
    }

    //---------------
    template<typename T>
    template<typename Clock, typename Duration>
    FutureStatus::type Future<T>::wait_until(const c11::chrono::time_point<Clock, Duration >& abs_time)
    {
        FutureStatus::type retval = FutureStatus::timeout;
        if (!valid()) {
            throw std::runtime_error("future is not valid");
        }
        std::unique_lock<std::mutex> lck(shared_state_ptr_->mutex_ptr_);
        while (!shared_state_ptr_->ready) { //ignore spurious wake-ups
            if (shared_state_ptr_->cv.wait_until(lck, abs_time) == std::cv_status::timeout) {
                break; // timed out, stop waiting
            }
        }
        if (shared_state_ptr_->ready) {
            retval = FutureStatus::ready;
        }
        return retval;
    }

    //---------------
    template<typename T>
    template< typename Rep, typename Period>
    FutureStatus::type Future<T>::wait_for(const c11::chrono::duration<Rep, Period>& rel_time) 
    {
        return wait_until(std::chrono::steady_clock::now() + rel_time);
    }

    //---------------
    template<typename T>
    Promise<T>::Promise()
        : shared_state_ptr_(0)
        , future_exists_(false)
    {}

    //---------------
    template<typename T>
    Promise<T>::Promise(SharedState* shared_state_ptr)
        : shared_state_ptr_(shared_state_ptr)
        , future_exists_(false)
    {
        if (!shared_state_ptr_) {
            throw std::runtime_error("invalid shared_state_ptr");
        }
        shared_state_ptr_->ready = false;
    }

    //---------------
    template<typename T>
    Promise<T>::~Promise()
    {
        if (shared_state_ptr_ ) {
            // Unblock Future if it is waiting.
            shared_state_ptr_->ready = true;
            shared_state_ptr_->cv.notify_one();
        }
    }

    //---------------
    template<typename T>
    void Promise<T>::swap(Promise& other)
    {
        std::swap(shared_state_ptr_, other.shared_state_ptr_);
        std::swap(future_exists_, other.future_exists_);
    }

    //---------------
    template<typename T>
    Future<T> Promise<T>::get_future()
    {
        // if no shared state, or future already exists
        if (!shared_state_ptr_ || future_exists_) {
            throw std::runtime_error("Can't construct Future object");
        }
        future_exists_ = true;
        return Future<T>(shared_state_ptr_);
    }

    //---------------
    template<typename T>
    void Promise<T>::set_value(const T& value) {
        // if no shared state, or value already set
        if (!shared_state_ptr_ || shared_state_ptr_->ready) {
            Throw std::runtime_error("Promise can't set value");
        }
        shared_state_ptr_->value = value;
        shared_state_ptr_->ready = true;
        shared_state_ptr_->cv.notify_one();
    }

} //namespace ldl
