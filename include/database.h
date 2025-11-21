#ifndef DATABASE_H
#define DATABASE_H  // header used to declare shared types and functions

#include <stddef.h>

#define MAX_RECORDS 100
#define MAX_STR_LEN 50

// Student record structure
typedef struct {
    int id;
    char name[MAX_STR_LEN];
    char programme[MAX_STR_LEN];
    float mark;
    char grade;
} Student;

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
void loadSampleData(void); 
int queryRecord(int id);
void updateRecord(int id);
void insertRecord(void);
int openDatabase(const char *path);
int deleteRecord(int id);
int saveDatabase(const char *path);
int isValidProgramme(const char* programme);
void showAll();
void showSummary();
int exportToCSV(const char *path);
void sortStudents(SortField field, int ascending);

// Read-only accessors for iteration (preferred public API)
size_t db_count(void);
Student* db_get(size_t idx);

#endif