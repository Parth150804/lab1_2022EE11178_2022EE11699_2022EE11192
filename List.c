#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Define the List structure
typedef struct List {
    void **items;        // Array of pointers (generic to store any type)
    int size;            // Current number of elements
    int capacity;        // Maximum capacity of the list
} List;

// Function to create a new List
List *createList(int initialCapacity) {
    List *list = (List *)malloc(sizeof(List));
    if (!list) {
        fprintf(stderr, "Memory allocation failed for List\n");
        exit(EXIT_FAILURE);
    }
    list->items = (void **)malloc(sizeof(void *) * initialCapacity);
    if (!list->items) {
        fprintf(stderr, "Memory allocation failed for List items\n");
        free(list);
        exit(EXIT_FAILURE);
    }
    list->size = 0;
    list->capacity = initialCapacity;
    return list;
}

// Function to resize the List when capacity is exceeded
void resizeList(List *list) {
    list->capacity *= 2;
    list->items = (void **)realloc(list->items, sizeof(void *) * list->capacity);
    if (!list->items) {
        fprintf(stderr, "Memory reallocation failed for List items\n");
        exit(EXIT_FAILURE);
    }
}

// Function to add an item to the List
void addToList(List *list, void *item) {
    if (list->size == list->capacity) {
        resizeList(list);
    }
    list->items[list->size++] = item;
}

// Function to remove an item from the List by index
bool removeFromList(List *list, int index) {
    if (index < 0 || index >= list->size) {
        fprintf(stderr, "Index out of bounds\n");
        return false;
    }
    for (int i = index; i < list->size - 1; i++) {
        list->items[i] = list->items[i + 1];
    }
    list->size--;
    return true;
}

// Function to check if an item exists in the List (linear search)
bool containsInList(List *list, void *item) {
    for (int i = 0; i < list->size; i++) {
        if (list->items[i] == item) {
            return true;
        }
    }
    return false;
}

// Function to free the List
void freeList(List *list) {
    free(list->items);
    free(list);
}

// Utility function to print the List (for debugging)
void printList(List *list, void (*printItem)(void *)) {
    for (int i = 0; i < list->size; i++) {
        printItem(list->items[i]);
    }
    printf("\n");
}
