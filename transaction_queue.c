#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "operations.h"

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

void create_transaction(transaction_queue *queue, char *key, double amount, bank_account *sender, bank_account *receiver) {
    transaction *new_trans = (transaction *) malloc(sizeof(transaction));
    if (new_trans == NULL) {
        printf("Error allocating memory\n");
        exit(1);
    }

    new_trans->status = 'U';
    strcpy(new_trans->key, key);
    new_trans->amount = amount;
    new_trans->senderID = sender->ID;
    new_trans->receiverID = receiver->ID;

    enqueue(queue, new_trans);
    save_transaction(queue);
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
