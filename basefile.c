#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

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

void dummyMenu(HashTable * hashTable, User * loggedRightNow, char * filename, char * banksfilename, transaction_queue *queue) {
    char choice;
    
    if (loggedRightNow == NULL) {
        do {
            printf("1. Login; 2. Register; 3. Exit\n\nChoice: ");
            scanf(" %c", &choice);
            while (getchar() != '\n');
        } while (choice < '1' || choice > '3');

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
        printf("Hello, %s!\n", loggedRightNow->userName);
        printf("You can do money stuff\n");
        
        bank_account * loggedBank = registerBankAccount(loggedRightNow->id, banksfilename);
        do {
            printf("\n1. Deposit; 2. Withdraw; 3. Transfer; 4. Process Transactions; 5. Logout\n");
            printf("Choice: ");
            scanf(" %c", &choice);
            while (getchar() != '\n');

            if(choice == '1') {
                deposit(loggedBank, 10.00); 
                printf("  %f\n", loggedBank->balance);
                updatefileById(banksfilename, loggedBank->ID, loggedBank);
                break;
            }
            if(choice == '2') {
                withdrawals(loggedBank, 10.00); 
                printf("  %f\n", loggedBank->balance);
                updatefileById(banksfilename, loggedBank->ID, loggedBank);
                break;
            }
            if(choice == '3') {
                char sender_iban[30], receiver_iban[30];
                printf("Enter IBAN of sender: ");
                scanf("%s", sender_iban);
                printf("Enter IBAN of reciever: ");
                scanf("%s", receiver_iban);

                double amount;
                printf("Enter amount[X.XX]: ");
                scanf("%f", &amount);

                bank_account *ba_S = find_account_by_iban(filename ,sender_iban);
                bank_account *ba_R = find_account_by_iban(filename ,receiver_iban);
                create_transaction(queue, amount, ba_S, ba_R);
            }
            if(choice == '4') {
                transaction_processing(filename, queue);
            }
            if(choice == '5') {
                    printf("Logging out %s\n", loggedRightNow->userName);
                    loggedRightNow = NULL;
            }
        } while (choice != '5');
    }
    printf("\n\n\nMenu:\n");
    dummyMenu(hashTable, loggedRightNow, filename, banksfilename, queue); 
}


int main(void) {
    printf("HI!");
    char filename[30] = "wedontstealyourdata.bin"; 
    char banksfilename[30] = "banksfilename.bin"; 
    HashTable * hashTable = createHashTable(); 
    transaction_queue *queue;
    init_queue(&queue);
//    dummyMenu(hashTable, 0);
    hashTable = fileReadAllUsers(filename);
    // printAllUsers(hashTable);
//     printf("creating-\n");
  //  createDummyHashTable(hashTable, filename);
    printf("dummy-\n");
    dummyMenu(hashTable, 0, filename, banksfilename, queue);
    //fileSaveUsers(hashTable, filename);
    printf("\n\nwell it didnt die");
    return 0;
}

