#include <stdio.h>
#include "operations.h"

#include <stdlib.h>
#include <string.h>

void transaction_func(transaction *trans){

}

void deposit(bank_account *b_account, unsigned int amount) {
    if (amount > 0) {
        b_account->balance += amount;
        printf("Successfully deposited %u. New balance: %u\n", amount, b_account->balance); 
    }else{
        printf("Deposit amount must be positive.\n");
    }
}

void withdrawals(bank_account *b_account, unsigned int amount) {
    if(amount > 0){
        if (b_account->balance >= amount) {
            b_account->balance -= amount;
            printf("Successfully withdrew %u. New balance: %u\n", amount, b_account->balance);
        }else{ 
            printf("Insufficient balance.\n");
        }
    }else{
        printf("Withdrawal amount must be positive.\n");
    } 
}

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