#include "ComponentTable.h"


// Get or create map for a component kind
template <typename Kind>
shared_ptr<unordered_map<EntityID, typename ComponentTraits<Kind>::Type>> ComponentTable::getStore() {
    auto type = type_index(typeid(Kind));
    auto it = stores.find(type);
    if (it == stores.end()) {
        // Create new map for this component kind
        auto store = make_shared<unordered_map<EntityID, typename ComponentTraits<Kind>::Type>>();
        stores[type] = store;
        return store;
    }
    // Cast shared_ptr<void> back to shared_ptr<unordered_map<EntityID, T>>.
    // This is safe because type_index(typeid(Kind)) ensures we retrieve the
    // exact map type created for this Kind (e.g., Name -> string, Hitpoints -> Vector2).
    // static_pointer_cast preserves shared_ptr's memory management.
    return static_pointer_cast<unordered_map<EntityID, typename ComponentTraits<Kind>::Type>>(it->second);
}

// Add a component
template <typename Kind>
void ComponentTable::set(EntityID entity, const typename ComponentTraits<Kind>::Type& component) {
    (*getStore<Kind>())[entity] = component;
}

// Get a component (returns optional)
template <typename Kind>
optional<typename ComponentTraits<Kind>::Type> ComponentTable::get(EntityID entity) {
    auto store = getStore<Kind>();
    auto it = store->find(entity);
    if (it == store->end()) {
        return nullopt;
    }
    return it->second;
}

template <typename Kind>
bool ComponentTable::has(EntityID entity) {
    return getStore<Kind>()->find(entity) != getStore<Kind>()->end();
}

template <typename Kind>
bool ComponentTable::remove(EntityID entity) {
    return getStore<Kind>()->erase(entity) > 0;
}

void ComponentTable::clear() { stores.clear(); }
