#ifndef USERHANDLING_H
#define USERHANDLING_H
#include "basefile.h"
#include <stdint.h>

//definitions
char generateId();
User * createUser(char * userName, char * password, int * id, HashTable * hashTable);
User * searchUser(HashTable * table, char * username);
void insertUser(HashTable * table, User * user);
void printAllUsers(HashTable * hashTable);
void registerUser(char * username, uint8_t * password, HashTable * hashTable, char * filename);
void fileSaveUsers(HashTable * hashTable, char * filename);
void fileAddUser(HashTable * hashTable, char * filename, User * current);
HashTable * fileReadAllUsers(char * filename);
User * searchUserById(HashTable * table, char id);

#endif
