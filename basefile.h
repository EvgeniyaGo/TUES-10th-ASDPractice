#ifndef COMMON_H
#define COMMON_H
#define TABLE_SIZE 26

typedef struct User {
    char userName[50];
    char password[20];
    char id;
} User;

typedef struct BucketNode {
    User * user;
    struct BucketNode * next;
} BucketNode;

typedef struct HashTable { 
    BucketNode * buckets[TABLE_SIZE];
} HashTable;

HashTable * createHashTable();
HashTable * createDummyHashTable(HashTable * hashTable, char * filename);

#endif // COMMON_H