#include <stdio.h>
#include <string.h>
#include "database.h"

Student records[MAX_RECORDS];
int recordCount = 0;

// --- TEMP DATA for testing (you can later replace this with file input) ---
void loadSampleData() {
    recordCount = 3;
    records[0] = (Student){2301234, "Joshua", "SoftwareEngineering", 70.5};
    records[1] = (Student){2201234, "Isaac", "ComputerScience", 63.4};
    records[2] = (Student){2304567, "John", "DigitalSupplyChain", 85.9};
}

// --- QUERY FEATURE ---
int queryRecord(int id) {
    for (int i = 0; i < recordCount; i++) {
        if (records[i].id == id) {
            printf("\nRecord found:\n");
            printf("ID: %d\nName: %s\nProgramme: %s\nMark: %.2f\n",
                   records[i].id,
                   records[i].name,
                   records[i].programme,
                   records[i].mark);
            return i;
        }
    }
    printf("\nNo record found with ID %d.\n", id);
    return -1;
}

// --- UPDATE FEATURE ---
void updateRecord(int id) {
    int index = queryRecord(id);
    if (index == -1) return;

    printf("\nEnter new name: ");
    scanf("%s", records[index].name);
    printf("Enter new programme: ");
    scanf("%s", records[index].programme);
    printf("Enter new mark: ");
    scanf("%f", &records[index].mark);

    printf("\nRecord updated successfully!\n");
}

// --- INSERT FEATURE ---
void insertRecord() {
    Student newStudent;

    printf("\nEnter new student ID: ");
    scanf("%d", &newStudent.id);

    // Check for duplicates
    for (int i = 0; i < recordCount; i++) {
        if (records[i].id == newStudent.id) {
            printf("Error: A record with ID %d already exists.\n", newStudent.id);
            return;
        }
    }

    printf("Enter name: ");
    scanf("%s", newStudent.name);
    printf("Enter programme: ");
    scanf("%s", newStudent.programme);
    printf("Enter mark: ");
    scanf("%f", &newStudent.mark);

    records[recordCount] = newStudent;
    recordCount++;

    printf("\nRecord inserted successfully!\n");
}
