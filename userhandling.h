#ifndef USERHANDLING_H
#define USERHANDLING_H
#include "basefile.h"
#include <stdint.h>

//definitions
void generateId(HashTable * table, char * id);
User * createUser(char * userName, char * password, char * id, HashTable * hashTable);
User * searchUser(HashTable * table, char * username);
void insertUser(HashTable * table, User * user);
void printAllUsers(HashTable * hashTable);
User * registerUser(char * username, char * password, HashTable * hashTable, char * filename);
void fileSaveUsers(HashTable * hashTable, char * filename);
void fileAddUser(HashTable * hashTable, char * filename, User * current);
HashTable * fileReadAllUsers(char * filename);
User * searchUserById(HashTable * table, char * id);

#endif
