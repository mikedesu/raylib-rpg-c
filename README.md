[@evildojo666](https://x.com/evildojo666) presents...

# Project.RPG 

![projectrpg](https://github.com/user-attachments/assets/2b86ab6b-b9c8-4887-8505-d5c9b669cd61)

*with art assets by Krishna Palacio* [https://www.minifantasy.net](https://www.minifantasy.net)

-----

## Building 

### Requirements

- `raylib`
- `gcc` or `clang` (but the `Makefile` assumes `gcc` for now)
- `emsdk`
- `make`
- `liblua5.4-dev`
- A custom-built version of `liblua.a` for the web build


### Instructions

1. Clone the repository
2. Change directory: `cd raylib-rpg-c/src`
3. Run the following command:

```
make
```

#### WebAssembly executable

You will have to edit some of the hardcoded paths in the `emcc` instruction. 
I will provide details for this in the near future.

```
make index.html
```

-----

## Running

### Linux 

```
./game
```

### WebAssembly

```
emrun index.html
```

