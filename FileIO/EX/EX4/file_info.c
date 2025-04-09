#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

int main() {
    const char *filename = "example_file.txt";
    const char *data = "This is a sample data written to the file.";

    // Create and write to the file
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Error opening file for writing");
        return 1;
    }
    fprintf(file, "%s", data);
    fclose(file);

    // Get file information
    struct stat file_info;
    if (stat(filename, &file_info) != 0) {
        perror("Error getting file information");
        return 1;
    }

    // Display file information
    printf("File Type: ");
    if (S_ISREG(file_info.st_mode)) {
        printf("Regular file\n");
    } else if (S_ISDIR(file_info.st_mode)) {
        printf("Directory\n");
    } else {
        printf("Other\n");
    }
    printf("File Name: %s\n", filename);
    printf("Last Modified Time: %s", ctime(&file_info.st_mtime));
    printf("File Size: %ld bytes\n", file_info.st_size);

    return 0;
}
