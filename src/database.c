#include <stdio.h>  // for FILE, fopen, fclose, fgets, fprintf, perror
#include <string.h>  // for strlen, strcmp, strcpy, strncpy, strncmp, sscanf, memmove
#include <ctype.h>  // for isdigit, isalpha, toupper
#include <stdlib.h>  // for atof
#include "database.h"  // for function prototypes, Student struct, constants

// =====================================================
// STORAGE
// =====================================================
Student records[MAX_RECORDS];  // default 100 records
int recordCount = 0;  //record counter 

// =====================================================
// Helper: Cleans user's input path
// =====================================================
void clean_path(const char *input, char *output, size_t out_size) {
    // if invalid input or output or zero size, exit
    if (!input || !output || out_size == 0) return;

    // safely copies input 
    snprintf(output, out_size, "%s", input);
    
    size_t len = strlen(output);
    // removes surrounding quotation marks " or ' if present
    if (len > 1 && ((output[0] == '"' || output[0] == '\'') && output[len - 1] == output[0])) {
        output[len - 1] = '\0';           // replaces last quote with null terminator
        memmove(output, output + 1, len); // shift left to remove first quote
    }
}

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
    if (!name || name[0] == '\0') { //if empty input is detected
        printf("Invalid name. Name cannot be empty.\n");
        return 0;
    }
    for (int i = 0; name[i]; i++) {
        if (!isalpha((unsigned char)name[i]) && name[i] != ' ') { //if anything else other than letters and spaces are detected
            printf("Invalid name. Only letters and spaces allowed.\n");
            return 0;
        }
    }
    return 1; //passed all checks
}

// =====================================================
// Helper: Validate Programme
// =====================================================
int isValidProgramme(const char *programme) {
    const char *valid[] = {"CS", "CE", "EE", "AI", "DSC"};
    // Check against valid programmes
    for (int i = 0; i < 5; i++) {
        if (strcmp(programme, valid[i]) == 0)
            return 1;
    }
    printf("Invalid programme. Use CS, CE, EE, AI, or DSC and the field cannot be empty.\n");
    return 0;
}

// =====================================================
// Helper: get full programme name from short form
// =====================================================
typedef struct {
    const char *code;  //short programme code like "CS"
    const char *full;  //full programme name like "Computer Science"
} Programme_map;

// programme mapping table
static const Programme_map programmeTable[] = {
    {"CS", "Computer Science"},
    {"CE", "Computer Engineering"},
    {"EE", "Electrical Engineering"},
    {"AI", "Artificial Intelligence"},
    {"DSC","Digital Supply Chain" },
};
static const int programmeCount = 5;

const char *fullProgramme(const char *code) {
    // loops through all items in mapping table, compares user input until match found, then returns full name
    for (int i = 0; i < programmeCount; i++) {
        if (strcmp(code, programmeTable[i].code) == 0) {
            return programmeTable[i].full;
        }
    }
    return code; // technically shouldn't reach here since validation done before this
}

// =====================================================
// Helper: Validate Numeric String (Mark)
// =====================================================
int isNumericString(const char *s) {
    if (!s || s[0] == '\0') return 0; //reject empty input
    int dotCount = 0; //track how many decimals there are 
    for (int i = 0; s[i]; i++) {
        if (isdigit((unsigned char)s[i])) continue; //if the input is from 0-9, continue
        if (s[i] == '.') { //only allow one decimal point
            dotCount++;
            if (dotCount > 1) return 0; //reject numbers with multiple decimal points
        } else { //any other characters will be invalid
            return 0;
        }
    }
    return 1;
}

// =====================================================
// Helper: Validate Mark Range
// =====================================================
int isValidMark(float mark) {
    if (mark < 1.0f || mark > 100.0f) { //make sure its from 1 to 100 only
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
    clean_path(path, clean, sizeof clean); //remove surrounding quotes or extra spaces

    FILE *fp = fopen(clean, "r");
    if (!fp) return -1; //return -1 if the file cant be opened

    // Skip header until "ID"
    while (fgets(line, sizeof(line), fp)) {
        char *p = line;
        while (*p == ' ' || *p == '\t') p++; //skip leading spaces or tabs
        if (strncmp(p, "ID", 2) == 0) break; //stop when a line starting with ID is found
    }

    recordCount = 0;
    while (recordCount < MAX_RECORDS && fgets(line, sizeof(line), fp)) { //read each student record line by line
        char *p = line;
        while (*p == ' ' || *p == '\t') p++; //skip whitespace at the start
        if (!isdigit((unsigned char)*p)) continue; //if the line does not start with a digit, skip it

        Student s;
        if (sscanf(line, "%d\t%49[^\t]\t%49[^\t]\t%f", &s.id, s.name, s.programme, &s.mark) == 4) {
            //calls fullProgramme to convert short code to full name
            strcpy(s.programme, fullProgramme(s.programme));
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
    for (int i = 0; i < recordCount; i++) { //prints the data associated with the ID
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
    int index = queryRecord(id); //checks if the student exists, queryRecord() will print if it does and return its index. if ID is not found, index = -1
    if (index == -1) return;
    char buffer[64];

    // NAME
    printf("\nEnter new name (Enter to keep. Letters and spaces only allowed): ");
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strcspn(buffer, "\n")] = '\0'; //remove newline characters

    if (buffer[0] != '\0' && isValidName(buffer)) //only update if theres an input, empty = keep old record
        strcpy(records[index].name, buffer);

    // PROGRAMME
    printf("Enter new programme (Enter to keep. CS,CE,EE,AI,DSC only allowed): ");
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strcspn(buffer, "\n")] = '\0';

    if (buffer[0] != '\0' && isValidProgramme(buffer)) { //update if user entered something and programme is valid
    strcpy(records[index].programme, fullProgramme(buffer)); //fullProgramme() converts 2-3 letter code to its full name
    }

    // MARK
    printf("Enter new mark (Enter to keep. Within 1-100 only): ");
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strcspn(buffer, "\n")] = '\0';

    if (buffer[0] != '\0') {
        if (!isNumericString(buffer)) {
            printf("Invalid mark. Must be numeric.\n");
            return;
        }
        float m = atof(buffer); //convert string into float 
        if (!isValidMark(m)) return; //check if its from 1-100 range
        records[index].mark = m;
    }

    records[index].grade = getGrade(records[index].mark); //recalculate grade after mark update
    printf("\nRecord updated successfully!\n");
}

// =====================================================
// INSERT
// =====================================================
void insertRecord(void) {
    if (recordCount >= MAX_RECORDS) { //stop insertion if database is full
        printf("Cannot insert: database full.\n");
        return;
    }

    Student s; //temporary student record used to store all new input 
    char buf[64]; //input buffer for reading strings like ID and mark

    // ID
    printf("\nEnter new student ID (7 digits): ");
    fgets(buf, sizeof(buf), stdin);
    buf[strcspn(buf, "\n")] = '\0'; //remove newline

    if (buf[0] == '\0') { printf("ID cannot be empty.\n"); return; } //empty input check
    for (int i = 0; buf[i]; i++) //ensure ID contains numeric input only
        if (!isdigit((unsigned char)buf[i])) {
            printf("ID must be numeric.\n");
            return;
        }

    s.id = atoi(buf); //convert string to integer once validated
    if (s.id < 1000000 || s.id > 9999999) { //check that ID is exactly 7 digits and cannot start with 0
        printf("Invalid ID length. Must be 7 digits and cannot start with 0.\n");
        return;
    }
    for (int i = 0; i < recordCount; i++) { //prevent duplicate IDs
        if (records[i].id == s.id) {
            printf("ID already exists.\n");
            return;
        }
    }

    // NAME
    printf("Enter name: ");
    fgets(s.name, sizeof(s.name), stdin);
    s.name[strcspn(s.name, "\n")] = '\0';
    if (!isValidName(s.name)) return; //validates that name contains letters and spaces plus its not empty

    // PROGRAMME
    printf("Enter programme (CS/CE/EE/AI/DSC): ");
    fgets(s.programme, sizeof(s.programme), stdin);
    s.programme[strcspn(s.programme, "\n")] = '\0';
    if (!isValidProgramme(s.programme)) return; //have to match one of the allowed codes

    // MARK
    printf("Enter mark (1-100): ");
    fgets(buf, sizeof(buf), stdin);
    buf[strcspn(buf, "\n")] = '\0';

    if (buf[0] == '\0') { printf("Mark cannot be empty.\n"); return; } //empty check 
    if (!isNumericString(buf)) { printf("Invalid mark. Must be numeric.\n"); return; } //reject letters/symbols

    float mark = atof(buf); //convert string to float 
    if (!isValidMark(mark)) return; //ensure mark is within allowed range which is 1-100
    //calls fullProgramme to convert short code to full name
    strcpy(s.programme, fullProgramme(s.programme));

    s.mark = mark;
    s.grade = getGrade(mark); //calculation of grade

    records[recordCount++] = s; //add to database 
    printf("\nRecord inserted successfully!\n");
}

// =====================================================
// DELETE
// =====================================================
int deleteRecord(int id) {
    int i = 0;
    while (i < recordCount && records[i].id != id) i++; //search for the record with the matching ID 

    if (i == recordCount) { //if the end is reached, ID does not exist 
        printf("No record found with ID %d.\n", id);
        return -1;
    }

    printf("Found: ID=%d, Name=%s, Programme=%s, Mark=%.2f\n", //display record before deleting it 
           records[i].id, records[i].name, records[i].programme, records[i].mark);

    char conf[8];
    printf("Confirm delete? (Y/N): ");
    fgets(conf, sizeof(conf), stdin);
    if (conf[0] != 'Y' && conf[0] != 'y') { //if the user does not type either Y or y, cancel the deletion 
        printf("Deletion cancelled.\n");
        return 1;
    }

    for (int j = i + 1; j < recordCount; j++) //shift all records after this index one step left to close the gap
        records[j - 1] = records[j];

    recordCount--; //reduce count since one record got removed
    printf("Record deleted.\n");
    return 0;
}

// =====================================================
// SAVE DATABASE (.txt)
// =====================================================
    int saveDatabase(const char *path) {
        char clean[512];  //variable to hold cleaned path

        // if path is NULL or empty, return error and exit function
        if (!path || path[0] == '\0') {
            fprintf(stderr, "SAVE: no file path specified\n");
            return -1;
        }

        //calling clean_path function to sanitize the input path
        clean_path(path, clean, sizeof clean);
        FILE *fp = fopen(clean, "w");
        if (!fp) {
            perror("SAVE");
            return -1;
        }

        //saves the header as well as the student records in tab-separated format into the file
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

// =====================================================
// EXPORT TO CSV
// =====================================================
int exportToCSV(const char *csvPath) {
    FILE *csv = fopen(csvPath, "w"); //open the csv file for writing. "w" creates/overwrites the file 
    if (!csv) { perror("CSV"); return -1; } //prints error message if file cannot be opened/created, return -1 to signal failure

    fprintf(csv, "ID,Name,Programme,Mark,Grade\n"); //write the csv header row 
    for (int i = 0; i < recordCount; i++) { //loop through existing records and write each one to the csv file
        fprintf(csv, "%d,\"%s\",\"%s\",%.1f,%c\n",
                records[i].id, records[i].name,
                records[i].programme, records[i].mark,
                records[i].grade);
    } //quotation marks are added around strings to avoid issues if the name contains spaces

    fclose(csv); //close csv file to save changes 
    return 0; //indicate success
}

// =====================================================
// ACCESSORS
// =====================================================
size_t db_count(void) {
     return recordCount; 
    }

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

// =====================================================
// SUMMARY
// =====================================================
void showSummary() {
    if (recordCount == 0) { //if no records, nothing to summarize
        printf("No records available.\n");
        return;
    }

    float total = 0; //total will accumulate all the marks so we can calculate the average 
    float high = records[0].mark;
    float low = records[0].mark; //initialise highest and lowest mark to the first student's mark 
    int idxH = 0, idxL = 0; //idxH stores the index of the student with the highest mark, idxL is for the lowest

    // Calculate total, highest, and lowest marks
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
void sortStudents(SortField field, int ascending) { //repeatedly compare and swap neighbouring elements 
    for (int i = 0; i < recordCount - 1; i++) {
        for (int j = 0; j < recordCount - i - 1; j++) {
            Student *s1 = &records[j]; //pointers to the two student records being compared 
            Student *s2 = &records[j + 1];
            int swap = 0; //flag to determine if a swap should happen
            switch (field) { //decide which field to sort by 
                case SORT_ID:
                    swap = ascending ? (s1->id > s2->id) : (s1->id < s2->id); //compare IDs, if sorting from low to high, check if first ID is bigger than the second. vice versa
                    break;

                case SORT_MARK:
                    swap = ascending ? (s1->mark > s2->mark) : (s1->mark < s2->mark); //compare marks
                    break;

                case SORT_GRADE: 
                    if (s1->grade == s2->grade) //if grades are equal, sort by mark as a secondary rule
                        swap = ascending ? (s1->mark > s2->mark) : (s1->mark < s2->mark);
                    else
                        swap = ascending ? (s1->grade > s2->grade) : (s1->grade < s2->grade);
                    break;

                case SORT_NAME:
                    swap = ascending ? (strcmp(s1->name, s2->name) > 0) //string comparison for alphabetical ordering 
                                     : (strcmp(s1->name, s2->name) < 0);
                    break;

                case SORT_PROGRAMME:
                    swap = ascending ? (strcmp(s1->programme, s2->programme) > 0) //sort alphabetically by programme name
                                     : (strcmp(s1->programme, s2->programme) < 0);
                    break;
            }

            if (swap) { //if swap flag is set, exchange the two records
                Student temp = *s1;
                *s1 = *s2;
                *s2 = temp;
            }
        }
    }
}
