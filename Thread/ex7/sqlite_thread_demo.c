#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sqlite3.h>
#include <time.h>

// Global variables
sqlite3 *db;
pthread_mutex_t db_mutex = PTHREAD_MUTEX_INITIALIZER;
int keep_running = 1;

// Function to initialize the database
int init_database() {
    int rc;
    char *err_msg = NULL;
    
    // Open database
    rc = sqlite3_open("hello_world.db", &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }
    
    // Create table if not exists
    const char *sql = "CREATE TABLE IF NOT EXISTS messages ("
                      "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                      "timestamp TEXT,"
                      "message TEXT);";
    
    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return 1;
    }
    
    printf("Database initialized successfully\n");
    return 0;
}

// Function to write data to the database
int write_message(const char *message) {
    int rc;
    char *err_msg = NULL;
    char sql[256];
    char timestamp[64];
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", t);
    
    sprintf(sql, "INSERT INTO messages (timestamp, message) "
                 "VALUES ('%s', '%s');", 
                 timestamp, message);
    
    pthread_mutex_lock(&db_mutex);
    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    pthread_mutex_unlock(&db_mutex);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return 1;
    }
    
    printf("Message written: \"%s\" at %s\n", message, timestamp);
    return 0;
}

// Callback function for reading data
static int callback(void *data, int argc, char **argv, char **azColName) {
    (void)data; // Mark parameter as unused to avoid compiler warning
    
    printf("Message from database: ");
    for (int i = 0; i < argc; i++) {
        printf("%s = %s | ", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

// Function to read data from the database
void read_messages() {
    int rc;
    char *err_msg = NULL;
    const char *sql = "SELECT * FROM messages ORDER BY id DESC LIMIT 5;";
    
    pthread_mutex_lock(&db_mutex);
    rc = sqlite3_exec(db, sql, callback, 0, &err_msg);
    pthread_mutex_unlock(&db_mutex);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
    }
}

// Thread function for reading data
void *reader_thread(void *arg) {
    (void)arg; // Mark parameter as unused to avoid compiler warning
    
    printf("Reader thread started\n");
    
    while (keep_running) {
        printf("\n--- Reading latest messages ---\n");
        read_messages();
        sleep(3); // Read every 3 seconds
    }
    
    printf("Reader thread exiting\n");
    return NULL;
}

// Main function
int main() {
    pthread_t reader;
    
    // Initialize database
    if (init_database() != 0) {
        return 1;
    }
    
    // Create reader thread
    if (pthread_create(&reader, NULL, reader_thread, NULL) != 0) {
        fprintf(stderr, "Error creating reader thread\n");
        sqlite3_close(db);
        return 1;
    }
    
    printf("Main thread started - Writing messages\n");
    
    // Write "Hello World!" message to the database
    write_message("Hello World!");
    
    // Wait for a few seconds to let the reader thread read the message
    sleep(5);
    
    // Write another message
    write_message("Hello from SQLite and Threads!");
    
    // Wait for a few more seconds
    sleep(5);
    
    // Signal thread to exit and wait for it
    keep_running = 0;
    pthread_join(reader, NULL);
    
    // Clean up
    pthread_mutex_destroy(&db_mutex);
    sqlite3_close(db);
    
    printf("Program completed successfully\n");
    return 0;
}
