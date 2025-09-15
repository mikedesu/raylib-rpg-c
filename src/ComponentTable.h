#pragma once

#include "ComponentTraits.h"
#include "mprint.h"
#include <memory>
#include <optional>
#include <string>
#include <typeindex>
#include <unordered_map>

using std::make_shared;
using std::nullopt;
using std::optional;
using std::shared_ptr;
using std::static_pointer_cast;
using std::string;
using std::type_index;
using std::unordered_map;

// Component manager
class ComponentTable {
    // Central storage for all component maps, using shared_ptr<void> to store
    // different shared_ptr<unordered_map<EntityID, T>> types
    unordered_map<type_index, shared_ptr<void>> stores;

public:
    // Get or create map for a component kind
    template <typename Kind>
    shared_ptr<unordered_map<EntityID, typename ComponentTraits<Kind>::Type>> getStore();

    // Add a component
    template <typename Kind>
    void set(EntityID entity, const typename ComponentTraits<Kind>::Type& component);

    // Get a component (returns optional)
    template <typename Kind>
    optional<typename ComponentTraits<Kind>::Type> get(EntityID entity);

    // Check if component exists
    template <typename Kind>
    bool has(EntityID entity);

    template <typename Kind>
    bool remove(EntityID entity);

    void clear();
};
