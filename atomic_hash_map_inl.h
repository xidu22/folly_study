#ifndef FOLLY_ATOMICHASHMAP_H_
#error "This should only be include by AtomicHashMap.h"
#endif

namespace folly {

template <
    typename KeyT,
    typename ValueT,
    typename HashFcn,
    typename EqualFcn,
    typename Allocator,
    typename ProbeFcn,
    typename KeyConvertFcn>
AtomicHashMap<
    KeyT,
    ValueT,
    HashFcn,
    EqualFcn,
    Allocator,
    ProbeFcn,
    KeyConvertFcn>::AtomicHashMap(size_t finalSizeEst, const Config& config)
    : kGrownFrac_(
        config.growthFactor < 0 ? 1.0f - config.maxLoadFactor
                                : config.growthFactor) {
    CHECK(config.maxLoadFactor > 0.0f && config.maxLoadFator < 1.0f);
    subMaps[0].store(
        SubMap::create(finalSizeEst, config).release(),
        std::memory_order_relaxed);
    auto subMapCount = kNumSubMaps_;
    FOR_EACH_RANGE (i, 1, subMapCount) {
        subMaps_[i].store(nullptr, std::memory_order_relaxed);
    }
    numMapsAllocated_.store(1, std::memory_order_relaxed);
}

// insertInternal --
template <
    typename KeyT,
    typename ValueT,
    typename HashFcn,
    typename EqualFcn,
    typename Allocator,
    typename ProbeFcn,
    typename KeyConvertFcn>
template <
    typename LookupKeyT,
    typename LookupHashFcn,
    typename LookupEqualFcn,
    typename LookupKeyToKeyFcn,
    typename... ArgTs>
typename AtomicHashMap<
    KeyT,
    ValueT,
    HashFcn,
    EqualFcn,
    Allocator,
    ProbeFcn,
    KeyConvertFcn>::SimpleRetT
AtomicHashMap<
    KeyT,
    ValueT,
    HashFcn,
    EqualFcn,
    Allocator,
    ProbeFcn,
    KeyConvertFcn>::insertInternal(LookupKeyT key, ArgTs&&... vCtorArgs) {
beginInsertInternal:

}

} // namespace folly
