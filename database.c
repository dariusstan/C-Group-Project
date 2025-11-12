#include <stdio.h>
#include <string.h>
#include <ctype.h>  // helpers, mainly for isdigit() and isalpha()
#include <stdlib.h>  // for atoi() because fgets reads it as a string and this is to change it to int
#include "database.h" 

Student records[MAX_RECORDS];
int recordCount = 0;

// --- Helper: validate name (alphabetical and not empty) ---
int isValidName(const char *name) {
    if (name == NULL || name[0] == '\0') {
        printf("Invalid name. Name cannot be empty.\n");
        return 0;
    }

    for (int i = 0; name[i] != '\0'; i++) {
        if (!isalpha((unsigned char)name[i]) && name[i] != ' ') {
            printf("Invalid name. Only letters and spaces are allowed.\n");
            return 0;
        }
    }

    return 1; // valid
}

// --- Helper: validate programme ---
int isValidProgramme(const char* programme) {
    const char* valid_programmes[] = {"CS", "CE", "EE"}; // Example valid programmes
    int num_valid_programmes = sizeof(valid_programmes) / sizeof(valid_programmes[0]);

    for (int i = 0; i < num_valid_programmes; i++) {
        if (strcmp(programme, valid_programmes[i]) == 0) {
            return 1; // It's a valid programme
        }
    }
    printf("Invalid programme. Please use one of the following: CS, CE, EE.\n");
    return 0; // It's an invalid programme
}

// --- OPEN FEATURE ---
int openDatabase(const char *path) {
    FILE *fp = fopen(path, "r");
    if (!fp) { 
        perror("OPEN"); 
        return -1; 
    }

    char line[512];
    // Find header line starting with "ID"
    while (fgets(line, sizeof line, fp)) {
        const char *p = line;
        while (*p == ' ' || *p == '\t') ++p;
        if (strncmp(p, "ID", 2) == 0) break;
    }

    recordCount = 0;
    while (recordCount < MAX_RECORDS && fgets(line, sizeof line, fp)) {
        const char *p = line;
        while (*p == ' ' || *p == '\t') ++p;
        if (!isdigit((unsigned char)*p)) continue;

        Student s;
        if (sscanf(line, "%d\t%49[^\t]\t%49[^\t]\t%f",
                   &s.id, s.name, s.programme, &s.mark) == 4) {
            records[recordCount++] = s;
        }
    }

    fclose(fp);
    return 0;
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

    printf("\nEnter new name (if needed): ");
    fgets(records[index].name, sizeof(records[index].name), stdin);
    records[index].name[strcspn(records[index].name, "\n")] = '\0'; // remove newline

    if (!isValidName(records[index].name)) {
        printf("Name not updated (invalid input).\n");
        return;
    }

    printf("Enter new programme (if needed): ");
    fgets(records[index].programme, sizeof(records[index].programme), stdin);
    records[index].programme[strcspn(records[index].programme, "\n")] = '\0';

    printf("Enter new mark (if needed): ");
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
    char idInput[20];  // buffer for ID input as string

    printf("\nEnter new student ID (7 digits): ");
    if (!fgets(idInput, sizeof(idInput), stdin)) {
        printf("Input error.\n");
        return;
    }
    idInput[strcspn(idInput, "\n")] = '\0'; // remove newline

    // --- Empty ID check ---
    if (idInput[0] == '\0') {
        printf("Invalid ID. ID cannot be empty.\n");
        return;
    }

    // --- Numeric check ---
    for (int i = 0; idInput[i] != '\0'; i++) {
        if (!isdigit((unsigned char)idInput[i])) {
            printf("Invalid ID. Please enter a numeric and positive number.\n");
            return;
        }
    }

    // --- Convert to integer ---
    newStudent.id = atoi(idInput);

    // --- 7-digit length check ---
    if (newStudent.id < 1000000 || newStudent.id > 9999999) {
        printf("Invalid ID length. Student ID must be exactly 7 digits and cannot start with 0.\n");
        return;
    }

    // --- Duplicate ID check ---
    for (int i = 0; i < recordCount; i++) {
        if (records[i].id == newStudent.id) {
            printf("Error: A record with ID %d already exists.\n", newStudent.id);
            return;
        }
    }

    // --- Name input + validation ---
    printf("Enter name: ");
    fgets(newStudent.name, sizeof(newStudent.name), stdin);
    newStudent.name[strcspn(newStudent.name, "\n")] = '\0';
    if (!isValidName(newStudent.name)) return;

// --- Programme and Mark ---
printf("Enter programme: ");
fgets(newStudent.programme, sizeof(newStudent.programme), stdin);
newStudent.programme[strcspn(newStudent.programme, "\n")] = '\0';
if (!isValidProgramme(newStudent.programme)) return;  // Add this line

printf("Enter mark: ");
scanf("%f", &newStudent.mark);

records[recordCount++] = newStudent;
printf("\nRecord inserted successfully!\n");

// --- DELETE FEATURE ---
int deleteRecord(int id) {
    int i = 0;
    for (; i < recordCount && records[i].id != id; ++i) {}
    if (i == recordCount) {
        printf("No record found with ID %d.\n", id);
        return -1;
    }

    printf("Found: ID=%d, Name=%s, Programme=%s, Mark=%.2f\n",
           records[i].id, records[i].name, records[i].programme, records[i].mark);

    printf("Confirm delete? (Y/N): ");
    char a[8];
    if (!fgets(a, sizeof a, stdin) || (a[0] != 'Y' && a[0] != 'y')) {
        puts("Deletion cancelled.");
        return 1;
    }

    for (int j = i + 1; j < recordCount; ++j)
        records[j - 1] = records[j];
    recordCount--;

    printf("Record with ID %d deleted.\n", id);
    return 0;
}

// ===== Read-only accessors for SHOW ALL ===== 
size_t db_count(void) {
    return (size_t)recordCount;
}

const Student* db_get(size_t idx) {
    return idx < (size_t)recordCount ? &records[idx] : NULL;
}

// --- SUMMARY STATISTICS ---
void showSummary() {
    if (recordCount == 0) {
        printf("No records in the database to summarize.\n");
        return;
    }

    float total_mark = 0;
    float highest_mark = records[0].mark;
    float lowest_mark = records[0].mark;
    int highest_idx = 0;
    int lowest_idx = 0;

    for (int i = 0; i < recordCount; i++) {
        total_mark += records[i].mark;
        if (records[i].mark > highest_mark) {
            highest_mark = records[i].mark;
            highest_idx = i;
        }
        if (records[i].mark < lowest_mark) {
            lowest_mark = records[i].mark;
            lowest_idx = i;
        }
    }

    printf("\n--- Summary Statistics ---\n");
    printf("Total number of students: %d\n", recordCount);
    printf("Average mark: %.2f\n", total_mark / recordCount);
    printf("Highest mark: %.2f (Student: %s)\n", highest_mark, records[highest_idx].name);
    printf("Lowest mark: %.2f (Student: %s)\n", lowest_mark, records[lowest_idx].name);
    printf("--------------------------\n\n");
}