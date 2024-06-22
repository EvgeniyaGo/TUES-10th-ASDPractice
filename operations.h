#ifndef OPERATIONS_H
#define OPERATIONS_H

#define COUNTRY_CODE "BG"
#define BANK_CODE "BNBG"

typedef struct bank_account{
    char iban[30];
    double balance;
    int ID;
}bank_account;

void generate_random_account_number(char *account_number, int length);
int calculate_checksum(const char *iban);
void generate_iban(char *iban);

void deposit(bank_account *b_account, double amount);
void withdrawals(bank_account *b_account, double amount);

// Transactions

typedef struct transaction{
    char status;
    char key[20];
    double amount;
    int senderID;
    int receiverID;
}transaction;

typedef struct transaction_node{
    transaction *trans;
    struct transaction_node *next;
} transaction_node;

typedef struct {
    transaction_node *head;
    transaction_node *tail;
} transaction_queue;

void init_queue(transaction_queue *queue);
void enqueue(transaction_queue *queue, transaction *new_trans);
void create_transaction(transaction_queue *queue, char *key, double amount, bank_account *sender, bank_account *receiver);
void free_queue(transaction_queue *queue);

void transaction_func(transaction_queue *queue, bank_account *sender, bank_account *receiver, transaction *trans);
void save_transaction(transaction_queue *queue);

#endif
