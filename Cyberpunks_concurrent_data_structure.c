#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// Define the structure of a node in the stack
struct Node {
    int data;
    struct Node* next;
};

// Define the structure of the stack
struct Stack {
    struct Node* top;
    pthread_mutex_t lock;
};

// Initialize the stack
void initStack(struct Stack* stack) {
    stack->top = NULL;
    pthread_mutex_init(&stack->lock, NULL);
}

// Push an element onto the stack
void push(struct Stack* stack, int data) {
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
    newNode->data = data;

    pthread_mutex_lock(&stack->lock);
    newNode->next = stack->top;
    stack->top = newNode;
    pthread_mutex_unlock(&stack->lock);
}

// Pop an element from the stack
int pop(struct Stack* stack) {
    pthread_mutex_lock(&stack->lock);
    if (stack->top == NULL) {
        pthread_mutex_unlock(&stack->lock);
        return -1; // Stack is empty
    }
    
    int data = stack->top->data;
    struct Node* temp = stack->top;
    stack->top = stack->top->next;
    free(temp);

    pthread_mutex_unlock(&stack->lock);

    return data;
}

int main() {
    struct Stack stack;
    initStack(&stack);

    pthread_t threads[4];

    // Create threads to push and pop concurrently
    for (int i = 0; i < 2; i++) {
        pthread_create(&threads[i], NULL, (void*)&push, &stack, i + 1);
        pthread_create(&threads[i + 2], NULL, (void*)&pop, &stack, i + 1);
    }

    // Wait for the threads to finish
    for (int i = 0; i < 4; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}
