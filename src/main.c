#include <stdio.h>  // for printf, fgets, perror
#include <string.h>  // for strcmp, strncpy, strlen, strstr
#include <ctype.h>  // for toupper
#include "database.h"  //database functions  

#define PATH_LENGTH 512  // defined reusable constant for path length

// Helper: read a whole line safely
static int read_line(char *buf, size_t n){
    if (!fgets(buf, (int)n, stdin)) return 0;
    size_t len = strlen(buf);
    // if newline at end or carriage return, remove it
    if (len && (buf[len-1] == '\n' || buf[len-1] == '\r')) buf[--len] = 0;
    return 1;
}

// Helper: convert user input to uppercase in-place
static void upper_inplace(char *s){
    for (; *s; ++s) *s = (char)toupper((unsigned char)*s);
}

// Helper: parse command string to enum
CommandType parseCommand(const char *cmd) {
    if (strcmp(cmd, "HELP") == 0) return CMD_HELP;
    if (strcmp(cmd, "OPEN") == 0) return CMD_OPEN;
    if (strncmp(cmd, "SHOW ALL", 8) == 0) return CMD_SHOW_ALL;
    if (strcmp(cmd, "QUERY") == 0) return CMD_QUERY;
    if (strcmp(cmd, "UPDATE") == 0) return CMD_UPDATE;
    if (strcmp(cmd, "INSERT") == 0) return CMD_INSERT;
    if (strcmp(cmd, "DELETE") == 0) return CMD_DELETE;
    if (strcmp(cmd, "SAVE") == 0) return CMD_SAVE;
    if (strcmp(cmd, "SUMMARY") == 0) return CMD_SUMMARY;
    if (strcmp(cmd, "EXPORT") == 0) return CMD_EXPORT;
    if (strcmp(cmd, "EXIT") == 0) return CMD_EXIT;
    return CMD_UNKNOWN;
}

// start of main program
int main(void) {
    int id;
    char command[64], match[64], current_path[PATH_LENGTH] = "";
    char path[PATH_LENGTH];   // <------ DECLARE 'path' ONLY ONCE HERE

    while (1) {
        printf("\nEnter command (Enter 'HELP' to see all commands): ");
        if (!read_line(command, sizeof command)) break;

        // Convert command to uppercase for case-insensitive comparison
        strncpy(match, command, sizeof match - 1);
        match[sizeof match - 1] = 0;
        upper_inplace(match);

        CommandType cmd = parseCommand(match);

        switch(cmd) {
            case CMD_HELP:
                printf("Available commands:\n");
                printf("  OPEN       - Open a database file\n");
                printf("  SHOW ALL   - Display all student records\n");
                printf("     (Optional Sort Syntax: SHOW ALL SORT BY <FIELD> <ORDER>)\n");
                printf("        <FIELD>: ID, NAME, PROGRAMME, MARK, GRADE\n");
                printf("        <ORDER>: ASC (Ascending) or DESC (Descending)\n");
                printf("  QUERY      - Query a student record by ID\n");
                printf("  UPDATE     - Update a student record by ID\n");
                printf("  INSERT     - Insert a new student record\n");
                printf("  DELETE     - Delete a student record by ID\n");
                printf("  SAVE       - Save the current database to file\n");
                printf("  SUMMARY    - Show summary of records\n");
                printf("  EXPORT     - Export records to CSV file\n");
                printf("  EXIT       - Exit the program\n");
                break;

            case CMD_OPEN:
                printf("Enter database file path: ");
                if (!read_line(path, sizeof path) || path[0] == '\0') {
                    puts("OPEN cancelled.");
                    break;
                }

                if (openDatabase(path) != 0) {
                    perror("OPEN failed");
                } else {
                    // Store the opened path
                    strncpy(current_path, path, PATH_LENGTH - 1);
                    current_path[PATH_LENGTH - 1] = '\0';
                    printf("Database opened with %zu records.\n", db_count());
                }
                break;

            //SHOW ALL operation
            case CMD_SHOW_ALL: {
                size_t n = db_count();
                if (n == 0) {
                    puts("No records to display. Open or insert records first.");
                    continue;
                }
                // SORT BY enhancement feature
                if (strstr(match, "SORT BY") != NULL) {
                    char *sortParams = match + strlen("SHOW ALL SORT BY ");
                    char field[20] = "";  // to hold field name
                    char order[10] = "";  // to hold order
                    int ascending = 1; // default ascending
                    if (sscanf(sortParams, "%s %s", field, order) == 2) {
                        if(strcmp(order, "ASC") != 0 && strcmp(order, "DESC") != 0) {
                            printf("Invalid sort order: %s. Use ASC or DESC.\n", order);
                            continue;
                        }
                        ascending = (strcmp(order, "ASC") == 0) ? 1 : 0;
                    }
                    if (strcmp(field, "ID") == 0) {
                        sortStudents(SORT_ID, ascending);
                    } else if (strcmp(field, "MARK") == 0) {
                        sortStudents(SORT_MARK, ascending);
                    } else if (strcmp(field, "GRADE") == 0) {
                        sortStudents(SORT_GRADE, ascending);
                    } else if (strcmp(field, "NAME") == 0) {
                        sortStudents(SORT_NAME, ascending);
                    } else if (strcmp(field, "PROGRAMME") == 0) {
                        sortStudents(SORT_PROGRAMME, ascending);
                    } else {
                        printf("Unknown field: %s\n", field);
                        break;
                    }
                }
                showAll();
                break;
            }

            //QUERY operation
            case CMD_QUERY:
                printf("Enter student ID: ");
                if (!read_line(command, sizeof command)) continue;
                //if invalid input, won't call queryRecord function
                if (sscanf(command, "%d", &id) != 1 || id <= 0) {
                    printf("Invalid ID. Please enter a positive number, it cannot be empty as well.\n");
                    continue;
                }
                queryRecord(id);
                break;

            //UPDATE operation
            case CMD_UPDATE:
                printf("Enter student ID: ");
                if (!read_line(command, sizeof command)) continue;
                if (sscanf(command, "%d", &id) != 1 || id <= 0) {
                    printf("Invalid ID. Please enter a positive number, it cannot be empty as well.\n");
                    continue;
                }
                updateRecord(id);
                break;

            //INSERT operation
            case CMD_INSERT:
                insertRecord();
                break;
            
            //DELETE operation
            case CMD_DELETE:
                printf("Enter student ID to delete: ");
                // if invalid input, won't call deleteRecord function
                if (!read_line(command, sizeof command) || sscanf(command, "%d", &id) != 1) {
                    puts("Delete cancelled.");
                    continue;
                }
                deleteRecord(id);
                break;

            //SAVE operation
            case CMD_SAVE:
                if (db_count() == 0) {
                    printf("There is no record to save.\n");
                    break;
                }

                if (current_path[0] == '\0') {
                    printf("No file currently opened.\n");
                    printf("Enter a filename to save as: ");
                    if (!read_line(path, sizeof path) || path[0] == '\0') {
                        printf("SAVE cancelled.\n");
                        break;
                    }
                    strcpy(current_path, path);
                }

                if (saveDatabase(current_path) == 0) {
                    printf("The database file \"%s\" is successfully saved.\n", current_path);
                } else {
                    printf("Failed to save database file.\n");
                }
                break;

            // sUMMARY operation    
            case CMD_SUMMARY:
                showSummary();
                break;

            // EXPORT operation
            case CMD_EXPORT:
            {
                if (db_count() == 0) {
                    printf("There is no record to export.\n");
                    continue;
                }
                if (exportToCSV("data.csv") == 0) {
                    printf("The database file \"%s\" is successfully exported.\n", current_path);
                } else {
                    printf("Failed to export database.\n");
                }
                break;
            }
            case CMD_EXIT:
                return 0;
            default:
                printf("Invalid command. Try again.\n");
                break;
        }
    }
    return 0;
}
