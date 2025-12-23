#pragma once

#include "ComponentTraits.h"
#include "entityid.h"
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
    shared_ptr<unordered_map<entityid, typename ComponentTraits<Kind>::Type>> getStore() {
        auto type = type_index(typeid(Kind));
        auto it = stores.find(type);
        if (it == stores.end()) {
            // Create new map for this component kind
            auto store = make_shared<unordered_map<entityid, typename ComponentTraits<Kind>::Type>>();
            stores[type] = store;
            return store;
        }
        // Cast shared_ptr<void> back to shared_ptr<unordered_map<entityid, T>>.
        // This is safe because type_index(typeid(Kind)) ensures we retrieve the
        // exact map type created for this Kind (e.g., Name -> string, Hitpoints -> Vector2).
        // static_pointer_cast preserves shared_ptr's memory management.
        return static_pointer_cast<unordered_map<entityid, typename ComponentTraits<Kind>::Type>>(it->second);
    }
    // Add a component
    template <typename Kind>
    void set(entityid entity, const typename ComponentTraits<Kind>::Type& component) {
        (*getStore<Kind>())[entity] = component;
    }
    // Get a component (returns optional)
    template <typename Kind>
    optional<typename ComponentTraits<Kind>::Type> get(entityid entity) {
        auto store = getStore<Kind>();
        auto it = store->find(entity);
        if (it == store->end()) {
            return nullopt;
        }
        return it->second;
    }
    // Check if component exists
    template <typename Kind>
    bool has(entityid entity) {
        return getStore<Kind>()->find(entity) != getStore<Kind>()->end();
    }
    // Remove a component (returns if removal succeeded)
    template <typename Kind>
    bool remove(entityid entity) {
        return getStore<Kind>()->erase(entity) > 0;
    }
    // Empty the table
    void clear() {
        stores.clear();
    }
};
