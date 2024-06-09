#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
[J] must do to finish this part of work
+ Make a search
- Hash the pass
- Save to file 
- Read from the file 
- Actually generate the id
- Actually register users/login 'em
+ Redo list to something fancier like a tree or smth
- Dont die (optional) 
*/

//Some basic init 
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

User * createUser(char * userName, char * password) {
    User * newUser = (User * )malloc(sizeof(User));
    strcpy(newUser->userName, userName);
    strcpy(newUser->password, password);
    newUser->id = generateId();
    return newUser;
}

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

// User *searchUser(HashTable *table, char *userName) {
//     unsigned int index = userName[0] - 'A';
//     BucketNode *current = table->buckets[index];
//     while (current != NULL) {
//         if (strcmp(current->user->userName, userName) == 0) {
//             return current->user;
//         } else if (strcmp(current->user->userName, userName) > 0) {
//             return NULL;
//         }
//         current = current->next;
//     }
//     return NULL;
// }



int main(void) {
    char * names[6] = {"Alice", "Bobi", "Niki", "Pikachu", "Aaive", "Abank"};
    HashTable * hashTable = createHashTable();
    for (int i = 0; i < 6; i++) {
        insertUser(hashTable, createUser(names[i], "test"));
    }

    printf("All Users:\n");
    for (int i = 0; i < TABLE_SIZE; i++) {
        BucketNode * current = hashTable->buckets[i];
        for(int depth = 0; current != NULL; depth++) {
            printf("Bucket Index: %d; Depth: %d;  UserName: %s; Password: %s; ID: %d;\n", i, depth, current->user->userName, current->user->password, current->user->id);
            current = current->next;
        }
    }
    return 0;
}
