# README

## Build and Run Instructions

### Step 1: Create Source Files
- Create `ex1.c` in the `src/` directory.
- Create `ex1.h` in the `inc/` directory.

### Step 2: Compile to Object File
- Compile source files to object files.
- Use `-fPIC` flag if creating a shared library.
- Output files are stored in the `obj/` directory.

### Step 3: Create Library
- **Static Library:** Create `.a` file.
- **Shared Library:** Create `.so` file.
- Output files are stored in the `lib/` directory.

### Step 4: Link with Main Program
- Link the main executable with the created library.
- Output binaries are stored in the `bin/` directory.

### Step 5: Run the Program
#### Static Library:
```sh
./bin/ex1_static_linked
```
#### Shared Library:
```sh
LD_LIBRARY_PATH=./lib/shared ./bin/ex1_shared_linked
```

