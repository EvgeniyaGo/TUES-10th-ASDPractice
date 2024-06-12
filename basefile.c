#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "basefile.h"
#include "userhandling.h"

//Working with HashTable - creating, filling, accessing
//I use hashset with buckets for storing and quickly accessing all the users. From 0(1) till 0(n/26)=0(n)
HashTable * createHashTable() {
    HashTable * table = (HashTable * )malloc(sizeof(HashTable));
    for (int i = 0; i < TABLE_SIZE; i++) {
        table->buckets[i] = NULL;
    }
    return table;
}

//dummies
HashTable * createDummyHashTable(HashTable * hashTable, char * filename){
    char * names[6] = {"Alice", "Bobi", "Niki", "Pikachu", "Aaive", "Abank"};
    for (int i = 0; i < 6; i++) {
        registerUser(names[i], "pass", hashTable, filename);
    }
    return hashTable;
}

int main(void) {
    char filename[30] = "wedontstealyourdata.bin"; 
    HashTable * hashTable = createHashTable(); 
    hashTable = fileReadAllUsers(filename);
    hashTable = createDummyHashTable(hashTable, filename);
//    HashTable * hashTable = fileReadAllUsers(filename);
    printAllUsers(hashTable);
    return 0;
}
