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
    char * names[] = { // Yep used gpt to generate them
        "Alice", "Bob", "Charlie", "David", "Emma", "Frank", "Nina", "Henry", "Ivy", "Jack",
        "Kate", "Liam", "Mia", "Noah", "Olivia", "Peter", "Quinn", "Rose", "Sam", "Tina",
        "Uma", "Victor", "Wendy", "Xavier", "Yara", "Zane", "Sophia", "Thomas", "Ella", "Lucas",
        "Hannah", "Jacob", "Ava", "Michael", "Elizabeth", "Daniel", "Chloe", "Alexander", "Sophie", "James",
        "Emily", "Benjamin", "Grace", "William", "Aria", "Oliver", "Evelyn", "Matthew", "Isabella", "Logan",
        "Avery", "Luke", "Lily", "Gabriel", "Madison", "Jackson", "Victoria", "Ethan", "Natalie", "Andrew",
        "Zoe", "Owen", "Claire", "Joshua", "Penelope", "Nathan", "Stella", "Ryan", "Samantha", "Dylan",
        "Addison", "Caleb", "Audrey", "Isaac", "Leah", "Hunter", "Maya", "Christian", "Eleanor", "Jonathan",
        "Sofia", "Elijah", "Anna", "Grayson", "Lillian", "Aaron", "Alyssa", "Tyler", "Hailey", "Nicholas"
    };

    // Array of 100 passwords
    char * passwords[] = { // Yep used gpt to generate them
        "password1", "securepass", "myp@ssw0rd", "123456", "pass1234", "qwerty", "letmein", "football", "123abc", "password123",
        "p@ssw0rd", "hello123", "welcome", "12345678", "qwerty123", "admin", "iloveyou", "monkey", "1234", "abc123",
        "123456789", "password", "welcome123", "baseball", "dragon", "sunshine", "master", "superman", "batman", "123qwe",
        "trustno1", "shadow", "123123", "password1", "ashley", "charlie", "monkey", "sunshine", "bailey", "mustang",
        "qwertyuiop", "access", "michael", "password", "12345", "jennifer", "iloveyou", "1234567", "purple", "samantha",
        "abcdef", "harley", "buster", "thomas", "falcon", "jordan", "hunter", "cookie", "andrew", "tigger",
        "1234567890", "555555", "chelsea", "peanut", "ginger", "dallas", "hannah", "panther", "joshua", "justin",
        "banana", "summer", "maggie", "cookie", "bailey", "qwerty", "snoopy", "jackson", "buster", "123456",
        "football", "monkey", "batman", "welcome", "lovely", "password", "ginger", "shadow", "superman", "soccer"
    };

    for (int i = 0; i < 90; i++) {
        registerUser(names[i],passwords[i], hashTable, filename);
    }
    return hashTable;
}


void dummyMenu(HashTable * hashTable, int logged){
    char choice;
    if(logged == 0){
        do{
            printf("1. Login; 2. Register; 3. Exit\n\nChoice:");
            scanf("%c", &choice);
        } while(choice < '1' || choice > '3');
        if(choice == '1') {
            printf("Login\n");
            logged = 1;
        }
        else if(choice == '2') {
            printf("Register\n");
            logged = 1;
        }
        else if(choice == '3') {
            printf("\n[!] Ending program.");
            exit(0);
        }
    }
    else{
        do{
            printf("Second menu, adi; 4. Logout\n\nChoice:");
            scanf("%c", &choice);
            if(choice == '4') {
                printf("Logout\n");
                logged = 0;
            }
        } while(choice != '4');
    }
    printf("\n\n\n");
    dummyMenu(hashTable, logged);
}

int main(void) {
    char filename[30] = "wedontstealyourdata.bin"; 
    HashTable * hashTable = createHashTable(); 
    dummyMenu(hashTable, 0);
    printf("\n\n-the-end-");
    return 0;
}
