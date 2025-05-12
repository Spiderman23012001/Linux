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
| Size | Using static lib takes more memory than shared lib. | Using shared lib takes less memory since there is only one copy in memory.|
| External File changes | The executable needs to be recompiled whenever there is a change in the static lib. | For shared libs, there is no need to recompile the executable.|
| Time | Takes longer to execute. | Using shared lib takes less time to execute because the library is already in memory. |


