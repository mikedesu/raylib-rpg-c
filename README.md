[@evildojo666](https://x.com/evildojo666) presents...

# Project.RPG 

*screenshots coming soon*

-----

## Building 

### Requirements

- `raylib`
- `gcc` or `clang` (but the `Makefile` assumes `gcc` for now)
- `emsdk`
- `make`

### Instructions

1. Clone the repository
2. Change directory to the repository
3. Run the following command:

For Linux executable:

```
make
```

For WebAssembly executable, you will have to edit some of the hardcoded paths in the `emcc` instruction. I will make this easier in the future.

```
make index.html
```

