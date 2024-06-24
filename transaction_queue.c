#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "operations.h"
#include "transaction_queue.h"
#include "basefile.h"

void init_queue(transaction_queue *queue) {
    queue->head = NULL;
    queue->tail = NULL;
}

void enqueue(transaction_queue *queue, transaction *new_trans) {
    transaction_node *new_node = (transaction_node *) malloc(sizeof(transaction_node));
    if (new_node == NULL) {
        printf("Error allocating memory\n");
        exit(1);
    }
    new_node->trans = new_trans;
    new_node->next = NULL;

    if (queue->tail == NULL) {
        queue->head = new_node;
        queue->tail = new_node;
    } else {
        queue->tail->next = new_node;
        queue->tail = new_node;
    }
}

void create_transaction(transaction_queue *queue, double amount, bank_account *sender, bank_account *receiver) {
    transaction *new_trans = (transaction *) malloc(sizeof(transaction));
    if (new_trans == NULL) {
        printf("Error allocating memory\n");
        exit(1);
    }

    new_trans->status = 'U';
    new_trans->amount = amount;
    strcpy(new_trans->sender_iban, sender->iban);
    strcpy(new_trans->receiver_iban, receiver->iban);

    srand(time(NULL)); 
    generate_transaction_code(new_trans->key, sizeof(new_trans->key));

    enqueue(queue, new_trans);
    save_transactions(queue);
    transaction_func(queue, sender, receiver, new_trans);
}

void free_queue(transaction_queue *queue) {
    transaction_node *current = queue->head;
    while (current != NULL) {
        transaction_node *temp = current;
        current = current->next;
        free(temp->trans);
        free(temp);
    }
    queue->head = NULL;
    queue->tail = NULL;
}

void generate_transaction_code(char *buffer, size_t buffer_size) {
    time_t now = time(NULL); 
    struct tm *t = localtime(&now);

    strftime(buffer, buffer_size, "%Y%m%d%H%M%S", t);

    int random_number = rand() % 1000000; 
    snprintf(buffer + strlen(buffer), buffer_size - strlen(buffer), "%06d", random_number);
}
