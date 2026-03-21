# THIS REPO WILL NO LONGER BE MAINTAINED AS OF 2026-21-03

The most recent release can always be played online at [https://evildojo.itch.io/projectrpg](https://evildojo.itch.io/projectrpg)

Building **Project.RPG** in public is a fun process, but I am moving future development into a private repository and changing the LICENSE.

People can continue to clone this repo and use the code for learning purposes, but active development moving forward will be private for now.

Individual game components may still be released, but under a different LICENSE.

Live development streams will still be present at: [https://www.youtube.com/@evildojo666](https://www.youtube.com/@evildojo666)


-----

[@evildojo666](https://x.com/evildojo666) presents...

# Project.RPG 

The most recent release can be played online at [https://evildojo.itch.io/projectrpg](https://evildojo.itch.io/projectrpg)

![projectrpg](https://github.com/user-attachments/assets/2b86ab6b-b9c8-4887-8505-d5c9b669cd61)

*with art assets by Krishna Palacio* [https://www.minifantasy.net](https://www.minifantasy.net)

*sound effects by Leoh Paz* [https://leohpaz.itch.io/](https://leohpaz.itch.io/)

-----

## Building 

### Requirements

- **Raylib** (development libraries)
- **g++** or **clang++** (C++17 compatible)
- **emcc** (for WebAssembly builds)
- **Make**

### Build Instructions

1. Clone the repository
2. Change to source directory:
```bash
cd src
```

#### Standard Build
```bash
make clean && make
```

#### Debug Build
```bash
make clean && CXXFLAGS="-DDEBUG_ASSERT=1 -DNPCS_ALL_AT_ONCE -DDEBUG=1 -DMASTER_VOLUME=0.25f" make game
```

#### WebAssembly Build
```bash
make clean && CXXFLAGS="-DDEBUG_ASSERT=1 -DNPCS_ALL_AT_ONCE -DDEBUG=1 -DMASTER_VOLUME=0.25f" make index.html
```

-----

## Running

### Native Executable
```bash
./game
```

### WebAssembly Version
```bash
emrun index.html
```

### Running Tests
```bash
make tests
./tests
```

-----

## Development Notes

- The project was originally developed in C, but has been ported to C++
- The Makefile supports both native and WebAssembly builds
- Debug builds enable additional logging and debugging features
- Unit tests can be built and run using the `test` target
- The game graphic and audio assets are not included in the repository
  - The graphics are licensed such that I cannot distribute them
  - The sound effects are licensed such that I cannot distribute them  
