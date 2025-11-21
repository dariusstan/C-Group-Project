#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "database.h"

Student records[MAX_RECORDS];
int recordCount = 0;

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
    FILE *fp = fopen(path, "r");
    if (!fp) {
        perror("OPEN");
        return -1;
    }

    char line[512];

    /* skip to header */
    while (fgets(line, sizeof line, fp)) {
        const char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (strncmp(p, "ID", 2) == 0)
            break;
    }

    recordCount = 0;

    while (recordCount < MAX_RECORDS && fgets(line, sizeof line, fp)) {
        const char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (!isdigit((unsigned char)*p))
            continue;

        Student s;
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

/* ============================================================
   UPDATE
   ============================================================ */
void updateRecord(int id) {
    int index = queryRecord(id);
    if (index == -1)
        return;

    char buf[128];

    /* name */
    printf("\nEnter new name: ");
    if (!fgets(buf, sizeof buf, stdin)) return;
    buf[strcspn(buf, "\n")] = '\0';

    if (!isValidName(buf)) {
        return;
    }
    strcpy(records[index].name, buf);

    /* programme */
    printf("Enter new programme (CS/CE/EE): ");
    if (!fgets(buf, sizeof buf, stdin)) return;
    buf[strcspn(buf, "\n")] = '\0';

    if (!isValidProgramme(buf)) {
        return;
    }
    strcpy(records[index].programme, buf);

    /* mark */
    printf("Enter new mark (1-100): ");
    if (!fgets(buf, sizeof buf, stdin)) return;
    buf[strcspn(buf, "\n")] = '\0';

    if (!isNumericString(buf)) {
        printf("Invalid mark. Must be numeric.\n");
        return;
    }

    float newMark = atof(buf);

    /* only one error message */
    if (!isValidMark(newMark))
        return;

    records[index].mark = newMark;
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

    Student s;
    char buf[128];

    /* ID */
    printf("\nEnter new student ID (7 digits): ");
    if (!fgets(buf, sizeof buf, stdin)) return;
    buf[strcspn(buf, "\n")] = '\0';

    if (strlen(buf) != 7 || strspn(buf, "0123456789") != 7) {
        printf("Invalid ID. Must be exactly 7 digits.\n");
        return;
    }

    s.id = atoi(buf);

    for (int i = 0; i < recordCount; i++) {
        if (records[i].id == s.id) {
            printf("Error: ID already exists.\n");
            return;
        }
    }

    /* name */
    printf("Enter name: ");
    if (!fgets(s.name, sizeof s.name, stdin)) return;
    s.name[strcspn(s.name, "\n")] = '\0';

    if (!isValidName(s.name)) return;

    /* programme */
    printf("Enter programme (CS/CE/EE): ");
    if (!fgets(s.programme, sizeof s.programme, stdin)) return;
    s.programme[strcspn(s.programme, "\n")] = '\0';

    if (!isValidProgramme(s.programme)) return;

    /* mark */
    printf("Enter mark (1-100): ");
    if (!fgets(buf, sizeof buf, stdin)) return;
    buf[strcspn(buf, "\n")] = '\0';

    if (!isNumericString(buf)) {
        printf("Invalid mark. Must be numeric.\n");
        return;
    }

    float mark = atof(buf);

    /* only one error message */
    if (!isValidMark(mark))
        return;

    s.mark = mark;

    printf("[DEBUG] Storing mark = %.2f\n", s.mark);

    records[recordCount++] = s;
    printf("\nRecord inserted successfully!\n");
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

/* ============================================================
   ACCESSORS
   ============================================================ */
size_t db_count(void) {
    return recordCount;
}

const Student* db_get(size_t idx) {
    return (idx < recordCount) ? &records[idx] : NULL;
}

/* ============================================================
   SUMMARY
   ============================================================ */
void showSummary() {
    if (recordCount == 0) {
        printf("No records in the database.\n");
        return;
    }

    float total = 0;
    float highest = records[0].mark;
    float lowest = records[0].mark;
    int idxHigh = 0;
    int idxLow = 0;

    for (int i = 0; i < recordCount; i++) {
        total += records[i].mark;

        if (records[i].mark > highest) {
            highest = records[i].mark;
            idxHigh = i;
        }
        if (records[i].mark < lowest) {
            lowest = records[i].mark;
            idxLow = i;
        }
    }

    printf("\n----- Summary Statistics -----\n");
    printf("Total students: %d\n", recordCount);
    printf("Average mark: %.2f\n", total / recordCount);
    printf("Highest mark: %.2f (Student: %s)\n", highest, records[idxHigh].name);
    printf("Lowest mark: %.2f (Student: %s)\n", lowest, records[idxLow].name);
    printf("------------------------------\n\n");
}

