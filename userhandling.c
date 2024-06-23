#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "basefile.h"
#include "userhandling.h"
#include "cryptfiles.h"
#include "sha-passhash.h"


void generateId(HashTable *table, char *id) {
    const char charset[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const size_t charsetSize = sizeof(charset) - 1; 

    do {
        for (int i = 0; i < ID_LENGTH - 1; i++) {
            int randomIndex = rand() % charsetSize;
            id[i] = charset[randomIndex];
        }
        id[ID_LENGTH - 1] = '\0';  
    } while (searchUserById(table, id) != NULL);
}


//inits a new user. Username and pass - should be defined, ID could be NULL 
User * createUser(char * userName, char * password, char * id, HashTable * hashTable) {
    if (userName == NULL) {
        return NULL;
    }
    User *newUser = (User *)malloc(sizeof(User));
    strcpy(newUser->userName, userName);
    strcpy(newUser->password, password);
    if (id == "" || id == NULL) {
        char newid[5];
        generateId(hashTable, newid);
        strcpy(newUser->id, newid);
    } else {
        strcpy(newUser->id, id);
    }
    return newUser;
}



//searchUser works with hashset and searches in it by username
User * searchUser(HashTable * table, char * username) {
    unsigned int index = username[0] - 'A';
    BucketNode *current = table->buckets[index];
    while (current != NULL) {
        if (strcmp(current->user->userName, username) == 0) {
            return current->user;
        } else if (strcmp(current->user->userName, username) > 0) {
            return NULL;
        }
        current = current->next;
    }
    return NULL;
}

//searchId works with hashset and searches in it by ID
User * searchUserById(HashTable *table, char * id) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        BucketNode *current = table->buckets[i];
        while (current != NULL) {
            if (strcmp(current->user->id, id) == 0) {
                return current->user;
            }
            current = current->next;
        }
    }
    return NULL;
}

//insertUser works with hashset and inserts properly a new user in it
void insertUser(HashTable * table, User * user) {
    unsigned int index = user->userName[0] - 'A';
    BucketNode * newNode = (BucketNode * )malloc(sizeof(BucketNode));
    newNode->user = user;
    newNode->next = NULL;

    if (table->buckets[index] == NULL || strcmp(table->buckets[index]->user->userName, user->userName) > 0) {
        newNode->next = table->buckets[index];
        table->buckets[index] = newNode;
    } else {
        BucketNode * prev = table->buckets[index];
        BucketNode * current = prev->next;
        while (current != NULL && strcmp(current->user->userName, user->userName) < 0) {
            prev = current;
            current = current->next;
        }
        prev->next = newNode;
        newNode->next = current;
    }
}

//prints all users. Needed only for code purposes, could delete later (prob wont tho)
void printAllUsers(HashTable * hashTable){
    printf("All Users:\n");
    for (int i = 0; i < TABLE_SIZE; i++) {
        BucketNode * current = hashTable->buckets[i];
        for(int depth = 0; current != NULL; depth++) {
            printf("Bucket Index: %d; Depth: %d;  UserName: %s; ID: %s; Password: %s\n", i, depth, current->user->userName, current->user->id, current->user->password);
            current = current->next;
        }
    }
}

//Functions for menu
User * registerUser(char * username, char * password, HashTable * hashTable, char * filename){
    if(username != NULL && searchUser(hashTable, username) != NULL) {
        printf("Error: Cannot save user with existing name or username undefined - %s \n", username);
        return NULL;
    }
    char pass[97];
    preppass(password, pass);
    User * newuser = createUser(username, pass, NULL, hashTable);
    fileAddUser(hashTable, filename, newuser);
    insertUser(hashTable, newuser);
    return newuser;
}

//Working with files - r, w, a
void fileSaveUsers(HashTable * hashTable, char * filename){
    FILE * fptr;
    fptr = fopen(filename, "wb");
    if (fptr == NULL) {
        printf("Error: Cannot create file %s\n", filename);
        return;
    }
    printf("Saving all Users...\n");
    for (int i = 0; i < TABLE_SIZE; i++) {
        BucketNode * current = hashTable->buckets[i];
        for(int depth = 0; current != NULL; depth++) {
            char toBeEncrypted[MAXREAD];
            char encrypted[MAXREAD];
            snprintf(toBeEncrypted, MAXREAD, "%s|%s|%s", current->user->id, current->user->userName, current->user->password);
            vigenereTable(toBeEncrypted, encrypted);
            fprintf(fptr, "%s\n", encrypted);
            current = current->next;
        }
    }
    fclose(fptr);
}
void fileAddUser(HashTable * hashTable, char * filename, User * current){
    FILE * fptr;
    fptr = fopen(filename, "a");
    if (fptr == NULL) {
        fptr = fopen(filename, "w");
        if (fptr == NULL) {
            printf("Error: Cannot create file %s\n", filename);
            return;
        }
    }
    char toBeEncrypted[MAXREAD];
    char encrypted[MAXREAD];
    int offset = 0;
    offset += snprintf(toBeEncrypted + offset, MAXREAD - offset, "%s|%s|%s", current->id, current->userName, current->password);
    vigenereTable(toBeEncrypted, encrypted);
    fprintf(fptr, "%s\n", encrypted);

    fclose(fptr);
}
HashTable *fileReadAllUsers(char *filename) {
    HashTable *hashTable = createHashTable();
    FILE *fptr;
    fptr = fopen(filename, "r");
    if (fptr == NULL) {
        printf("Error: Cannot open file %s\n", filename);
        return NULL;
    }

    char toBeDecrypted[MAXREAD];
    char myString[MAXREAD];

    while (fgets(toBeDecrypted, MAXREAD, fptr)) {
        decodeVigenere(toBeDecrypted, myString);
        char id[5];
        char name[50] = {0};
        char pass[97] = {0}; //32 by 3 digits (from 8 bits) + \0
        char *token;

        token = strtok(myString, "|");
        if (token != NULL) strncpy(id, token, sizeof(id) - 1);
        token = strtok(NULL, "|");
        if (token != NULL) strncpy(name, token, sizeof(name) - 1);
        token = strtok(NULL, "\n");
        if (token != NULL) strncpy(pass, token, sizeof(pass) - 1);  
        User *newuser = createUser(name, pass, id, hashTable); 
        insertUser(hashTable, newuser);
    }

    fclose(fptr);
    return hashTable;
}
