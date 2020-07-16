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
    const float kGrowthFrac_; // How much to grow when we run out of capacity.

    explicit AtomicHashmap(size_t finalSizeEst, const Config& c = Config());

    AtomicHashMap(const AtomicHashMap&) = delete;
    AtomicHashMap& operator=(const AtomicHashMap&) = delete;

    ~AtomicHashMap() {
        const unsigned int numMaps = 
            numMapsAllocated_.load(std::memory_order_relaxed);
        FOR_EACH_RANGE (i, 0, numMaps) {
            SubMap* thisMap = subMaps_[i].load(std::memory_order_relaxed);
            DCHECK(thisMap);
            SubMap::destory(thisMap);
        }
    }

    key_equal key_eq() const {
        return key_equal();
    }
    hasher hash_function() const {
        return hasher();
    }

private:
    static const uint32_t kNumSubMapBits_ = 4;
    static const uint32_t kSecondaryMapBit_ = 1u << 31; 
    static const uint32_t kSubMapIndexShift_ = 32 - kNumSubMapBits_ - 1;
    static const uint32_t kSubMapIndexMask_ = (1 << kSubMapIndexShift_) -1;
    static const uintptr_t kLockedPtr_ = 0x88ULL << 48; // invalid pointer

    struct SimpleRetT {
        uint32_t i;
        size_t j;
        bool success;
        SimpleRetT(uint32_t ii, size_t jj, bool s) : i(ii), j(jj), success(s) {}
        SimpleRetT() = default;
    };

    template<
        typename LookupKeyT = key_type,
        typename LookupHashFcn = hasher,
        typename LookupEqualFcn = key_equal,
        typename LookupKeyToKeyFcn = key_convert,
        typename... ArgTs>
    SimpleRetT insertInternal(LookupKey key, ArgTs&&... value);

    std::atomic<SubMap*> subMaps_[kNumSubMaps_];
    std::atomic<uint32_t> numMapsAllocated_;

    inline bool tryLockMap(unsigned int idx) {
        SubMap* val = nullptr;
        return subMaps_[idx].compare_exchange_strong(
                val, (SubMap*)kLockedPtr_, std::memory_order_acquire);
    }

    static inline uint32_t encodeIndex(uint32_t subMap, uint32_t subMapIdx);

};  // AtomicHashMap

}  // namespace folly

#include <folly/atomic_hash_map_inl.h>
