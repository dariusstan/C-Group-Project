#include <stdio.h>
#include <string.h>
#include "database.h" // actual code logic

Student records[MAX_RECORDS];
int recordCount = 0;

// --- TEMP DATA for testing (you can later replace this with file input) ---
void loadSampleData(void) {
    recordCount = 3;
    records[0] = (Student){2301234, "Joshua", "SoftwareEngineering", 70.5f};
    records[1] = (Student){2201234, "Isaac", "ComputerScience", 63.4f};
    records[2] = (Student){2304567, "John", "DigitalSupplyChain", 85.9f};
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
    scanf("%49s", records[index].name);        // width limit to avoid overflow
    printf("Enter new programme: ");
    scanf("%49s", records[index].programme);   // width limit
    printf("Enter new mark: ");
    scanf("%f", &records[index].mark);

    printf("\nRecord updated successfully!\n");
}

// --- INSERT FEATURE ---
void insertRecord(void) {
    if (recordCount >= MAX_RECORDS) {
        printf("Cannot insert: database is full.\n");
        return;
    }

    Student newStudent;

    printf("\nEnter new student ID (7 digits): ");
    if (scanf("%d", &newStudent.id) != 1 || newStudent.id <= 0) {
        printf("Invalid ID. Please enter a numeric and positive number.\n");
        while (getchar() != '\n'); // clear bad input
        return;
    }

    // --- 7-digit length check ---
    if (newStudent.id < 1000000 || newStudent.id > 9999999) {
        printf("Invalid ID length. Student ID must be exactly 7 digits and cannot start with 0\n");
        return;
    }

    // --- Duplicate ID check ---
    for (int i = 0; i < recordCount; i++) {
        if (records[i].id == newStudent.id) {
            printf("Error: A record with ID %d already exists.\n", newStudent.id);
            return;
        }
    }

    // --- Continue with other fields ---
    printf("Enter name: ");
    scanf("%49s", newStudent.name);            // width limit
    printf("Enter programme: ");
    scanf("%49s", newStudent.programme);       // width limit
    printf("Enter mark: ");
    scanf("%f", &newStudent.mark);

    records[recordCount++] = newStudent;

    printf("\nRecord inserted successfully!\n");
}

// ===== Read-only accessors for SHOW ALL ===== 

size_t db_count(void) {
    return (size_t)recordCount;
}

const Student* db_get(size_t idx) {
    return idx < (size_t)recordCount ? &records[idx] : NULL;
}
