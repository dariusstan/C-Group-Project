#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "database.h"  // entry point, connects to functions in database.c

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

int main(void) {
    char command[64];
    int id;
    char line[512], match[32], arg[480]; 

    while (1) {
        printf("\nEnter command (OPEN, SHOW ALL, QUERY, UPDATE, INSERT, DELETE, EXIT): ");
        if (!read_line(command, sizeof command)) break;

        // Normalize command for matching
        char match[64];
        strncpy(match, command, sizeof match - 1);
        match[sizeof match - 1] = 0;
        upper_inplace(match);

        if (strcmp(match, "OPEN") == 0) {
            char path[512];
            printf("Enter database file path: ");
            //calls helper to read line, if fails, cancels OPEN operation
            if (!read_line(path, sizeof path)) { puts("Open cancelled."); continue; }
            if (openDatabase(path) != 0) {
                perror("OPEN failed");
            } else {
                printf("Database opened with %d records.\n", recordCount);
            }

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
        } else if (strcmp(match, "QUERY") == 0) {

            printf("Enter student ID: ");
            if (!read_line(command, sizeof command)) break;
            if (sscanf(command, "%d", &id) != 1 || id <= 0) {
                printf("Invalid ID. Please enter a positive number and do not leave this field empty.\n");
                continue;
            }
            queryRecord(id);

        } else if (strcmp(match, "UPDATE") == 0) {

            printf("Enter student ID: ");
            if (!read_line(command, sizeof command)) break;
            if (sscanf(command, "%d", &id) != 1 || id <= 0) {
                printf("Invalid ID. Please enter a positive number.\n");
                continue;
            }
            updateRecord(id);

        } else if (strcmp(match, "INSERT") == 0) {
            insertRecord();

        } else if (strcmp(match, "SAVE") == 0) {
            printf("Saving records...\n");

        } else if (strcmp(match, "DELETE") == 0) {
            int id;
            // Try to parse an ID following the DELETE command 
            if (sscanf(arg, "%*s %d", &id) != 1) {
                char buf[64];
                printf("Enter student ID to delete: ");
                if (!fgets(buf, sizeof buf, stdin) || sscanf(buf, "%d", &id) != 1) {
                    puts("Delete cancelled.");
                    continue;
                }
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
