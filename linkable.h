#pragma once
#ifndef LDL_LINKABLE_H_
#define LDL_LINKABLE_H_

namespace ldl {

    class Linkable {
    protected:
        template<typename T> friend class LinkedList;
        Linkable* next_linkable_;
        Linkable() : next_linkable_(0) {}
    };

} //namespace ldl

#endif //! LDL_LINKABLE_H_
