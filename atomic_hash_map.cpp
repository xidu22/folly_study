// author: xidu22
//
// A concurrent hash map with int32_t or int64_t keys. Supports insert, 
// find(key), findAt(index), erase(key), size, and more. 

#pragma once
#define FOLLY_ATOMICHASHMAP_H_

#include <atomic>
#include <functional>
#include <stdexcept>


namespace folly {

// Throw when insertion fail due to running out of space for submaps.
struct FOLLY_EXPORT AtomicHashMapFullError : std::runtime_error {
    explicit AtomicHashMapFullError()
        : std::runtime_error("AtomicHashMap is full") {}
};

template <
    class KeyT,
    class ValueT,
    class HashFcn,
    class EqualFcn,
    class Allocator,
    class ProbeFcn,
    class KeyConvertFcn>
class AtomicHashMap {
    typedef AtomicHashArray<
        KeyT,
        ValueT,
        HashFcn,
        EqualFcn,
        Allocator,
        ProbeFcn,
        KeyConvertFcn>
        SubMap;

public:
    typedef KeyT key_type;
    typedef ValueT mapped_type;
    typedef std::pair<const KeyT, ValueT> value_type;
    typedef HashFcn hasher;
    typedef EqualFcn key_equal;
    typedef KeyConvertFcn key_convert;
    typedef value_type* pointer;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef std::ptrdiff_t difference_type;
    typedef std::size_t size_type;
    typedef typename SubMap::Config Config;

    template <class ContT, class IterVal, class SubIt>
    struct ahm_iterator;

    typedef ahm_iterator<
        const AtomicHashMap,
        const value_type,
        typename Submap::const_iterator>
        const_iterator;
    typedef ahm_iterator<AtomicHashMap, value_type, typename SubMap::iterator>
        iterator;

public:

};  // AtomicHashMap

}  // namespace folly

#include <folly/AtomicHashMap-inl.h>
