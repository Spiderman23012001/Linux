# Makefile 
- Makefile is a script inside which contains:
    - Struct of project (file, dependency)
    - Command Line used to create-destroy files
    - Understand target, 

# C program compilation process

# Static Lib and Shared Lib
| Properties | Staic Library | Shared Library |
|------------|---------------|----------------|
| Linking time | All modules in the library will be copied into the executable file at compile time. When the program is loaded into memory, the OS only places a single executable file including the source code and the linked library (Static linking).| Meanwhile, shared lib is used during linking when both the executable and the library files are loaded into memory (runtime). A shared lib can be used by multiple programs. (Dynamic linking).|
