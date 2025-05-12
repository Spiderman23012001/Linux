#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

int main() {
    int fd;
    
    // Open file with O_APPEND flag
    fd = open("test.txt", O_WRONLY | O_APPEND | O_CREAT, 0644);
    if (fd < 0) {
        printf("Error opening file: %s\n", strerror(errno));
        return 1;
    }

    // Write initial data
    const char *initial_data = "Initial data\n";
    if (write(fd, initial_data, strlen(initial_data)) < 0) {
        printf("Error writing initial data: %s\n", strerror(errno));
        close(fd);
        return 1;
    }

    // Try to seek to beginning of file
    if (lseek(fd, 0, SEEK_SET) < 0) {
        printf("Error seeking: %s\n", strerror(errno));
        close(fd);
        return 1;
    }

    // Try to write data after seeking to beginning
    const char *new_data = "New data after seek\n";
    if (write(fd, new_data, strlen(new_data)) < 0) {
        printf("Error writing new data: %s\n", strerror(errno));
        close(fd);
        return 1;
    }

    // Close the file
    close(fd);
    
    printf("Program completed. Check test.txt to see the results.\n");
    printf("Note: Due to O_APPEND flag, data was written at the end despite seeking to beginning.\n");

    return 0;
}
