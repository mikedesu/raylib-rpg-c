/** @file ComponentTable.h
 *  @brief Type-erased storage table for per-entity component maps.
 */

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

/**
 * @brief Runtime component storage for the game's entity-component data.
 *
 * Stores one `unordered_map<entityid, T>` per component kind and indexes those
 * maps by the component tag type declared through `ComponentTraits`.
 */
class ComponentTable
{
    /// @brief Type-indexed owning store for all per-component entity maps.
    unordered_map<type_index, shared_ptr<void>> stores;

public:
    /**
     * @brief Return the backing entity map for a component kind, creating it if needed.
     *
     * @tparam Kind Component tag type resolved through `ComponentTraits`.
     * @return Shared pointer to the typed entity-component map for `Kind`.
     * @warning Calling this for a previously unused component kind creates an empty store.
     */
    template <typename Kind>
    shared_ptr<unordered_map<entityid, typename ComponentTraits<Kind>::Type>> getStore()
    {
        auto type = type_index(typeid(Kind));
        auto it = stores.find(type);
        if (it == stores.end())
        {
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
    /**
     * @brief Add or replace a component value for an entity.
     *
     * @tparam Kind Component tag type resolved through `ComponentTraits`.
     * @param entity Entity receiving the component value.
     * @param component Component value to store.
     */
    template <typename Kind>
    void set(entityid entity, const typename ComponentTraits<Kind>::Type& component)
    {
        (*getStore<Kind>())[entity] = component;
    }
    /**
     * @brief Return a component value for an entity, if present.
     *
     * @tparam Kind Component tag type resolved through `ComponentTraits`.
     * @param entity Entity to query.
     * @return Stored component value, or `nullopt` when absent.
     */
    template <typename Kind>
    optional<typename ComponentTraits<Kind>::Type> get(entityid entity)
    {
        auto store = getStore<Kind>();
        auto it = store->find(entity);
        if (it == store->end())
        {
            return nullopt;
        }
        return it->second;
    }
    /**
     * @brief Return whether an entity currently has a component of the requested kind.
     *
     * @tparam Kind Component tag type resolved through `ComponentTraits`.
     * @param entity Entity to query.
     * @return `true` when the component exists.
     * @warning Calling this for a previously unused component kind creates an empty store.
     */
    template <typename Kind>
    bool has(entityid entity)
    {
        return getStore<Kind>()->find(entity) != getStore<Kind>()->end();
    }
    /**
     * @brief Remove a component of the requested kind from an entity.
     *
     * @tparam Kind Component tag type resolved through `ComponentTraits`.
     * @param entity Entity whose component should be removed.
     * @return `true` when a stored component was removed.
     * @warning Calling this for a previously unused component kind creates an empty store.
     */
    template <typename Kind>
    bool remove(entityid entity)
    {
        return getStore<Kind>()->erase(entity) > 0;
    }
    /** @brief Remove all component stores and all entity-component data. */
    void clear()
    {
        stores.clear();
    }
};
