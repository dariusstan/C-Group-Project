#ifndef DATABASE_H  
#define DATABASE_H  // header used to declare shared types and functions
#define MAX_RECORDS 100  // maximum number of student records
#define MAX_STR_LEN 50  // maximum length of strings

#include <stddef.h>

// Student record structure
typedef struct {
    int id;
    char name[MAX_STR_LEN];
    char programme[MAX_STR_LEN];
    float mark;
    char grade;
} Student;

// Command types enumeration
typedef enum {
    CMD_HELP,
    CMD_OPEN,
    CMD_SHOW_ALL,
    CMD_QUERY,
    CMD_UPDATE,
    CMD_INSERT,
    CMD_DELETE,
    CMD_SAVE,
    CMD_EXIT,
    CMD_SUMMARY,
    CMD_EXPORT,
    CMD_UNKNOWN
} CommandType;

// Sort field enumeration
typedef enum {
    SORT_ID,
    SORT_MARK,
    SORT_GRADE,
    SORT_NAME,
    SORT_PROGRAMME,
} SortField;

// Internal storage owned by database.c
extern Student records[MAX_RECORDS];
extern int recordCount;

// Existing operations
int openDatabase(const char *path);  // open database function
int queryRecord(int id);  // query record function
int deleteRecord(int id);  // delete record function
int saveDatabase(const char *path);  // save database function
int isValidProgramme(const char* programme);  // validate programme
int exportToCSV(const char *path);  // export to CSV
void showAll();  // show all records function
void updateRecord(int id);  // update record function
void insertRecord(void);   // insert record function
void showSummary();  // show summary statistics
void sortStudents(SortField field, int ascending);  // sorting function
void clean_path(const char *input, char *output, size_t out_size);  // clean path utility
const char *fullProgramme(const char *code);  // convert to full programme name

// Read-only accessors for iteration (preferred public API)
size_t db_count(void);
Student* db_get(size_t idx);

#endif