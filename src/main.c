#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "database.h"
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

int main(void) {
    int id;
    char command[64], match[64], current_path[PATH_LENGTH] = "";

    while (1) {
        printf("\nEnter command (OPEN, SHOW ALL, QUERY, UPDATE, INSERT, DELETE, SAVE, SUMMARY, EXIT): ");
        if (!read_line(command, sizeof command)) break;

        // Normalize command
        strncpy(match, command, sizeof match - 1);
        match[sizeof match - 1] = 0;
        upper_inplace(match);

        /* ==================================================
           OPEN
        ================================================== */
        if (strcmp(match, "OPEN") == 0) {

            char path[PATH_LENGTH];
            printf("Enter database file path: ");

            if (!read_line(path, sizeof path)) {
                puts("Open cancelled.");
                continue;
            }

            if (openDatabase(path) != 0) {
                perror("OPEN failed");
            } else {
                strcpy(current_path, path);
                printf("Database opened with %zu records.\n", db_count());
            }

        /* ==================================================
           SHOW ALL
        ================================================== */
        } else if (strcmp(match, "SHOW ALL") == 0) {

            size_t n = db_count();
            if (n == 0) {
                puts("No records to display. Open or insert records first.");
                continue;
            }

            puts("Here are all the records found in the table StudentRecords.");
            puts("ID           Name                 Programme               Mark");

            for (size_t i = 0; i < n; ++i) {
                const Student *s = db_get(i);
                printf("%-11d %-20s %-22s %.1f\n",
                       s->id, s->name, s->programme, s->mark);
            }

        /* ==================================================
           QUERY
        ================================================== */
        } else if (strcmp(match, "QUERY") == 0) {

            printf("Enter student ID: ");

            if (!read_line(command, sizeof command)) continue;
            if (sscanf(command, "%d", &id) != 1 || id <= 0) {
                printf("Invalid ID. Please enter a positive number.\n");
                continue;
            }

            queryRecord(id);

        /* ==================================================
           UPDATE
        ================================================== */
        } else if (strcmp(match, "UPDATE") == 0) {

            printf("Enter student ID: ");

            if (!read_line(command, sizeof command)) continue;
            if (sscanf(command, "%d", &id) != 1 || id <= 0) {
                printf("Invalid ID. Please enter a positive number.\n");
                continue;
            }

            updateRecord(id);

        /* ==================================================
           INSERT — FIXED & CLEAN
        ================================================== */
        } else if (strcmp(match, "INSERT") == 0) {
            insertRecord();
        /* ==================================================
           SAVE
        ================================================== */
        } else if (strcmp(match, "SAVE") == 0) {

            char path[PATH_LENGTH];

            if (db_count() == 0) {
                printf("There is no record to save.\n");
                continue;
            }

            if (current_path[0] == '\0') {

                printf("No file currently opened.\n");
                printf("Enter a filename to save as: ");

                if (!read_line(path, sizeof path) || path[0] == '\0') {
                    printf("SAVE cancelled.\n");
                    continue;
                }

                if (saveDatabase(path) == 0) {
                    strcpy(current_path, path);
                    printf("The database file \"%s\" is successfully saved.\n", current_path);
                } else {
                    printf("Failed to save database.\n");
                }

            } else {
                if (saveDatabase(current_path) == 0) {
                    printf("The database file \"%s\" is successfully saved.\n", current_path);
                } else {
                    printf("Failed to save database.\n");
                }
            }

        /* ==================================================
           DELETE
        ================================================== */
        } else if (strcmp(match, "DELETE") == 0) {

            char buf[64];
            printf("Enter student ID to delete: ");

            if (!fgets(buf, sizeof buf, stdin) || sscanf(buf, "%d", &id) != 1) {
                puts("Delete cancelled.");
                continue;
            }

            deleteRecord(id);

        /* ==================================================
           SUMMARY
        ================================================== */
        } else if (strcmp(match, "SUMMARY") == 0) {

            showSummary();

        /* ==================================================
           EXIT
        ================================================== */
        } else if (strcmp(match, "EXIT") == 0) {

            printf("\nExiting program... Goodbye!\n");
            break;

        /* ==================================================
           INVALID
        ================================================== */
        } else {
            printf("Invalid command. Try again.\n");
        }
    }

    return 0;
}

