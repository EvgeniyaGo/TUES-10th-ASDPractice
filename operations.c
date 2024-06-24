#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "operations.h"
#include "transaction_queue.h"
#include "userhandling.h"
#include "cryptfiles.h"

#define MAXREAD_O 200

void generate_random_account_number(char *account_number, int length) {
    static const char alphanum[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    
    for(int i = 0; i < length; ++i){
        account_number[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
    }
    account_number[length] = '\0';
}

int calculate_checksum(const char *iban) {
    char rearranged_iban[34];
    snprintf(rearranged_iban, sizeof(rearranged_iban), "%s%s", iban + 4, iban);

    char numeric_iban[70] = "";
    for (int i = 0; i < strlen(rearranged_iban); i++) {
        if(rearranged_iban[i] >= 'A' && rearranged_iban[i] <= 'Z'){
            char num[3];
            snprintf(num, sizeof(num), "%d", rearranged_iban[i] - 'A' + 10);
            strcat(numeric_iban, num);
        }else{
            strncat(numeric_iban, &rearranged_iban[i], 1);
        }
    }

    int remainder = 0;
    for(int i = 0; i < strlen(numeric_iban); i++){
        int digit = numeric_iban[i] - '0';
        remainder = (remainder * 10 + digit) % 97;
    }

    return remainder;
}

void generate_iban(char *iban) {
    char account_number[17];
    generate_random_account_number(account_number, 16);

    char initial_iban[30];
    snprintf(initial_iban, sizeof(initial_iban), "%s00%s%s", COUNTRY_CODE, BANK_CODE, account_number);

    int checksum = 98 - calculate_checksum(initial_iban);

    snprintf(iban, 30, "%s%02d%s%s", COUNTRY_CODE, checksum, BANK_CODE, account_number);
}

double round_two_decimal_places(double value){
    double scale = pow(10, 2);
    return round(value * scale) / scale;
}

void deposit(bank_account * b_account, double amount) {
    amount = round_two_decimal_places(amount);

    if(amount > 0) {
        b_account->balance += amount;
        printf("Successfully deposited %f. New balance: %f\n", amount, b_account->balance); 
    }else{
        printf("Deposit amount must be positive.\n");
    }
}

void withdrawals(bank_account *b_account, double amount) {
    amount = round_two_decimal_places(amount);

    if(amount > 0){
        if (b_account->balance >= amount) {
            b_account->balance -= amount;
            printf("Successfully withdrew %f. New balance: %f\n", amount, b_account->balance);
        }else{ 
            printf("Insufficient balance.\n");
        }
    }else{
        printf("Withdrawal amount must be positive.\n");
    } 
}

void transaction_func(transaction_queue *queue, bank_account *sender, bank_account *receiver, transaction *trans) {
    trans->amount = round_two_decimal_places(trans->amount);

    if (sender->balance >= trans->amount) {
        sender->balance -= trans->amount;
        receiver->balance += trans->amount;
        
        trans->status = 'C';
        printf("Transaction completed successfully.\n");
    } else {
        trans->status = 'F';
        printf("Transaction failed: insufficient funds.\n");
        return;
    }

    save_transactions(queue);
}

void save_transactions(transaction_queue *queue) {
    FILE *fp = fopen("transactions.txt", "w");
    if (fp == NULL) {
        printf("Error opening file\n");
        return;
    }

    transaction_node *current = queue->head;
    while (current != NULL) {
        transaction *trans = current->trans;
        fprintf(fp, "%c|%s|%.2f|%s|%s\n", trans->status, trans->key, trans->amount, trans->sender_iban, trans->receiver_iban);
        current = current->next;
    }

    fclose(fp);
}
void transaction_processing(char *filename, transaction_queue *queue){
    if(queue == NULL) {
        fprintf(stderr, "Error: transaction_queue is NULL\n");
        return;
    }

    transaction_node *current = queue->head;
    while(current != NULL) {
        transaction *trans = current->trans;
        if(trans == NULL) {
            fprintf(stderr, "Error: transaction is NULL\n");
            current = current->next;
            continue; 
        }

        if(trans->status == 'U') {
            bank_account *ba_S = find_account_by_iban(filename, trans->sender_iban);
            bank_account *ba_R = find_account_by_iban(filename, trans->receiver_iban);

            if(ba_S == NULL) {
                fprintf(stderr, "Error: Sender with IBAN %d not found\n", trans->sender_iban);
                trans->status = 'F'; 
                current = current->next;
                continue; 
            }
            if(ba_R == NULL) {
                fprintf(stderr, "Error: Receiver with IBAN %d not found\n", trans->receiver_iban);
                trans->status = 'F';
                current = current->next;
                continue; 
            }
            if(ba_S->balance < trans->amount) {
                fprintf(stderr, "Error: Insufficient funds for transaction from IBAN %d\n", trans->sender_iban);
                trans->status = 'F'; 
                current = current->next;
                continue; 
            }
            transaction_func(queue, ba_S, ba_R, trans);
        }
        current = current->next;
    }
    save_transactions(queue);
}


bank_account * createBankAccount(char * ID) {
    bank_account * bAccount = (bank_account *)malloc(sizeof(bank_account));
    generate_iban(bAccount->iban);
    bAccount->balance = 0.0;
    strncpy(bAccount->ID, ID, 5);
    bAccount->ID[4] = '\0'; 
    return bAccount;
}

void fileAddBankAccount(bank_account * bAccount, char  *filename) {
    FILE * fptr;
    fptr = fopen(filename, "a");
    if (fptr == NULL) {
        fptr = fopen(filename, "w");
        if (fptr == NULL) {
            printf("Error: Cannot create file %s\n", filename);
            return;
        }
    }
    char toBeEncrypted[MAXREAD_O];
    char encrypted[MAXREAD_O];
    snprintf(toBeEncrypted, MAXREAD_O, "%s|%.2f|%s", bAccount->iban, bAccount->balance, bAccount->ID);
    vigenereTable(toBeEncrypted, encrypted);
    fprintf(fptr, "%s\n", encrypted);
    fclose(fptr);
}

bank_account *find_account_by_iban(char *filename, char * iban_to_search) {
    FILE *fptr;
    fptr = fopen(filename, "r");
    if (fptr == NULL) {
        printf("Error: cannot open file %s\n", filename);
        return NULL;
    }

    char to_be_decrypted[MAXREAD_O];
    char line[MAXREAD_O];
    while (fgets(line, sizeof(line), fptr)) {
        char my_string[MAXREAD_O];
        decodeVigenere(line, my_string);

        char fileID[5];
        char iban[30];
        double balance;
        char *token;

        token = strtok(my_string, "|");
        if (token != NULL) strncpy(iban, token, sizeof(iban) - 1);
        iban[sizeof(iban) - 1] = '\0'; 

        token = strtok(NULL, "|");
        if (token != NULL) balance = atof(token);

        token = strtok(NULL, "\n");
        if (token != NULL) strncpy(fileID, token, sizeof(fileID) - 1);
        fileID[sizeof(fileID) - 1] = '\0'; 
        if (strcmp(iban_to_search, iban) == 0) {
            bank_account *account = (bank_account *)malloc(sizeof(bank_account));
            if (account == NULL) {
                printf("Error: Memory allocation failed\n");
                fclose(fptr);
                return NULL;
            }
            strncpy(account->ID, fileID, sizeof(account->ID) - 1);
            account->ID[sizeof(account->ID) - 1] = '\0'; 

            account->balance = balance;
            strncpy(account->iban, iban, sizeof(account->iban) - 1);
            account->iban[sizeof(account->iban) - 1] = '\0'; 

            fclose(fptr);
            return account;
        }
    }

    fclose(fptr);
    return NULL;
}

bank_account * findAccountByIDFromFile(char * filename, char * ID) {
    FILE *fptr;
    fptr = fopen(filename, "r");
    if (fptr == NULL) {
        printf("Error opening file %s\n", filename);
        printf("Error: cannot open file %s\n", filename);
        return NULL;
    }

    char toBeDecrypted[MAXREAD_O];
    char myString[MAXREAD_O];
    while (fgets(toBeDecrypted, MAXREAD_O, fptr)) {
        decodeVigenere(toBeDecrypted, myString);
        char fileID[5];
        char iban[30];
        double balance;
        char * token;
        token = strtok(myString, "|");
        if (token != NULL) strncpy(iban, token, sizeof(iban) - 1);
        token = strtok(NULL, "|");
        if (token != NULL) balance = atof(token);
        token = strtok(NULL, "\n");
        if (token != NULL) strncpy(fileID, token, sizeof(fileID) - 1);
        if (strcmp(ID, fileID) == 0) {
            bank_account * account = (bank_account *)malloc(sizeof(bank_account));
            if (account == NULL) {
                printf("Memory allocation failed\n");
                printf("Error: Memory allocation failed\n");
                fclose(fptr);
                return NULL;
            }
            strncpy(account->ID, fileID, sizeof(account->ID) - 1);
            account->balance = balance;
            strncpy(account->iban, iban, sizeof(account->iban) - 1);

            account->ID[4] = '\0';
            fclose(fptr);
            return account;
        }
    }

    fclose(fptr);
    printf("Account with ID %s must be created\n", ID);
    return NULL;
}

bank_account * registerBankAccount(char * ID, char * filename) {
    if(findAccountByIDFromFile(filename, ID) != NULL) return findAccountByIDFromFile(filename, ID);
    bank_account * bAccount = createBankAccount(ID);
    fileAddBankAccount(bAccount, filename);
    return bAccount;
}
void updatefileById(char *filename, char *searchingId, bank_account *bAccount) {
    FILE *fptr;
    char line[MAXREAD_O];
    char updated[MAXREAD_O * 100] = ""; 
    int found = 0;

    fptr = fopen(filename, "r");
    if (fptr == NULL) {
        printf("Error: cannot open file %s\n", filename);
        return;
    }

    while (fgets(line, sizeof(line), fptr)) {
        char decryptedLine[MAXREAD_O];
        decodeVigenere(line, decryptedLine);

        char iban[30];
        double balance;
        char id[5];

        char *token = strtok(decryptedLine, "|");
        if (token != NULL) strncpy(iban, token, sizeof(iban) - 1);
        token = strtok(NULL, "|");
        if (token != NULL) balance = atof(token);
        token = strtok(NULL, "\n");
        if (token != NULL) strncpy(id, token, sizeof(id) - 1);
        iban[sizeof(iban) - 1] = '\0';
        id[sizeof(id) - 1] = '\0';

        if (strcmp(searchingId, id) == 0) {
            char newLine[MAXREAD_O];
            char encryptedLine[MAXREAD_O];
            snprintf(newLine, sizeof(newLine), "%s|%.2f|%s\n", bAccount->iban, bAccount->balance, bAccount->ID);
            vigenereTable(newLine, encryptedLine);
            strcat(updated, encryptedLine);
            found = 1;
        } else {
            strcat(updated, line);
        }
    }
    fclose(fptr);

    if (!found) {
        printf("Error: ID %s not found in the file\n", searchingId);
        return;
    }

    fptr = fopen(filename, "w");
    if (fptr == NULL) {
        printf("Error: cannot open file %s\n", filename);
        return;
    }
    fputs(updated, fptr);
    fclose(fptr);
}
