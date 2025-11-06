#include <stdio.h>
#include <string.h>
#include "database.h"  // entry point, connects to functions in database.c

int main() {
    char command[20];
    int id;

    loadSampleData(); // Temporary data for testing

    while (1) {
        printf("\nEnter command (QUERY, UPDATE, INSERT, EXIT): ");
        scanf(" %[^\n]", command);

        // Convert to uppercase-insensitive match if you want
        if (strcmp(command, "QUERY") == 0) {

            printf("Enter student ID: ");
            if (scanf("%d", &id) != 1 || id <= 0) {
                printf("Invalid ID. Please enter a positive number.\n"); //put validation here instead of database.c because it handles user input here 
                while (getchar() != '\n'); // clear invalid input
                continue; // skip back to menu
            }
            queryRecord(id);
        } 
        else if (strcmp(command, "UPDATE") == 0) {

            printf("Enter student ID: ");
            if (scanf("%d", &id) != 1 || id <= 0) {
                printf("Invalid ID. Please enter a positive number.\n"); //put validation here instead of database.c because it handles user input here 
                while (getchar() != '\n');
                continue;
            }
            updateRecord(id);
        } 
        else if (strcmp(command, "INSERT") == 0) {
            insertRecord();
        } 
        else if (strcmp(command, "EXIT") == 0) {
            printf("\nExiting program... Goodbye!\n");
            break;
        } 
        else {
            printf("Invalid command. Try again.\n");
        }
    }
    return 0;
}

