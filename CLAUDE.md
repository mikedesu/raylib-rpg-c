# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview
- C++ RPG game built with raylib
- Recent development focused on NPC mechanics and performance optimization
- Uses Minifantasy asset packs for game content

## Key Files/Directories
- src/main.cpp: Main game entry point
- src/img/: Contains all game image assets
- src/audio/: Contains audio assets
- Note: src/minifantasy/ is a local asset library, not part of active project
- Recent work focused on dead NPC handling (dead_npc_cache)

## Development Notes
- Recent commits show work on:
  - Dead body visibility and caching
  - NPC killing mechanics
  - Performance optimization

## Testing
- Unit tests exist for dead_npc_cache methods (test: add unit tests commits)
- Testing approach appears to be direct unit testing rather than a formal test framework

## Build System
No standard build files found. Likely needs manual compilation with:

```
make clean
CXXFLAGS="-DDEBUG_ASSERT=1 -DNPCS_ALL_AT_ONCE -DDEBUG=1 -DMASTER_VOLUME=1.0f -DMUSIC_OFF " make game
```
