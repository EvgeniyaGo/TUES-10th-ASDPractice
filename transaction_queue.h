#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef TRANS_H
#define TRANS_H

void init_queue(transaction_queue *queue);

void create_transaction(transaction_queue *queue, double amount, bank_account *sender, bank_account *receiver);
void enqueue(transaction_queue *queue, transaction *new_trans);
void generate_transaction_code(char *buffer, size_t buffer_size);

void free_queue(transaction_queue *queue);


#endif 
