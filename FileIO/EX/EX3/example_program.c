#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>
#include <stdint.h>

void print_usage(char *program_name) {
    printf("Usage: %s filename num-bytes [r/w] content\n", program_name);
    printf("  filename: Name of the file to read from or write to\n");
    printf("  num-bytes: Number of bytes to read or write\n");
    printf("  [r/w]: 'r' for read operation, 'w' for write operation\n");
    printf("  content: Content to write (required for write operation)\n");
}

int main(int argc, char *argv[]) {
    // Check argument count
    if (argc != 5) {
        printf("Error: Invalid number of arguments\n");
        print_usage(argv[0]);
        return 1;
    }

    char *filename = argv[1];
    char *endptr;
    errno = 0;  // Reset errno before the conversion
    uintmax_t num_bytes_parsed = strtoumax(argv[2], &endptr, 10);
    
    // Validate num_bytes conversion
    if (errno != 0 || *endptr != '\0' || num_bytes_parsed == 0 || num_bytes_parsed > SIZE_MAX) {
        printf("Error: num-bytes must be a positive number\n");
        return 1;
    }
    size_t num_bytes = (size_t)num_bytes_parsed;
    char operation = argv[3][0];
    char *content = argv[4];

    // Validate operation
    if (operation != 'r' && operation != 'w') {
        printf("Error: Operation must be 'r' or 'w'\n");
        return 1;
    }

    int fd;
    if (operation == 'r') {
        // Open file for reading
        fd = open(filename, O_RDONLY);
        if (fd < 0) {
            printf("Error opening file for reading: %s\n", strerror(errno));
            return 1;
        }

        // Allocate buffer for reading
        char *buffer = (char *)malloc(num_bytes + 1);
        if (buffer == NULL) {
            printf("Error allocating memory\n");
            close(fd);
            return 1;
        }

        // Read from file
        ssize_t bytes_read = read(fd, buffer, num_bytes);
        if (bytes_read < 0) {
            printf("Error reading from file: %s\n", strerror(errno));
            free(buffer);
            close(fd);
            return 1;
        }

        // Null terminate and print the read content
        buffer[bytes_read] = '\0';
        printf("Read %zd bytes: %s\n", bytes_read, buffer);

        free(buffer);
    } else {
        // Open file for writing
        fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd < 0) {
            printf("Error opening file for writing: %s\n", strerror(errno));
            return 1;
        }

        // Write to file
        size_t content_len = strlen(content);
        size_t bytes_to_write = content_len < num_bytes ? content_len : num_bytes;
        ssize_t bytes_written = write(fd, content, bytes_to_write);
        
        if (bytes_written < 0) {
            printf("Error writing to file: %s\n", strerror(errno));
            close(fd);
            return 1;
        }

        printf("Written %zd bytes to %s\n", bytes_written, filename);
    }

    close(fd);
    return 0;
}
