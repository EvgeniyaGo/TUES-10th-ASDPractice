#ifndef OPERATIONS_H
#define OPERATIONS_H

#define COUNTRY_CODE "BG"
#define BANK_CODE "BNBG"

typedef struct bank_account{
    char iban[30];
    unsigned int balance;
    int ID;
}bank_account;

void generate_random_account_number(char *account_number, int length);
int calculate_checksum(const char *iban);
void generate_iban(char *iban);

void deposit(bank_account *b_account, unsigned int amount);
void withdrawals(bank_account *b_account, unsigned int amount);

typedef struct transaction{
    char status;
    char key;
    int amount;
    int senderID;
    int receiverID;
}transaction;

#endif