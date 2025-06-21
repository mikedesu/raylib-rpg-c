[@evildojo666](https://x.com/evildojo666) presents...

# Project.RPG 

![projectrpg](https://github.com/user-attachments/assets/2b86ab6b-b9c8-4887-8505-d5c9b669cd61)

*with art assets by Krishna Palacio* [https://www.minifantasy.net](https://www.minifantasy.net)

-----

## Building 

### Requirements

- **Raylib** (development libraries)
- **GCC** or **Clang** (C++17 compatible)
- **Emscripten** (for WebAssembly builds)
- **Make**

### Build Instructions

1. Clone the repository
2. Change to source directory:
```bash
cd src
```

#### Standard Build
```bash
make
```

#### Debug Build
```bash
CFLAGS="-DDEBUG=1" make
```

#### WebAssembly Build
```bash
make index.html
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
make test
./test
```

-----

## Development Notes

- The project is written in C++ (C++17 standard)
- The Makefile supports both native and WebAssembly builds
- Debug builds enable additional logging and debugging features
- Unit tests can be built and run using the `test` target

