#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <conio.h>

#include "basefile.h"
#include "userhandling.h"
#include "cryptfiles.h"
#include "sha-passhash.h"
#include "operations.h"
#include "transaction_queue.h"

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

    for (int i = 0; i < 5; i++) {
        registerUser(names[i], passwords[i], hashTable, filename);
    }
    return hashTable;
}

void free_hash_table(HashTable *hash_table) {
    if (hash_table == NULL) {
        return;
    }

    for (int i = 0; i < TABLE_SIZE; i++) {
        BucketNode *current = hash_table->buckets[i];
        while (current != NULL) {
            BucketNode *next_node = current->next;
            free(current->user); 
            free(current); 
            current = next_node;
        }
    }

    free(hash_table);
}

void hashedToDecimalString(uint8_t *hashed, char *passchar) {
    int passcharind = 0;
    for (int i = 0; i < 32; i++) {
        passcharind += sprintf(passchar + passcharind, "%03d", hashed[i]);
    }
}

void hashPassToString(uint8_t * password, char * passchar){
    passchar[96] = '\0';
    int passcharind = 0;
    for (int i = 0; i < 32; i++) {
        passcharind += sprintf(passchar + passcharind, "%03d", password[i]);
    }
}

User *  canlogin(HashTable * hashTable, char * username, char * password){
    User * foundUser = searchUser(hashTable, username);
    if(foundUser == NULL) {
        printf("Cannot find user\n");
        return NULL;
    }
    char pass[97];
    preppass(password, pass);
    if (strcmp(foundUser->password, pass) != 0) {
        printf("Password doesn't match\n");
        return NULL;
    }
    return foundUser;
}


void dummyMenu(HashTable *hashTable, User *loggedRightNow, char *filename, char *banksfilename, transaction_queue *queue) {
    char choice;
    int menuChoice = 0;
    int numChoices;
    int ch;

    if (loggedRightNow == NULL) {
        numChoices = 3;
        do {
            printf("\n");
            printf("%s1. Login\n", menuChoice == 0 ? "> " : "  ");
            printf("%s2. Register\n", menuChoice == 1 ? "> " : "  ");
            printf("%s3. Exit\n", menuChoice == 2 ? "> " : "  ");

            ch = _getch(); 
            if (ch == 0 || ch == 224) {
                ch = _getch();
                if (ch == 72) { 
                    menuChoice = (menuChoice - 1 + numChoices) % numChoices;
                } else if (ch == 80) {
                    menuChoice = (menuChoice + 1) % numChoices;
                }
            } else if (ch == 13) { 
                choice = '1' + menuChoice;
                break;
            }

        } while (1);

        if (choice == '1') {
            printf("Login\n");
            char username[50];
            char password[32];

            do {
                printf("Enter username: ");
                fgets(username, sizeof(username), stdin);
                username[strcspn(username, "\n")] = 0; // Remove newline character

                printf("Enter password: ");
                fgets(password, sizeof(password), stdin);
                password[strcspn(password, "\n")] = 0;

                loggedRightNow = canlogin(hashTable, username, password);
                if (loggedRightNow != NULL) {
                    printf("Login successful!\n");
                } else {
                    printf("Invalid username or password\n");
                }
            } while (loggedRightNow == NULL);
        } else if (choice == '2') {
            char username[50];
            char password[32];
            printf("Register\n");

            printf("Enter username: ");
            fgets(username, sizeof(username), stdin);
            username[strcspn(username, "\n")] = 0;

            printf("Enter password: ");
            fgets(password, sizeof(password), stdin);
            password[strcspn(password, "\n")] = 0;

            loggedRightNow = registerUser(username, password, hashTable, filename);
            if (loggedRightNow != NULL) {
                printf("Registration successful for user: %s\n", loggedRightNow->userName);
            } else {
                printf("Registration failed\n");
            }
        } else if (choice == '3') {
            printf("\n[!] Ending program\n");
            exit(0);
        }
    } else {
        bank_account *loggedBank = registerBankAccount(loggedRightNow->id, banksfilename);
        printf("Hello, %s! Your IBAN is: %s\n", loggedRightNow->userName, loggedBank->iban);
        numChoices = 5;
        do {
            printf("\n");
            printf("%s1. Deposit\n", menuChoice == 0 ? "> " : "  ");
            printf("%s2. Withdraw\n", menuChoice == 1 ? "> " : "  ");
            printf("%s3. Transfer\n", menuChoice == 2 ? "> " : "  ");
            printf("%s4. Process Transactions\n", menuChoice == 3 ? "> " : "  ");
            printf("%s5. Logout\n", menuChoice == 4 ? "> " : "  ");

            ch = _getch(); 
            if (ch == 0 || ch == 224) {
                ch = _getch();
                if (ch == 72) { 
                    menuChoice = (menuChoice - 1 + numChoices) % numChoices;
                } else if (ch == 80) {
                    menuChoice = (menuChoice + 1) % numChoices;
                }
            } else if (ch == 13) { 
                choice = '1' + menuChoice;
                break;
            }
        } while (1);
        if (choice == '1') {
            double amount;
            printf("Amount to deposit: ");
            scanf("%lf", &amount);
            printf("Am:%f", amount);
            deposit(loggedBank, amount);
            printf("  %f\n", loggedBank->balance);
            updatefileById(banksfilename, loggedBank->ID, loggedBank);
        } else if (choice == '2') {
            double amount;
            printf("Amount to withdraw: ");
            scanf("%lf", &amount);
            withdrawals(loggedBank, amount);
            printf("  %f\n", loggedBank->balance);
            updatefileById(banksfilename, loggedBank->ID, loggedBank);
        } else if (choice == '3') {
            char receiver_iban[30];
            printf("Enter IBAN of receiver: ");
            scanf("%s", receiver_iban);
            double amount;
            printf("Enter amount[X.XX]: ");
            scanf("%lf", &amount);
            bank_account *ba_S = find_account_by_iban(filename, loggedBank->iban);
            bank_account *ba_R = find_account_by_iban(filename, receiver_iban);
            printf("%s, %s\n", ba_S->iban, ba_R->iban);
            create_transaction(queue, amount, ba_S, ba_R);
        } else if (choice == '4') {
            transaction_processing(filename, queue);
        } else if (choice == '5') {
            printf("Logging out %s\n", loggedRightNow->userName);
            loggedRightNow = NULL;
        }
    }
    printf("\n\n\nMenu:\n");
    dummyMenu(hashTable, loggedRightNow, filename, banksfilename, queue);
}

int main(void) {
    printf("Hello and welcome!\n");
    char filename[30] = "wedontstealyourdata.bin"; 
    char banksfilename[30] = "banksfilename.bin"; 
    HashTable * hashTable = createHashTable(); 
    transaction_queue *queue;
    init_queue(queue);
//    dummyMenu(hashTable, 0);
    hashTable = fileReadAllUsers(filename);
    // printAllUsers(hashTable);
//     printf("creating-\n");
  //  createDummyHashTable(hashTable, filename);
    dummyMenu(hashTable, 0, filename, banksfilename, queue);
    //fileSaveUsers(hashTable, filename);
    printf("\n\nwell it didnt die");

    free_hash_table(hashTable);
    free_queue(queue);
    return 0;
}

