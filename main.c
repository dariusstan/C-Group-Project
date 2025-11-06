#include <stdio.h>
#include <string.h>
#include "database.h" //entry point, calls functions in database.h

int main() {
    char command[20];
    int id;

    loadSampleData(); // Temporary data for testing

    while (1) {
        printf("\nEnter command (QUERY, UPDATE, INSERT, EXIT): ");
        scanf(" %[^\n]", command);

        if (strcmp(command, "QUERY") == 0) {
            printf("Enter student ID: ");
            scanf("%d", &id);
            queryRecord(id);
        } 
        else if (strcmp(command, "UPDATE") == 0) {
            printf("Enter student ID: ");
            scanf("%d", &id);
            updateRecord(id);
        } 
        else if (strcmp(command, "INSERT") == 0) {
            insertRecord();
        } 
        else if (strcmp(command, "EXIT") == 0) {
            break;
        } 
        else {
            printf("Invalid command.\n");
        }
    }
    return 0;
}
