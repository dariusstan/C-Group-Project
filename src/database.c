#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "database.h"

// =====================================================
// STORAGE
// =====================================================
Student records[MAX_RECORDS];
int recordCount = 0;

// =====================================================
// Helper: Calculate Grade
// =====================================================
char getGrade(float mark) {
    if (mark >= 80) return 'A';
    if (mark >= 70) return 'B';
    if (mark >= 60) return 'C';
    if (mark >= 40) return 'D';
    return 'F';
}

// =====================================================
// Helper: Validate Name
// =====================================================
int isValidName(const char *name) {
    if (!name || name[0] == '\0') {
        printf("Invalid name. Name cannot be empty.\n");
        return 0;
    }
    for (int i = 0; name[i]; i++) {
        if (!isalpha((unsigned char)name[i]) && name[i] != ' ') {
            printf("Invalid name. Only letters and spaces allowed.\n");
            return 0;
        }
    }
    return 1;
}

// =====================================================
// Helper: Validate Programme
// =====================================================
int isValidProgramme(const char *programme) {
    const char *valid[] = {"CS", "CE", "EE"};
    for (int i = 0; i < 3; i++) {
        if (strcmp(programme, valid[i]) == 0)
            return 1;
    }
    printf("Invalid programme. Use CS, CE, or EE.\n");
    return 0;
}

// =====================================================
// Helper: Validate Numeric String (Mark)
// =====================================================
int isNumericString(const char *s) {
    if (!s || s[0] == '\0') return 0;
    int dotCount = 0;
    for (int i = 0; s[i]; i++) {
        if (isdigit((unsigned char)s[i])) continue;
        if (s[i] == '.') {
            dotCount++;
            if (dotCount > 1) return 0;
        } else {
            return 0;
        }
    }
    return 1;
}

// =====================================================
// Helper: Validate Mark Range
// =====================================================
int isValidMark(float mark) {
    if (mark < 1.0f || mark > 100.0f) {
        printf("Invalid mark. Must be between 1 and 100.\n");
        return 0;
    }
    return 1;
}

// =====================================================
// OPEN DATABASE (.txt)
// =====================================================
int openDatabase(const char *path) {
    char clean[512], line[512];
    snprintf(clean, sizeof(clean), "%s", path);

    // Remove surrounding quotes
    size_t len = strlen(clean);
    if (len > 1 && ((clean[0] == '"' || clean[0] == '\'') && clean[len - 1] == clean[0])) {
        clean[len - 1] = '\0';
        memmove(clean, clean + 1, len);
    }

    FILE *fp = fopen(clean, "r");
    if (!fp) return -1;

    // Skip header until "ID"
    while (fgets(line, sizeof(line), fp)) {
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (strncmp(p, "ID", 2) == 0) break;
    }

    recordCount = 0;
    while (recordCount < MAX_RECORDS && fgets(line, sizeof(line), fp)) {
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (!isdigit((unsigned char)*p)) continue;

        Student s;
        if (sscanf(line, "%d\t%49[^\t]\t%49[^\t]\t%f",
                   &s.id, s.name, s.programme, &s.mark) == 4) {

            s.grade = getGrade(s.mark);
            records[recordCount++] = s;
        }
    }

    fclose(fp);
    return 0;
}

// =====================================================
// QUERY
// =====================================================
int queryRecord(int id) {
    for (int i = 0; i < recordCount; i++) {
        if (records[i].id == id) {
            printf("\nRecord found:\n");
            printf("ID\t: %d\n", records[i].id);
            printf("Name\t: %s\n", records[i].name);
            printf("Programme: %s\n", records[i].programme);
            printf("Mark\t: %.2f\n", records[i].mark);
            printf("Grade\t: %c\n", records[i].grade);
            return i;
        }
    }
    printf("\nNo record found with ID %d.\n", id);
    return -1;
}

// =====================================================
// UPDATE
// =====================================================
void updateRecord(int id) {
    int index = queryRecord(id);
    if (index == -1) return;
    char buffer[64];

    // NAME
    printf("\nEnter new name (Enter = keep): ");
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strcspn(buffer, "\n")] = '\0';

    if (buffer[0] != '\0' && isValidName(buffer))
        strcpy(records[index].name, buffer);

    // PROGRAMME
    printf("Enter new programme (Enter = keep): ");
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strcspn(buffer, "\n")] = '\0';

    if (buffer[0] != '\0' && isValidProgramme(buffer))
        strcpy(records[index].programme, buffer);

    // MARK
    printf("Enter new mark (Enter = keep): ");
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strcspn(buffer, "\n")] = '\0';

    if (buffer[0] != '\0') {
        if (!isNumericString(buffer)) {
            printf("Invalid mark. Must be numeric.\n");
            return;
        }
        float m = atof(buffer);
        if (!isValidMark(m)) return;
        records[index].mark = m;
    }

    records[index].grade = getGrade(records[index].mark);
    printf("\nRecord updated successfully!\n");
}

// =====================================================
// INSERT
// =====================================================
void insertRecord(void) {
    if (recordCount >= MAX_RECORDS) {
        printf("Cannot insert: database full.\n");
        return;
    }

    Student s;
    char buf[64];

    // ID
    printf("\nEnter new student ID (7 digits): ");
    fgets(buf, sizeof(buf), stdin);
    buf[strcspn(buf, "\n")] = '\0';

    if (buf[0] == '\0') { printf("ID cannot be empty.\n"); return; }
    for (int i = 0; buf[i]; i++)
        if (!isdigit((unsigned char)buf[i])) {
            printf("ID must be numeric.\n");
            return;
        }

    s.id = atoi(buf);
    if (s.id < 1000000 || s.id > 9999999) {
        printf("Invalid ID length. Must be 7 digits.\n");
        return;
    }
    for (int i = 0; i < recordCount; i++) {
        if (records[i].id == s.id) {
            printf("ID already exists.\n");
            return;
        }
    }

    // NAME
    printf("Enter name: ");
    fgets(s.name, sizeof(s.name), stdin);
    s.name[strcspn(s.name, "\n")] = '\0';
    if (!isValidName(s.name)) return;

    // PROGRAMME
    printf("Enter programme (CS/CE/EE): ");
    fgets(s.programme, sizeof(s.programme), stdin);
    s.programme[strcspn(s.programme, "\n")] = '\0';
    if (!isValidProgramme(s.programme)) return;

    // MARK
    printf("Enter mark (1-100): ");
    fgets(buf, sizeof(buf), stdin);
    buf[strcspn(buf, "\n")] = '\0';

    if (buf[0] == '\0') { printf("Mark cannot be empty.\n"); return; }
    if (!isNumericString(buf)) { printf("Invalid mark. Must be numeric.\n"); return; }

    float mark = atof(buf);
    if (!isValidMark(mark)) return;

    s.mark = mark;
    s.grade = getGrade(mark);

    records[recordCount++] = s;
    printf("\nRecord inserted successfully!\n");
}

// =====================================================
// DELETE
// =====================================================
int deleteRecord(int id) {
    int i = 0;
    while (i < recordCount && records[i].id != id) i++;

    if (i == recordCount) {
        printf("No record found with ID %d.\n", id);
        return -1;
    }

    printf("Found: ID=%d, Name=%s, Programme=%s, Mark=%.2f\n",
           records[i].id, records[i].name, records[i].programme, records[i].mark);

    char conf[8];
    printf("Confirm delete? (Y/N): ");
    fgets(conf, sizeof(conf), stdin);
    if (conf[0] != 'Y' && conf[0] != 'y') {
        printf("Deletion cancelled.\n");
        return 1;
    }

    for (int j = i + 1; j < recordCount; j++)
        records[j - 1] = records[j];

    recordCount--;
    printf("Record deleted.\n");
    return 0;
}

// =====================================================
// SAVE DATABASE (.txt)
// =====================================================
int saveDatabase(const char *path) {
    FILE *fp = fopen(path, "w");
    if (!fp) { perror("SAVE"); return -1; }

    fprintf(fp, "ID\tName\tProgramme\tMark\n");
    for (int i = 0; i < recordCount; i++)
        fprintf(fp, "%d\t%s\t%s\t%.1f\n",
                records[i].id, records[i].name,
                records[i].programme, records[i].mark);

    fclose(fp);
    return 0;
}

// =====================================================
// EXPORT TO CSV
// =====================================================
int exportToCSV(const char *csvPath) {
    FILE *csv = fopen(csvPath, "w");
    if (!csv) { perror("CSV"); return -1; }

    fprintf(csv, "ID,Name,Programme,Mark,Grade\n");

    for (int i = 0; i < recordCount; i++) {
        fprintf(csv, "%d,\"%s\",\"%s\",%.1f,%c\n",
                records[i].id, records[i].name,
                records[i].programme, records[i].mark,
                records[i].grade);
    }

    fclose(csv);
    return 0;
}

// =====================================================
// ACCESSORS
// =====================================================
size_t db_count(void) { return recordCount; }

Student* db_get(size_t idx) {
    return (idx < recordCount) ? &records[idx] : NULL;
}

// =====================================================
// SHOW ALL (UPDATED ALIGNMENT!)
// =====================================================
void showAll() {
    printf("-----------------------------------------------------------------------------\n");
    printf("%-8s %-20s %-25s %6s %5s\n", "ID", "Name", "Programme", "Mark", "Grade");
    printf("-----------------------------------------------------------------------------\n");
    for (int i = 0; i < recordCount; i++) {
        printf("%-8d %-20s %-25s %6.1f %5c\n",
               records[i].id,
               records[i].name,
               records[i].programme,
               records[i].mark,
               records[i].grade);
    }
    printf("-----------------------------------------------------------------------------\n");
}

// =====================================================
// GRADE DISTRIBUTION
// =====================================================
void showGradeDistribution() {
    if (recordCount == 0) return;

    int countA = 0, countB = 0, countC = 0, countD = 0, countF = 0;

    for (int i = 0; i < recordCount; i++) {
        switch (records[i].grade) {
            case 'A': countA++; break;
            case 'B': countB++; break;
            case 'C': countC++; break;
            case 'D': countD++; break;
            case 'F': countF++; break;
        }
    }

    printf("\nGrade Distribution:\n");
    printf("A: %d\n", countA);
    printf("B: %d\n", countB);
    printf("C: %d\n", countC);
    printf("D: %d\n", countD);
    printf("F: %d\n", countF);
}

// =====================================================
// SUMMARY
// =====================================================
void showSummary() {
    if (recordCount == 0) {
        printf("No records available.\n");
        return;
    }

    float total = 0;
    float high = records[0].mark;
    float low = records[0].mark;
    int idxH = 0, idxL = 0;

    for (int i = 0; i < recordCount; i++) {
        total += records[i].mark;

        if (records[i].mark > high) {
            high = records[i].mark;
            idxH = i;
        }
        if (records[i].mark < low) {
            low = records[i].mark;
            idxL = i;
        }
    }

    printf("\n---------------------------------------\n");
    printf("Summary Statistics\n");
    printf("---------------------------------------\n");
    printf("Total students: %d\n", recordCount);
    printf("Average mark : %.2f\n", total / recordCount);
    printf("Highest mark : %.2f (%s)\n", high, records[idxH].name);
    printf("Lowest mark  : %.2f (%s)\n", low, records[idxL].name);
    printf("---------------------------------------\n");

    showGradeDistribution();
}

// =====================================================
// SORTING
// =====================================================
void sortStudents(SortField field, int ascending) {

    for (int i = 0; i < recordCount - 1; i++) {
        for (int j = 0; j < recordCount - i - 1; j++) {

            Student *s1 = &records[j];
            Student *s2 = &records[j + 1];
            int swap = 0;

            switch (field) {
                case SORT_ID:
                    swap = ascending ? (s1->id > s2->id) : (s1->id < s2->id);
                    break;

                case SORT_MARK:
                    swap = ascending ? (s1->mark > s2->mark) : (s1->mark < s2->mark);
                    break;

                case SORT_GRADE:
                    if (s1->grade == s2->grade)
                        swap = ascending ? (s1->mark > s2->mark) : (s1->mark < s2->mark);
                    else
                        swap = ascending ? (s1->grade > s2->grade) : (s1->grade < s2->grade);
                    break;

                case SORT_NAME:
                    swap = ascending ? (strcmp(s1->name, s2->name) > 0)
                                     : (strcmp(s1->name, s2->name) < 0);
                    break;

                case SORT_PROGRAMME:
                    swap = ascending ? (strcmp(s1->programme, s2->programme) > 0)
                                     : (strcmp(s1->programme, s2->programme) < 0);
                    break;
            }

            if (swap) {
                Student temp = *s1;
                *s1 = *s2;
                *s2 = temp;
            }
        }
    }
}
