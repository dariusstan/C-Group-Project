#include <stdio.h>
#include <string.h>
#include <ctype.h>  //helpers, mainly for isdigit()
#include "database.h" // actual code logic

Student records[MAX_RECORDS];
int recordCount = 0;

// // --- TEMP DATA for testing (you can later replace this with file input) ---
// void loadSampleData(void) {
//     recordCount = 3;
//     records[0] = (Student){2301234, "Joshua", "SoftwareEngineering", 70.5f};
//     records[1] = (Student){2201234, "Isaac", "ComputerScience", 63.4f};
//     records[2] = (Student){2304567, "John", "DigitalSupplyChain", 85.9f};
// }

// --- OPEN FEATURE ---
int openDatabase(const char *path) {
    FILE *fp = fopen(path, "r");
    //If cant open file, print error and return -1 to indicate failure
    if (!fp) { perror("OPEN"); return -1; }

    char line[512];
    //Loop to find the first header ID 
    while (fgets(line, sizeof line, fp)) {
        // set p to start of line, skip spaces/tabs
        const char *p=line; while(*p==' '||*p=='\t') ++p;
        if (strncmp(p,"ID",2)==0) break;
    }

    recordCount = 0;
    //reads lines until end of file or max capacity reached
    while (recordCount < MAX_RECORDS && fgets(line, sizeof line, fp)) {
        const char *p=line; while(*p==' '||*p=='\t') ++p;
        //Ensures first character is a digit before parsing
        if (!isdigit((unsigned char)*p)) continue;
        Student s;
        //formats each row, and only adds to records if all 4 fields successfully parsed
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
        printf("Invalid ID. Please enter a positive number.\n");
        while (getchar() != '\n'); // clear bad input
        return;
    }

    // --- 7-digit length check ---
    if (newStudent.id < 1000000 || newStudent.id > 9999999) {
        printf("Invalid ID length. Student ID must be exactly 7 digits.\n");
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

// --- DELETE FEATURE ---
int deleteRecord(int id) {
    int i = 0;
    for (; i < recordCount && records[i].id != id; ++i) {}
    if (i == recordCount) {                      // not found
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

    for (int j = i + 1; j < recordCount; ++j)    // shift left
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
