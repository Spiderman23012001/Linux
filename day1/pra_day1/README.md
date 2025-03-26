step 1: Create ex1.c (src) and ex1.h (inc)
step 2: Create object file (usse -fpic for shared lib) (obj)
step 3: create .a for static lib or .so for shared lib (lib)
step 4: linking main with lib (bin)
step 5: run
    - static lib: ./bin/./bin/ex1_static_linked
    - shared lib: LD_LIBRARY_PATH=./lib/shared  ./bin/ex1_shared_linked