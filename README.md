# C-Group-Project
This is a C program that takes in a student records database file, and allows users to perform various operations to the records. Our Core operations are:
- OPEN  #Opens a database file
- SHOW ALL  #Shows all current student records
- INSERT  #Inserts a new record into the database
- QUERY  #Searches the database for a record based on student ID
- UPDATE  #Updates an existing record
- DELETE  #Deletes an existing record
- SAVE  #Saves the database into a file

Our enhanced and additional features:
- Enhanced Query
- Enhanced Update
- Enhanced Show All summary
- New Grade column
- Export as CSV
- HELP command

To compile the file file:
gcc -I include src/main.c src/database.c -o build/cms.exe
