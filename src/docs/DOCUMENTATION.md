# Documentation Standard

This project uses **Doxygen** with **Javadoc-style C/C++ comments** to generate HTML API documentation.

## Goals

- Make headers readable to a human without hunting through implementation details.
- Make generated HTML documentation useful as a navigation surface across the codebase.
- Keep new APIs documented as they are introduced instead of trying to backfill everything later.

## Generation

Generate docs with:

```sh
make docs
```

Clean generated docs with:

```sh
make docs-clean
```

The generated HTML output is written to:

```text
docs/api/html
```

Open the main page at:

```text
docs/api/html/index.html
```

## Comment Style

Prefer documenting declarations in header files.

Use Javadoc-style blocks:

```cpp
/**
 * @brief Create a prop entity at a dungeon location.
 *
 * Adds the prop to the target floor tile cache and initializes its components.
 *
 * @param type Logical prop type used for sprite selection and behavior.
 * @param loc Dungeon-space location, including floor in `z`.
 * @param initFun Additional component initialization callback.
 * @return The created entity id, or `ENTITYID_INVALID` on failure.
 */
entityid create_prop_at_with(proptype_t type, vec3 loc, with_fun initFun);
```

Single-line comments are also acceptable for short declarations:

```cpp
/// @brief Returns the current floor index.
int get_floor();
```

## Tags To Use

- `@brief`: required for public-facing declarations.
- `@param`: required for non-obvious parameters.
- `@return`: required for non-`void` functions unless the result is trivial.
- `@note`: use for important behavior details or assumptions.
- `@warning`: use for sharp edges, ownership rules, lifetime rules, or state coupling.
- `@see`: use when a declaration is closely related to another API.
- `@todo`: acceptable for known follow-up work, but do not use it as a substitute for real docs.

## What To Document

Prioritize these first:

1. Core API headers such as `gamestate.h`, `dungeon_floor.h`, `dungeon_tile.h`, and `libdraw.h`.
2. Public helper types and enums that define gameplay or renderer behavior.
3. Functions whose names do not fully explain ownership, side effects, or failure behavior.

At a minimum, new code should document:

- classes and structs
- enums and typedef-style public types
- non-trivial constants/macros
- public methods and functions
- functions that mutate game state, rendering state, or entity placement

## Style Rules

- Describe behavior, not implementation trivia.
- Keep the first sentence short and high-signal; Doxygen uses it as summary text.
- Document the declaration once instead of duplicating the same block on both declaration and definition.
- Use present tense and direct wording.
- Mention side effects and failure conditions explicitly.
- Mention ownership or state assumptions when they matter.

## Initial Adoption Plan

- Start with key headers and stabilizing surfaces first.
- Improve coverage incrementally as files are touched for normal feature work.
- Prefer adding docs while refactoring rather than creating a separate massive documentation-only pass.
