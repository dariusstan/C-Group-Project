#ifndef DATABASE_H
#define DATABASE_H  // header used to declare shared types and functions

#include <stddef.h>

#define MAX_RECORDS 100
#define MAX_STR_LEN 50

typedef struct {
    int id;
    char name[MAX_STR_LEN];
    char programme[MAX_STR_LEN];
    float mark;
} Student;

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

// Read-only accessors for iteration (preferred public API)
size_t db_count(void);
const Student* db_get(size_t idx);

#endif
