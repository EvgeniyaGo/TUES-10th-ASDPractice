#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "basefile.h"
#include "userhandling.h"
#include "cryptfiles.h"

char generateId(HashTable * table) {
    char random;
    do{
        random = rand(); 
    } while(searchUserById(table, random) != NULL);
    return random;
}

//inits a new user. Username and pass - should be defined, ID could be NULL 
User * createUser(char * userName, char * password, int * id, HashTable * hashTable) {
    if(userName == "") {
        printf("Error: Cannot create user - username error");
        return NULL;
    }
    User * newUser = (User * )malloc(sizeof(User));
    strcpy(newUser->userName, userName);
    strcpy(newUser->password, password);
    if(id == NULL) newUser->id = generateId(hashTable);
    else newUser->id = * id;
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
User * searchUserById(HashTable * table, char id) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        BucketNode * current = table->buckets[i];
        for(int depth = 0; current != NULL; depth++) {
            if(current->user->id == id){
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
            printf("Bucket Index: %d; Depth: %d;  UserName: %s; ID: %d; ", i, depth, current->user->userName, current->user->id);
            for(int i = 0; i < 96; i++){
                printf("%c", current->user->password[i]);
            }
            printf("\n");
            current = current->next;
        }
    }
}

//Functions for menu
void registerUser(char * username, uint8_t * password, HashTable * hashTable, char * filename){
    if(username != NULL && searchUser(hashTable, username) != NULL) {
        printf("Error: Cannot save user with existing name or username undefined - %s \n", username);
        return;
    }
    printf("Registering\n");
    char passchar[96];
    int passcharind = 0;
    for (int i = 0; i < 32; i++) {
        passcharind += sprintf(passchar + passcharind, "%03d", password[i]);
    }
    User * newuser = createUser(username, passchar, NULL, hashTable);
    printf("  Created user\n");
    fileAddUser(hashTable, filename, newuser);
    printf(" Added to file\n");
    insertUser(hashTable, newuser);
    printf("  Inserted to hashtable\n");
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
            snprintf(toBeEncrypted, MAXREAD, "%d|%s|%s", current->user->id, current->user->userName, current->user->password);
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
    printf("    Opened file\n");
    char toBeEncrypted[MAXREAD];
    char encrypted[MAXREAD];
    int offset = 0;
    printf("    Init completed\n");
    offset += snprintf(toBeEncrypted + offset, MAXREAD - offset, "%d|%s|", current->id, current->userName);
    printf("    Added id and username\n");
    for(int i = 0; i < 96; i++){
        offset += snprintf(toBeEncrypted + offset, MAXREAD - offset, "%c", current->password[i]);
    }
    printf("    Added pass\n");
    printf("    Encrypting\n");
    vigenereTable(toBeEncrypted, encrypted);
    printf("    Encrypted\n");
    fprintf(fptr, "%s\n", encrypted);
    printf("    Saved to file\n");

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
        int id;
        char name[50] = {0};
        char pass[97] = {0}; //32 by 3 digits (from 8 bits) + \0
        char *token;

        token = strtok(myString, "|");
        if (token != NULL) id = atoi(token);
        token = strtok(NULL, "|");
        if (token != NULL) strncpy(name, token, sizeof(name) - 1);
        token = strtok(NULL, "\n");
        if (token != NULL) strncpy(pass, token, sizeof(pass) - 1);  
        User *newuser = createUser(name, pass, &id, hashTable); 
        insertUser(hashTable, newuser);
    }

    fclose(fptr);
    return hashTable;
}
