#ifndef DATABASE_H 
#define DATABASE_H //this is the header file and its used to declare functions so we can share them between multiple .c files

#define MAX_RECORDS 100
#define MAX_STR_LEN 50

typedef struct {
    int id;
    char name[MAX_STR_LEN];
    char programme[MAX_STR_LEN];
    float mark;
} Student;

extern Student records[MAX_RECORDS];
extern int recordCount;

void loadSampleData();
int queryRecord(int id);
void updateRecord(int id);
void insertRecord();

#endif
