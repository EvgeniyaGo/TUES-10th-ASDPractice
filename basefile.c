#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "basefile.h"
#include "userhandling.h"
#include "cryptfiles.h"
#include "sha-passhash.h"

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


// void dummyMenu(HashTable * hashTable, int logged){
//     if(logged == 0){
//         int choice;
//         do{
//             printf("1. Login; 2. Register; 3. Exit\n");
//             scanf("%d", &choice);
//         } while(choice != 3);
//         if(choice == 1) {
//             printf("Login");
//         }
//         else if(choice == 2) {
//             printf("Register");
//         }
//         else if(choice == 3) {
//             printf("\n[!] Ending program.");
//             exit(0);
//         }
//     }
//     else{
//         int choice;
//         do{
//             printf("Second menu, adi; 4. Exit\n");
//             scanf("%d", &choice);
//         } while(choice != 4);
//     }
//     printf("\n\n\n");
//     dummyMenu(hashTable);

// }

int main(void) {
    char filename[30] = "wedontstealyourdata.bin"; 
    HashTable * hashTable = createHashTable(); 

//    hashTable = fileReadAllUsers(filename);
//    hashTable = createDummyHashTable(hashTable, filename);
//    hashTable = fileReadAllUsers(filename);
 
    uint8_t hashedPass[32];
    hashPass("spls", hashedPass);
    registerUser("Janneta", hashedPass, hashTable, filename);
    hashPass("parampampam", hashedPass);
    registerUser("Niko", hashedPass, hashTable, filename);
    hashPass("lele", hashedPass);
    registerUser("Kireo", hashedPass, hashTable, filename);
    hashPass("raaqwertyurwqkpfoqwkwpfoiolre", hashedPass);
    registerUser("Niks", hashedPass, hashTable, filename);

    printAllUsers(hashTable);
    fileSaveUsers(hashTable, filename);
    printf("\n\n-the-end-");
    return 0;
}
