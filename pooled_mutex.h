#pragma once
#ifndef LDL_POOLED_MUTEX_H_
#define LDL_POOLED_MUTEX_H_

#include "pooled_new.h"
#include "linkable.h"

#include <mutex>
namespace c11 {
    using namespace std;
}

namespace ldl {

    // add Pool allocators to the standard mutex type.
    // default constructors, etc. are all good
    class PooledMutex : public c11::mutex, public Linkable, public PooledNew<PooledMutex> {};

} //namespace ldl

#endif //! LDL_POOLED_MUTEX_
