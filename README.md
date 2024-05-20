# a3clib
This repository contains my own implementation of the c standard library.

## Quickstart

### Building
```sh
git clone git@github.com:WhisperinCheetah/a3clib.git
cd a3clib/lib
make all
```

### Compiling
```sh
gcc $(CFLAGS) -I/path/to/lib -L../path/to/lib/output/ <output name & file you're compiling> -la3clib  
```

## Currently implemented
### Algorithms
- Mergesort (includes integer & reverse integer comparison functions)
- Bubblesort
- Double bubble sort (Performs the same swaps when sorting an array on another array)

### Strings
- atoi

### Memory
- memcpy
- memcmp
- malloc
- free

### Datastructures
- dynamic array
- hashmap (String -> Int)

## Tests
The current test suite is everything except thorough or even good. It performs some basic tests for all the functions but there are definitely still bugs.
