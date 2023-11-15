
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// Node structure representing an element in the stack
struct Node {
    int data;
    struct Node* next;
};

// Stack structure with a top pointer and a mutex lock for thread safety
struct Stack {
    struct Node* top;
    pthread_mutex_t lock;
};

// Initialize the stack with a NULL top pointer and a mutex
void initStack(struct Stack* stack) {
    stack->top = NULL;
    pthread_mutex_init(&stack->lock, NULL);
}

// Push a new element onto the stack
void push(struct Stack* stack, int data) {
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
    newNode->data = data;

    // Lock the mutex before modifying the stack
    pthread_mutex_lock(&stack->lock);

    // Update pointers to push the new node onto the stack
    newNode->next = stack->top;
    stack->top = newNode;

    // Print a message indicating the push operation
    printf("Thread %lu: Pushed %d onto the stack\n", pthread_self(), data);

    // Unlock the mutex after the operation is complete
    pthread_mutex_unlock(&stack->lock);
}

// Pop an element from the stack
int pop(struct Stack* stack) {
    // Lock the mutex before modifying the stack
    pthread_mutex_lock(&stack->lock);

    // Check if the stack is empty
    if (stack->top == NULL) {
        // Print a message indicating that the stack is empty
        printf("Thread %lu: Stack is empty\n", pthread_self());

        // Unlock the mutex and return -1 to indicate an empty stack
        pthread_mutex_unlock(&stack->lock);
        return -1;
    }

    // Retrieve data from the top node and update pointers to pop the node
    int data = stack->top->data;
    struct Node* temp = stack->top;
    stack->top = stack->top->next;

    // Free the memory of the popped node
    free(temp);

    // Print a message indicating the pop operation
    printf("Thread %lu: Popped %d from the stack\n", pthread_self(), data);

    // Unlock the mutex after the operation is complete
    pthread_mutex_unlock(&stack->lock);

    // Return the popped data
    return data;
}

// Structure to hold arguments for thread functions
struct ThreadArgs {
    struct Stack* stack;
    int data;
};

// Thread function to push elements onto the stack
void* pushThread(void* arg) {
    // Cast the argument to the ThreadArgs structure
    struct ThreadArgs* args = (struct ThreadArgs*)arg;

    // Call the push function with the specified stack and data
    push(args->stack, args->data);

    // Return NULL as required by the pthread_create function
    return NULL;
}

// Thread function to pop elements from the stack
void* popThread(void* arg) {
    // Cast the argument to the ThreadArgs structure
    struct ThreadArgs* args = (struct ThreadArgs*)arg;

    // Call the pop function with the specified stack
    pop(args->stack);

    // Return NULL as required by the pthread_create function
    return NULL;
}

// Main function
int main() {
    // Initialize the stack
    struct Stack stack;
    initStack(&stack);

    // Array to hold thread IDs
    pthread_t threads[4];

    // Create threads to push and pop concurrently
    struct ThreadArgs pushArgs[2] = {{&stack, 1}, {&stack, 2}};
    struct ThreadArgs popArgs[2] = {{&stack, 1}, {&stack, 2}};

    for (int i = 0; i < 2; i++) {
        // Create threads for pushing
        pthread_create(&threads[i], NULL, pushThread, (void*)&pushArgs[i]);
        // Create threads for popping
        pthread_create(&threads[i + 2], NULL, popThread, (void*)&popArgs[i]);
    }

    // Wait for the threads to finish
    for (int i = 0; i < 4; i++) {
        pthread_join(threads[i], NULL);
    }

    // Return 0 to indicate successful program execution
    return 0;
}
