#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "database.h"  // entry point, connects to functions in database.h and database.c
#define PATH_LENGTH 512

// Helper: read a whole line safely
static int read_line(char *buf, size_t n){
    if (!fgets(buf, (int)n, stdin)) return 0;
    size_t len = strlen(buf);
    if (len && (buf[len-1] == '\n' || buf[len-1] == '\r')) buf[--len] = 0;
    return 1;
}

// Helper: uppercase copy for command matching
static void upper_inplace(char *s){
    for (; *s; ++s) *s = (char)toupper((unsigned char)*s);
}

//start of main program
int main(void) {
    int id;
    char command[64], line[512], match[64], cmdline[256], current_path[PATH_LENGTH] = ""; // empty string = no file yet

    while (1) {
        printf("\nEnter command (OPEN, SHOW ALL, QUERY, UPDATE, INSERT, DELETE, SAVE, EXIT): ");
        if (!read_line(command, sizeof command)) break;

        // Normalize command for matching
        strncpy(match, command, sizeof match - 1);
        match[sizeof match - 1] = 0;
        upper_inplace(match);

        //OPEN command
        if (strcmp(match, "OPEN") == 0) {
            char path[PATH_LENGTH];
            printf("Please enter the database file path: ");
            //calls helper to read line, if it fails, cancels OPEN operation
            if (!read_line(path, sizeof path)) { puts("Open cancelled."); continue; }
            if (openDatabase(path) != 0) {
                perror("OPEN failed");
            } else {
                printf("Database opened with %d records.\n", db_count());
            }

        //SHOW ALL command
        } else if (strcmp(match, "SHOW ALL") == 0) {
            // Display all records in student records
            // Uses database accessors so formatting stays here
            size_t n = db_count();
            if (n == 0) {
                puts("No records to display. Open or insert records first.");
                continue;
            }
            puts("Here are all the records found in the table StudentRecords.");
            puts("ID           Name                 Programme               Mark");
            for (size_t i = 0; i < n; ++i) {
                const Student *s = db_get(i);
                printf("%-11d %-20s %-22s %.1f\n", s->id, s->name, s->programme, s->mark);
            }

        //QUERY command
        } else if (strcmp(match, "QUERY") == 0) {
            printf("Enter student ID: ");
            if (!read_line(command, sizeof command)) break;
            if (sscanf(command, "%d", &id) != 1 || id <= 0) {
                printf("Invalid ID. Please enter a positive number and do not leave this field empty.\n");
                continue;
            }
            queryRecord(id);

        //UPDATE command
        } else if (strcmp(match, "UPDATE") == 0) {
            printf("Enter student ID: ");
            if (!read_line(command, sizeof command)) break;
            if (sscanf(command, "%d", &id) != 1 || id <= 0) {
                printf("Invalid ID. Please enter a positive number.\n");
                continue;
            }
            updateRecord(id);

        //INSERT command
        } else if (strcmp(match, "INSERT") == 0) {
            insertRecord();
        
        //SAVE command
        } else if (strcmp(match, "SAVE") == 0) {
            char path[PATH_LENGTH];
            //check if there are any records to save
            if (db_count() == 0) {
                printf("There is no record to save.\n");
                continue;
            }
            //if no file is opened, will ask for filename to save as
            if (current_path[0] == '\0') {
                printf("No file currently opened.\n");
                printf("Enter a filename to save as: ");
                if (!read_line(path, sizeof path) || path[0] == '\0') {
                    printf("SAVE cancelled.\n");
                    continue;
                }
                if (saveDatabase(path) == 0) {
                    strcpy(current_path, path);   
                    printf("The database file \"%s\" has been successfully saved.\n", current_path);
                } else {
                    printf("Failed to save database file.\n");
                }
            } else {
                //If a file is currently opened
                if (saveDatabase(current_path) == 0) {
                    printf("The database file \"%s\" has been successfully saved.\n", current_path);
                } else {
                    printf("Failed to save database file.\n");
                }
            }

        //DELETE command
        } else if (strcmp(match, "DELETE") == 0) {
            int id;
            char buf[64];
            printf("Enter the student ID you wish to delete: ");
            //reads input line, if it fails or is invalid, will cancel DELETE operation
            if (!fgets(buf, sizeof buf, stdin) || sscanf(buf, "%d", &id) != 1) {
                puts("Delete cancelled.");
                continue;
            }
            deleteRecord(id);

    } else if (strcmp(match, "EXIT") == 0) {
        printf("\nExiting program... Goodbye!\n");
        break;

    } else if (strcmp(match, "SUMMARY") == 0) {
        showSummary();

    } else {
        printf("Invalid command. Try again.\n");
    }
}
return 0;
}
