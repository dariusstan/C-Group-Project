#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "database.h"

Student records[MAX_RECORDS];
int recordCount = 0;

/* ============================================================
   Helper: calculate grade from mark
   ============================================================ */
char getGrade(float mark) {
    if (mark >= 80) return 'A';
    else if (mark >= 70) return 'B';
    else if (mark >= 60) return 'C';
    else if (mark >= 40) return 'D';
    else return 'F';
}

/* ============================================================
   Helper: validate name
   ============================================================ */
int isValidName(const char *name) {
    if (name == NULL || name[0] == '\0') {
        printf("Invalid name. Name cannot be empty.\n");
        return 0;
    }
    for (int i = 0; name[i] != '\0'; i++) {
        if (!isalpha((unsigned char)name[i]) && name[i] != ' ') {
            printf("Invalid name. Only letters and spaces allowed.\n");
            return 0;
        }
    }
    return 1;
}

/* ============================================================
   Helper: validate programme
   ============================================================ */
int isValidProgramme(const char *programme) {
    const char *valid[] = {"CS", "CE", "EE"};
    int n = sizeof(valid) / sizeof(valid[0]);

    for (int i = 0; i < n; i++) {
        if (strcmp(programme, valid[i]) == 0)
            return 1;
    }

    printf("Invalid programme. Use CS, CE, or EE.\n");
    return 0;
}

/* ============================================================
   Helper: validate numeric string for mark
   ============================================================ */
int isNumericString(const char *s) {
    int decimalCount = 0;
    int i = 0;

    if (s == NULL || s[0] == '\0')
        return 0;

    while (s[i] != '\0') {
        if (isdigit((unsigned char)s[i])) {
            /* ok */
        } else if (s[i] == '.') {
            decimalCount++;
            if (decimalCount > 1)
                return 0; /* too many decimals */
        } else {
            return 0; /* reject symbols/letters */
        }
        i++;
    }

    return 1;
}

/* ============================================================
   Helper: validate mark range
   ============================================================ */
int isValidMark(float mark) {
    if (mark < 1.0f || mark > 100.0f) {
        printf("Invalid mark. Must be between 1 and 100.\n");
        return 0;
    }
    return 1;
}

/* ============================================================
   OPEN
   ============================================================ */
int openDatabase(const char *path) {
    char clean[512], line[512];
    FILE *fp;  // file pointer for loaded file
    size_t len;  //length of path

    //writes formatted path without surrounding quotation marks into clean array
    snprintf(clean, sizeof clean, "%s", path);
    len = strlen(clean);
    //checks if first and last character has quotation marks
    if (len > 1) {
        char q = clean[0];
        if ((q == '"' || q == '\'') && clean[len - 1] == q) {
            // replaces last quote with terminator
            clean[len - 1] = '\0';
            // shifts path to left by 1 to remove first quote 
            memmove(clean, clean + 1, len);
        }
    }

    //if file cannot be opened, exits with error 
    fp = fopen(clean, "r");
    if (!fp) {
        return -1;
    }

    // find header line starting with "ID"
    while (fgets(line, sizeof line, fp)) {
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (strncmp(p, "ID", 2) == 0) break;
    }

    recordCount = 0;
    //reads through each line and parses any valid student record(s)
    while (recordCount < MAX_RECORDS && fgets(line, sizeof line, fp)) {
        char *p = line;
        // skips whitespaces and/or tabs
        while (*p == ' ' || *p == '\t') p++;
        // if line does not start with a digit, skip it
        if (!isdigit((unsigned char)*p)) continue;

        Student s;
        // parses line as a record
        if (sscanf(line, "%d\t%49[^\t]\t%49[^\t]\t%f",
                   &s.id, s.name, s.programme, &s.mark) == 4) {
            records[recordCount++] = s;
        }
    }
    fclose(fp);
    return 0;
}

/* ============================================================
   QUERY
   ============================================================ */
int queryRecord(int id) {
    for (int i = 0; i < recordCount; i++) {
        if (records[i].id == id) {
            printf("\nRecord found:\n");
            printf("ID\t: %d\nName\t: %s\nProgramme: %s\nMark\t: %.2f\nGrade\t: %c\n",
                   records[i].id,
                   records[i].name,
                   records[i].programme,
                   records[i].mark,
                   records[i].grade);
            return i;
        }
    }
    printf("\nNo record found with ID %d.\n", id);
    return -1;
}

/* ============================================================
   UPDATE
   ============================================================ */
void updateRecord(int id) {
    int index = queryRecord(id);
    if (index == -1) return;

    char originalName[64];
    char originalProgramme[64];
    float originalMark;

    strcpy(originalName, records[index].name);
    strcpy(originalProgramme, records[index].programme);
    originalMark = records[index].mark;

    // Temporary buffers
    char tempName[64];
    char tempProgramme[64];
    char tempMark[32];

    printf("\nEnter new name (press 'Enter' to keep current): ");
    fgets(tempName, sizeof(tempName), stdin);
    tempName[strcspn(tempName, "\n")] = '\0'; // remove newline

    if (tempName[0] != '\0') {
        // Input is NOT empty, update the name
        if (isValidName(tempName)) {
            strcpy(records[index].name, tempName);
        } else { return; }
    }

    printf("Enter new programme (press 'Enter' to keep current): ");
    fgets(tempProgramme, sizeof(tempProgramme), stdin);
    tempProgramme[strcspn(tempProgramme, "\n")] = '\0'; // remove newline

    if (tempProgramme[0] != '\0') {
        // Input is NOT empty, update the programme
        if (isValidProgramme(tempProgramme)) {
           strcpy(records[index].programme, tempProgramme);
        } else { return; }
    } 

    printf("Enter new mark (press 'Enter' to keep current): ");
    fgets(tempMark, sizeof(tempMark), stdin);
    tempMark[strcspn(tempMark, "\n")] = '\0'; // remove newline

    if (tempMark[0] != '\0') {
        // Input is NOT empty, update the mark
        float newMark = atof(tempMark);
        if (isValidMark(newMark)) {
            records[index].mark = newMark;
        } else { return; }
    }

    records[index].grade = getGrade(records[index].mark);    
    printf("\nRecord updated successfully!\n");
}

/* ============================================================
   INSERT
   ============================================================ */
void insertRecord(void) {
    if (recordCount >= MAX_RECORDS) {
        printf("Cannot insert: database is full.\n");
        return;
    }

    Student newStudent;
    char idInput[20];  // buffer for ID input as string

    /* ID */
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
    printf("Enter programme (CS/CE/EE): ");
    fgets(newStudent.programme, sizeof(newStudent.programme), stdin);
    newStudent.programme[strcspn(newStudent.programme, "\n")] = '\0';
    if (!isValidProgramme(newStudent.programme)) return;  // Add this line

    if (!isValidProgramme(newStudent.programme)) return;

    printf("Enter mark (1-100): ");
    scanf("%f", &newStudent.mark);

    newStudent.grade = getGrade(newStudent.mark);

    records[recordCount++] = newStudent;
    printf("\nRecord inserted successfully!\n");

    getchar(); // clear newline from input buffer
}

/* ============================================================
   DELETE
   ============================================================ */
int deleteRecord(int id) {
    int i = 0;
    for (; i < recordCount && records[i].id != id; i++) {}

    if (i == recordCount) {
        printf("No record found with ID %d.\n", id);
        return -1;
    }

    printf("Found: ID=%d, Name=%s, Programme=%s, Mark=%.2f\n",
           records[i].id,
           records[i].name,
           records[i].programme,
           records[i].mark);

    printf("Confirm delete? (Y/N): ");
    char a[8];
    if (!fgets(a, sizeof a, stdin) || (a[0] != 'Y' && a[0] != 'y')) {
        printf("Deletion cancelled.\n");
        return 1;
    }

    for (int j = i + 1; j < recordCount; j++)
        records[j - 1] = records[j];

    recordCount--;
    printf("Record with ID %d deleted.\n", id);
    return 0;
}

/* ============================================================
   SAVE
   ============================================================ */
int saveDatabase(const char *path) {
    FILE *fp = fopen(path, "w");
    if (!fp) {
        perror("SAVE");
        return -1;
    }

    fprintf(fp, "ID\tName\tProgramme\tMark\n");

    for (int i = 0; i < recordCount; i++) {
        fprintf(fp, "%d\t%s\t%s\t%.1f\n",
                records[i].id,
                records[i].name,
                records[i].programme,
                records[i].mark);
    }

    fclose(fp);
    return 0;
}

int exportToCSV(const char *csvPath) {
    FILE *csvFile = fopen(csvPath, "w");
    if (!csvFile) {
        perror("Failed to create CSV file");
        return -1;
    }

    // Write CSV header
    fprintf(csvFile, "ID,Name,Programme,Mark,Grade\n");

    // Write each record
    size_t n = db_count();
    for (size_t i = 0; i < n; i++) {
        const Student *s = db_get(i);
        
        // Use quotes around fields that might contain commas/spaces
        fprintf(csvFile, "%d,\"%s\",\"%s\",%.1f,%c\n", 
                s->id, s->name, s->programme, s->mark, s->grade);
    }

    fclose(csvFile);
    printf("Successfully exported %zu records to '%s'\n", n, csvPath);
    return 0;
}

/* ============================================================
   ACCESSORS
   ============================================================ */
size_t db_count(void) {
    return recordCount;
}

Student* db_get(size_t idx) {
    return (idx < recordCount) ? &records[idx] : NULL;
}

/* ============================================================
   SHOW ALL
   ============================================================ */
void showAll(){
    // Display all records in student records
    // Uses database accessors so formatting stays here
    size_t n = db_count();
    puts("Here are all the records found in the table StudentRecords.");
    puts("---------------------------------------------------------------------------------------------------");
    puts("|  ID\t\t|  Name\t\t\t|  Programme\t\t\t|  Mark \t|  Grade  |");
    puts("---------------------------------------------------------------------------------------------------");
    for (size_t i = 0; i < n; ++i) {
        const Student *s = db_get(i);
        printf("|  %-11d\t|  %-20s\t|  %-22s\t|  %.1f \t|    %c    |\n", s->id, s->name, s->programme, s->mark, s->grade);
    }
    puts("---------------------------------------------------------------------------------------------------");
}

//--- GRADE DISTRIBUTION FEATURE ---
void showGradeDistribution() {
    size_t n = db_count();
    if (n == 0) {
        return;
    }

    const char *gradeLabels[] = {"A", "B", "C", "D", "F"};
    const char *gradeRanges[] = {"(80-100)", "(70-79) ", "(60-69) ", "(50-59) ", "(0-49)  "};

    // Initialize counters
    int gradeCounts[5] = {0};
    
    // Count grades
    for (size_t i = 0; i < n; i++) {
        const Student *s = db_get(i);

        switch (s->grade) {
            case 'A': gradeCounts[0]++; break;
            case 'B': gradeCounts[1]++; break;
            case 'C': gradeCounts[2]++; break;
            case 'D': gradeCounts[3]++; break;
            case 'F': gradeCounts[4]++; break;
        }
    }

    printf("\n------------------------------------------------------");
    printf("\n|                 Grade Distribution                 |");
    printf("\n------------------------------------------------------\n");
    for (int i = 0; i < 5; i++) {
        float percentage = (gradeCounts[i] * 100.0) / n;
        printf("Grade %s %s: %2d students (%.1f%%)\t", 
               gradeLabels[i], gradeRanges[i], gradeCounts[i], percentage);
        
        // Simple bar chart
        int bars = (int)(percentage / 5);  // Each bar = 5%
        for (int j = 0; j < bars; j++) {
            printf("\xDB");
        }
        printf("\n\n");
    }
}

/* ============================================================
   Summary statistics
   ============================================================ */
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

    printf("\n------------------------------------------------------");
    printf("\n|---------------- Summary Statistics ----------------|");
    printf("\n------------------------------------------------------\n");
    printf("Total number of students: %d\n", recordCount);
    printf("Average mark\t: %.2f\n", total_mark / recordCount);
    printf("Highest mark\t: %.2f (Student: %s)\n", highest_mark, records[highest_idx].name);
    printf("Lowest mark\t: %.2f (Student: %s)\n", lowest_mark, records[lowest_idx].name);

    showGradeDistribution();

    printf("------------------------------------------------------\n");
}

/* ============================================================
   Sorting Feature
   ============================================================ */
void sortStudents(SortField field, int ascending) {
    size_t n = db_count();
    
    for (size_t i = 0; i < n - 1; i++) {
        for (size_t j = 0; j < n - i - 1; j++) {
            Student *s1 = db_get(j);
            Student *s2 = db_get(j + 1);

            int shouldSwap = 0;

            switch (field) {
                case SORT_ID:
                    shouldSwap = ascending ? (s1->id > s2->id) : (s1->id < s2->id);
                    break;
                case SORT_MARK:
                    shouldSwap = ascending ? (s1->mark > s2->mark) : (s1->mark < s2->mark);
                    break;
                case SORT_GRADE:
                    if (s1->grade != s2->grade) {
                        shouldSwap = ascending ? (s1->grade > s2->grade) : (s1->grade < s2->grade);
                    } else { // If grades are equal, sort by mark as tiebreaker
                        shouldSwap = ascending ? (s1->mark > s2->mark) : (s1->mark < s2->mark);
                    }
                    break;
                case SORT_NAME:
                    shouldSwap = ascending ? (strcmp(s1->name, s2->name) > 0) : (strcmp(s1->name, s2->name) < 0);
                    break;
                case SORT_PROGRAMME:
                    shouldSwap = ascending ? (strcmp(s1->programme, s2->programme) > 0) : (strcmp(s1->programme, s2->programme) < 0);
                    break;
                default:
                    shouldSwap = 0; // No swap
            }

            if (shouldSwap) {
                // Swap the two records
                Student temp = *s1;
                *s1 = *s2;
                *s2 = temp;
            }
        }
    }
}