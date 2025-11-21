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

//start of main program
int main(void) {
    int id;
    char command[64], match[64], current_path[PATH_LENGTH] = "";

    while (1) {
        printf("\nEnter command (Enter 'HELP' to see all commands): ");
        if (!read_line(command, sizeof command)) break;

        // Normalize command
        strncpy(match, command, sizeof match - 1);
        match[sizeof match - 1] = 0;
        upper_inplace(match);

        CommandType cmd = parseCommand(match);

        switch(cmd) {
        /* ==================================================
           Help feature
        ================================================== */
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

        /* ==================================================
           OPEN
        ================================================== */
            case CMD_OPEN:
                char path[PATH_LENGTH];
                printf("Enter database file path: ");
                //calls helper to read line, if fails, cancels OPEN operation
                if (!read_line(path, sizeof path)) { puts("Open cancelled."); continue; }
                if (openDatabase(path) != 0) {
                    perror("OPEN failed");
                } else {
                    printf("Database opened with %d records.\n", db_count());
                }
                break;
        /* ==================================================
           SHOW ALL
        ================================================== */
            case CMD_SHOW_ALL: {
                size_t n = db_count();
                if (n == 0) {
                    puts("No records to display. Open or insert records first.");
                    continue;
                }

                if (strstr(match, "SORT BY") != NULL) {
                        // Extract what comes after "SHOW ALL SORT BY "
                        char *sortParams = match + strlen("SHOW ALL SORT BY ");

                        char field[20] = "";
                        char order[10] = "";
                        // field = "ID" or "MARK" or "GRADE"
                        // order = "ASC" or "DESC"
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

        /* ==================================================
           QUERY
        ================================================== */
            case CMD_QUERY:
                printf("Enter student ID: ");
                if (!read_line(command, sizeof command)) continue;
                if (sscanf(command, "%d", &id) != 1 || id <= 0) {
                    printf("Invalid ID. Please enter a positive number.\n");
                    continue;
                }

                queryRecord(id);
                break;
        /* ==================================================
           UPDATE
        ================================================== */
            case CMD_UPDATE:
                printf("Enter student ID: ");
                if (!read_line(command, sizeof command)) continue;
                if (sscanf(command, "%d", &id) != 1 || id <= 0) {
                    printf("Invalid ID. Please enter a positive number.\n");
                    continue;
                }
                updateRecord(id);
                break;

        /* ==================================================
           INSERT
        ================================================== */
            case CMD_INSERT:
                insertRecord();
                break;

        /* ==================================================
           DELETE
        ================================================== */
            case CMD_DELETE:
                int id;
                char buf[64];
                printf("Enter student ID to delete: ");
                if (!fgets(buf, sizeof buf, stdin) || sscanf(buf, "%d", &id) != 1) {
                    puts("Delete cancelled.");
                    continue;
                }
                deleteRecord(id);
                break;

        /* ==================================================
           SAVE
        ================================================== */
            case CMD_SAVE:
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
                        printf("Failed to save database file.\n");
                    }

                } else {
                    if (saveDatabase(current_path) == 0) {
                        printf("The database file \"%s\" is successfully saved.\n", current_path);
                    } else {
                        printf("Failed to save database file.\n");
                    }
                }
                break;
            }

        /* ==================================================
           SUMMARY
        ================================================== */
            case CMD_SUMMARY:
                showSummary();
                break;

        /* ==================================================
           EXPORT
        ================================================== */
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
            
        /* ==================================================
           EXIT
        ================================================== */
            case CMD_EXIT:
                return 0;
            
        /* ==================================================
           INVALID
        ================================================== */
            default:
                printf("Invalid command. Try again.\n");
                break;
        }
    }
    return 0;
}
