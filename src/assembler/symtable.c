#include "symtable.h" 
#include <stdlib.h>
#include <string.h>
#define DJB2_HASH_CONSTANT 33

// Canonical implementation of the djb2 hash function
static uint32_t hash_function_djb2(const char* label, int capacity){
    uint32_t hash = 5381;
    for(int i = 0; label[i]!='\0'; i++){
        hash = hash*DJB2_HASH_CONSTANT + (uint32_t)label[i];
    }    
    return hash & (capacity-1); // since capacity will always be a power of 2
}

static bool resize(Sym_Table* sym_table){
    int old_capacity = sym_table->capacity;
    if (((double)sym_table->size) / old_capacity > LOAD_FACTOR){
        int new_capacity = old_capacity*2;
        Node** new_buckets = calloc(new_capacity, sizeof(Node*));
        if(new_buckets==NULL){
            return false; 
        }
        for(int i = 0; i < old_capacity; i++){
            Node *bucket = sym_table->buckets[i];
            Node *curr = bucket;
            while(curr!=NULL){
                int new_index = hash_function_djb2(curr->label, new_capacity);
                Node *new_bucket = new_buckets[new_index];
                Node *head = new_bucket; 
                Node *next = curr->next;
                curr->next = head; 
                new_buckets[new_index] = curr; 
                curr = next; 
            }
        }
        free(sym_table->buckets);
        sym_table->buckets = new_buckets;
        sym_table->capacity= new_capacity;
    }
    return true; 
}

Sym_Table* symtable_init(void){
    Sym_Table* sym_table = malloc(sizeof(Sym_Table));
    if(sym_table==NULL){
        return NULL;
    }
    sym_table->capacity = INIT_CAPACITY;
    sym_table->size = 0;
    // initialise all the buckets to every head node to NULL
    sym_table->buckets = calloc(sym_table->capacity, sizeof(Node*));
    if(sym_table->buckets == NULL){
        free(sym_table);
        return NULL; 
    }
    return sym_table;
}

bool insert_address(Sym_Table* sym_table, char* label, uint64_t address){
    uint32_t index = hash_function_djb2(label, sym_table->capacity);
    Node* bucket = sym_table->buckets[index];
    Node* curr = bucket;
    // find existing value of label - unlikely but perhaps possible
    while(curr!=NULL){
        if(strcmp(curr->label, label)==0){
            curr->address = address;
            return true;
        }
        curr = curr->next; 
    }
    // insert new node 
    Node* head = bucket;
    Node* new = malloc(sizeof(Node));
    if(new == NULL){
        return false; 
    }
    new->label = strdup(label); 
    if(new->label == NULL){
        free(new);
        return false; 
    }
    new->address = address;
    new->next = head; 
    sym_table->buckets[index] = new; 
    sym_table->size++;
    resize(sym_table);
    return true; 
}

bool search_label(Sym_Table* sym_table, char* label, uint64_t*out){
    uint32_t index = hash_function_djb2(label, sym_table->capacity);
    Node* curr = sym_table->buckets[index];
    while(curr!=NULL){
        if(strcmp(curr->label, label)==0){
            *out = curr->address;
            return true;
        }
        curr = curr->next;
    }
    return false; 
}

void symtable_free(Sym_Table* sym_table){
    for(int i = 0; i < sym_table->capacity; i++){
        Node* curr_bucket = sym_table->buckets[i];
        Node* curr_node = curr_bucket;
        while(curr_node != NULL){
            Node* temp = curr_node->next;
            free(curr_node->label); 
            free(curr_node);
            curr_node = temp;
        }
    }
    free(sym_table->buckets);
    free(sym_table); 
}
