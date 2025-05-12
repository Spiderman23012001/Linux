# Linux File System

## 1. File Management
- Dup() and dup2()
- File Descriptor table 
- Open file table
- I-node 

## 2. File Permission
- Check permission: `ls -l`
- File type – User – Group - Other
- Edit permission: chmod 0777 test.txt 

## 3. Opreate file
- `int open(const char *pathname, int flags, mode_t mode);`
- `ssize_t read(int fd, void *buffer, size_t count);
`
- `ssize_t write(int fd, void *buffer, size_t count);
`
- `off_t  lseek(int fd, off_t offset, int whence);
`
- `int  close(int fd);
`

## 4. File Clock
- To manage many processes access a file to read/write
- how to work:
    - Step 1: Write the lock status to the file's I-node. 
    - Step 2: If successful, read and write the file. If unsuccessful, it means the file is being used by another process. 
    - Step 3: After reading/writing is complete, remove the lock status from the file's I-node.
- 2 types:
    - Flock(): Lock entire file
    `int flock(int fd, int operation);`
    - Fcntl(): Lock each area of ​​file `fcntl(fd, cmd, &flockstr)`


## Command line
- Check permission: `ls -l`