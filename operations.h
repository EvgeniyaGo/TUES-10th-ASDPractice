#ifndef OPERATIONS_H
#define OPERATIONS_H

#define COUNTRY_CODE "BG"
#define BANK_CODE "BNBG"

// Accounts
typedef struct bank_account {
    char iban[30];
    double balance;
    char ID[5];
} bank_account;

void generate_random_account_number(char *account_number, int length);
int calculate_checksum(const char *iban);
void generate_iban(char *iban);

bank_account * findAccountByIDFromFile(char * filename, char * ID);
bank_account * registerBankAccount(char *ID, char * filename);
void fileAddBankAccount(bank_account *bAccount, char *filename);
bank_account * createBankAccount(char * ID);
void updatefileById(char *filename, char * searchindId, bank_account * bAccount);

// Transactions
typedef struct transaction {
    char status;
    char key[20];
    double amount;
    char senderID[5];    
    char receiverID[5];  
} transaction;

typedef struct transaction_node {
    transaction *trans;
    struct transaction_node *next;
} transaction_node;

typedef struct {
    transaction_node *head;
    transaction_node *tail;
} transaction_queue;

void init_queue(transaction_queue *queue);
void enqueue(transaction_queue *queue, transaction *new_trans);
void create_transaction(HashTable *table, transaction_queue *queue, double amount, bank_account *sender, bank_account *receiver);
void free_queue(transaction_queue *queue);

// Operations
void deposit(bank_account *b_account, double amount);
void withdrawals(bank_account *b_account, double amount);

void transaction_func(transaction_queue *queue, bank_account *sender, bank_account *receiver, transaction *trans);
void save_transactions(transaction_queue *queue);
void transaction_processing(HashTable *table, transaction_queue *queue);

#endif
