#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TABLE_SIZE 26

typedef struct User {
    char userName[50];
    char password[20];
    unsigned int id;
} User;

typedef struct BucketNode {
    User * user;
    struct BucketNode * next;
} BucketNode;

typedef struct HashTable { 
    BucketNode * buckets[TABLE_SIZE];
} HashTable;

char generateId() {
    return rand(); 
}

User * createUser(char * userName, char * password, int * id) {
    User * newUser = (User * )malloc(sizeof(User));
    strcpy(newUser->userName, userName);
    strcpy(newUser->password, password);
    if(id == NULL) newUser->id = generateId();
    else newUser->id = *id;
    return newUser;
}

//Working with HashTable - creating, filling, accessing
//I use hashset with buckets for storing and quickly accessing all the users. From 0(1) till 0(n/26)=0(n)
HashTable * createHashTable() {
    HashTable * table = (HashTable * )malloc(sizeof(HashTable));
    for (int i = 0; i < TABLE_SIZE; i++) {
        table->buckets[i] = NULL;
    }
    return table;
}
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
User * searchUser(HashTable *table, char *userName) {
    unsigned int index = userName[0] - 'A';
    BucketNode *current = table->buckets[index];
    while (current != NULL) {
        if (strcmp(current->user->userName, userName) == 0) {
            return current->user;
        } else if (strcmp(current->user->userName, userName) > 0) {
            return NULL;
        }
        current = current->next;
    }
    return NULL;
}

//Working with files - r, w, a
void fileSaveUsers(HashTable * hashTable, char * filename){
    FILE * fptr;
    fptr = fopen(filename, "wb");
    printf("Saving all Users...\n");
    for (int i = 0; i < TABLE_SIZE; i++) {
        BucketNode * current = hashTable->buckets[i];
        for(int depth = 0; current != NULL; depth++) {
            fprintf(fptr, "%d|%s|%s\n", current->user->id, current->user->userName, current->user->password);
            current = current->next;
        }
    }
    fclose(fptr);
}
void fileAddUser(HashTable * hashTable, char * filename, User * current){
    if(searchUser(hashTable, current->userName) != NULL) {
        printf("Error: Cannot save user with existing name");
        return;
    }
    FILE * fptr;
    fptr = fopen(filename, "a");
    fprintf(fptr, "%d|%s|%s\n", current->id, current->userName, current->password);
    fclose(fptr);
}
HashTable * fileReadAllUsers(char * filename){
    HashTable * hashTable = createHashTable();
    FILE * fptr;
    fptr = fopen(filename, "r");
    char myString[200];
    int i = 0;
    while(fgets(myString, 200, fptr)) {
        int id;
        char name[50] = {0};
        char pass[50] = {0};
        char *token;
        token = strtok(myString, "|");
        if (token != NULL) id = atoi(token);
        token = strtok(NULL, "|");
        if (token != NULL) strncpy(name, token, sizeof(name) - 1);
        token = strtok(NULL, "\n");
        if (token != NULL) strncpy(pass, token, sizeof(pass) - 1);
        User * newuser = createUser(name, pass, &id);
        insertUser(hashTable, newuser);
    }
    fclose(fptr);
    return hashTable;
}

void printAllUsers(HashTable * hashTable){
    printf("All Users:\n");
    for (int i = 0; i < TABLE_SIZE; i++) {
        BucketNode * current = hashTable->buckets[i];
        for(int depth = 0; current != NULL; depth++) {
            printf("Bucket Index: %d; Depth: %d;  UserName: %s; Password: %s; ID: %d;\n", i, depth, current->user->userName, current->user->password, current->user->id);
            current = current->next;
        }
    }
}

int main(void) {
    char * names[6] = {"Alice", "Bobi", "Niki", "Pikachu", "Aaive", "Abank"};
    char filename[30] = "wedontstealyourdata.bin"; 
    HashTable * hashTable = createHashTable();
    for (int i = 0; i < 6; i++) {
        insertUser(hashTable, createUser(names[i], "test", NULL));
    }
    fileSaveUsers(hashTable, filename);
    User * newuser = createUser("Milana Milanovna", "lalej", NULL);
    fileAddUser(hashTable, filename, newuser);

    HashTable * hashTableTwo = fileReadAllUsers(filename);
    printAllUsers(hashTableTwo);
    return 0;
}
