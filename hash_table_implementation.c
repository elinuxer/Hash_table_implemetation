#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CAPACITY 500 // size of hashtable

unsigned long hash_function(char *str)
{
    unsigned long i = 0;

    for (int j = 0; str[j]; j++)
        i += str[j];

    return i % CAPACITY;
}

// hashtable item
typedef struct Ht_item
{
    char *key;
    char *value;
} Ht_item;

//LinkedList - Chaining
typedef struct LinkedList
{
    Ht_item *item;
    struct LinkedList *next;
} LinkedList;

//HashTable.
typedef struct HashTable
{
    // Contains an array of pointers to items.
    Ht_item **items;
    LinkedList **of_buckets;
    int size;
    int count;
} HashTable;

LinkedList *linkedlist_insert(LinkedList *list, Ht_item *item)
{
    // Inserts the item onto the LinkedList.
    if (!list)
    {
        LinkedList *head = (LinkedList *)malloc(sizeof(LinkedList));
        head->item = item;
        head->next = NULL;
        list = head;
        return list;
    }
    else if (list->next == NULL)
    {
        LinkedList *node = (LinkedList *)malloc(sizeof(LinkedList));
        node->item = item;
        node->next = NULL;
        list->next = node;
        return list;
    }

    LinkedList *temp = list;

    while (temp->next->next)
    {
        temp = temp->next;
    }

    LinkedList *node = (LinkedList *)malloc(sizeof(LinkedList));
    node->item = item;
    node->next = NULL;
    temp->next = node;
    return list;
}

Ht_item *linkedlist_remove(LinkedList *list)
{
    // Removes the head from the LinkedList.
    // Returns the item of the removed element.

    if (!list->next)
        return NULL;

    LinkedList *node = list->next;
    LinkedList *temp = list;
    temp->next = NULL;
    list = node;
    Ht_item *it = NULL;
    memcpy(temp->item, it, sizeof(Ht_item));
    return it;
}

Ht_item *create_item(char *key, char *value)
{
    // Creates a pointer to a new HashTable item.
    Ht_item *item = (Ht_item *)malloc(sizeof(Ht_item));
    item->key = (char *)malloc(strlen(key) + 1);
    item->value = (char *)malloc(strlen(value) + 1);
    strcpy(item->key, key);
    strcpy(item->value, value);
    return item;
}

HashTable *create_table(int size)
{
    // Creates a new HashTable.
    HashTable *table = (HashTable *)malloc(sizeof(HashTable));
    table->size = size;
    table->count = 0;
    table->items = (Ht_item **)calloc(table->size, sizeof(Ht_item *));

    for (int i = 0; i < table->size; i++)
        table->items[i] = NULL;

    table->overflow_buckets = LinkedList **buckets = (LinkedList **)calloc(table->size, sizeof(LinkedList *));

    for (int i = 0; i < table->size; i++)
        buckets[i] = NULL;

    return table;
}




void handle_collision(HashTable *table, unsigned long index, Ht_item *item)
{
    LinkedList *head = table->overflow_buckets[index];

    if (head == NULL)
    {
        // Creates the list.
        head = (LinkedList *)malloc(sizeof(LinkedList));
        head->item = item;
        table->overflow_buckets[index] = head;
        return;
    }
    else
    {
        // Insert to the list.
        table->overflow_buckets[index] = linkedlist_insert(head, item);
        return;
    }
}

void ht_insert(HashTable *table, char *key, char *value)
{
    // Creates the item.
    Ht_item *item = create_item(key, value);

    // Computes the index.
    int index = hash_function(key);

    Ht_item *current_item = table->items[index];

    if (current_item == NULL)
    {
        // Key does not exist.
        if (table->count == table->size)
        {
            // HashTable is full.
            printf("Insert Error: Hash Table is full\n");
            return;
        }
        // Insert directly.
        table->items[index] = item;
        table->count++;
    }
    else
    {
        // Scenario 1: Update the value.
        if (strcmp(current_item->key, key) == 0)
        {
            strcpy(table->items[index]->value, value);
            return;
        }
        else
        {
            // Scenario 2: Handle the collision.
            handle_collision(table, index, item);
            return;
        }
    }
}

char *ht_search(HashTable *table, char *key)
{
    // Searches for the key in the HashTable, returns NULL if it doesn't exist.
    int index = hash_function(key);
    Ht_item *item = table->items[index];
    LinkedList *head = table->of_buckets[index];

    // Provide only non-NULL values.
    if (item != NULL)
    {
        if (strcmp(item->key, key) == 0)
            return item->value;

        if (head == NULL)
            return NULL;

        item = head->item;
        head = head->next;
    }

    return NULL;
}

void ht_delete(HashTable *table, char *key)
{
    // Deletes an item from the table.
    int index = hash_function(key);
    Ht_item *item = table->items[index];
    LinkedList *head = table->overflow_buckets[index];

    if (item == NULL)
        return;
    else
    {
        if (head == NULL && strcmp(item->key, key) == 0)
        {
            // Collision chain does not exist.
            // Remove the item.
            // Set table index to NULL.
            table->items[index] = NULL;
            table->count--;
            return;
        }
        else if (head != NULL)
        {
            // Collision chain exists.
            if (strcmp(item->key, key) == 0)
            {
                // Remove this item.
                // Set the head of the list as the new item.
                LinkedList *node = head;
                head = head->next;
                node->next = NULL;
                table->items[index] = create_item(node->item->key, node->item->value);
                
                table->overflow_buckets[index] = head;
                return;
            }

            LinkedList *curr = head;
            LinkedList *prev = NULL;

            while (curr)
            {
                if (strcmp(curr->item->key, key) == 0)
                {
                    if (prev == NULL)
                    {
                        // First element of the chain.
                                            
                        table->overflow_buckets[index] = NULL;
                        return;
                    }
                    else
                    {
                        // This is somewhere in the chain.
                        prev->next = curr->next;
                        curr->next = NULL;
                        
                        table->overflow_buckets[index] = head;
                        return;
                    }
                }

                curr = curr->next;
                prev = curr;
            }
        }
    }
}

void print_search(HashTable *table, char *key)
{
    char *val;

    if ((val = ht_search(table, key)) == NULL)
    {
        printf("Key:%s does not exist\n", key);
        return;
    }
    else    
        printf("Key:%s, Value:%s\n", key, val);
}

void print_table(HashTable *table)
{
    for (int i = 0; i < table -> size; i++)
    {
        if (table -> items[i])
        {
            printf("Index:%d, Key:%s, Value:%s\n", i, table -> items[i] -> key, table -> items[i] -> value);
        }
    }
}

int main()
{
    HashTable *ht = create_table(CAPACITY);
    ht_insert(ht, (char *)"1", (char *)"First address");
    ht_insert(ht, (char *)"2", (char *)"Second address");
    ht_insert(ht, (char *)"Hel", (char *)"Third address");
    ht_insert(ht, (char *)"Cau", (char *)"Fourth address");
    print_search(ht, (char *)"1");
    print_search(ht, (char *)"2");
    ht_delete(ht, (char *)"Cau");
    print_table(ht);
    return 0;
}