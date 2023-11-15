#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_SIZE 10

// Node structure representing an element in the distributed queue
struct Node {
    int data;
    struct Node* next;
};

// Distributed Queue structure with a front and rear pointer
struct DistributedQueue {
    struct Node* front;
    struct Node* rear;
    pthread_mutex_t lock;
};

// Initialize the distributed queue with NULL front and rear pointers
void initDistributedQueue(struct DistributedQueue* queue) {
    queue->front = NULL;
    queue->rear = NULL;
    pthread_mutex_init(&queue->lock, NULL);
}

// Enqueue an element into the distributed queue
void enqueue(struct DistributedQueue* queue, int data) {
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
    newNode->data = data;
    newNode->next = NULL;

    // Lock the mutex before modifying the queue
    pthread_mutex_lock(&queue->lock);

    // If the queue is empty, update both front and rear pointers
    if (queue->front == NULL) {
        queue->front = newNode;
        queue->rear = newNode;
    } else {
        // Otherwise, update the rear pointer
        queue->rear->next = newNode;
        queue->rear = newNode;
    }

    // Print a message indicating the enqueue operation
    printf("Enqueued %d into the distributed queue\n", data);

    // Unlock the mutex after the operation is complete
    pthread_mutex_unlock(&queue->lock);
}

// Dequeue an element from the distributed queue
int dequeue(struct DistributedQueue* queue) {
    // Lock the mutex before modifying the queue
    pthread_mutex_lock(&queue->lock);

    // Check if the queue is empty
    if (queue->front == NULL) {
        // Print a message indicating that the queue is empty
        printf("Distributed Queue is empty\n");

        // Unlock the mutex and return -1 to indicate an empty queue
        pthread_mutex_unlock(&queue->lock);
        return -1;
    }

    // Retrieve data from the front node and update pointers
    int data = queue->front->data;
    struct Node* temp = queue->front;

    // If there is only one element in the queue, update both front and rear pointers to NULL
    if (queue->front == queue->rear) {
        queue->front = NULL;
        queue->rear = NULL;
    } else {
        // Otherwise, update the front pointer
        queue->front = queue->front->next;
    }

    // Free the memory of the dequeued node
    free(temp);

    // Print a message indicating the dequeue operation
    printf("Dequeued %d from the distributed queue\n", data);

    // Unlock the mutex after the operation is complete
    pthread_mutex_unlock(&queue->lock);

    // Return the dequeued data
    return data;
}

// Structure to hold arguments for thread functions
struct ThreadArgs {
    struct DistributedQueue* queue;
    int data;
};

// Thread function to enqueue elements into the distributed queue
void* enqueueThread(void* arg) {
    // Cast the argument to the ThreadArgs structure
    struct ThreadArgs* args = (struct ThreadArgs*)arg;

    // Call the enqueue function with the specified queue and data
    enqueue(args->queue, args->data);

    // Return NULL as required by the pthread_create function
    return NULL;
}

// Thread function to dequeue elements from the distributed queue
void* dequeueThread(void* arg) {
    // Cast the argument to the ThreadArgs structure
    struct ThreadArgs* args = (struct ThreadArgs*)arg;

    // Call the dequeue function with the specified queue
    dequeue(args->queue);

    // Return NULL as required by the pthread_create function
    return NULL;
}

// Main function
int main() {
    // Initialize the distributed queue
    struct DistributedQueue queue;
    initDistributedQueue(&queue);

    // Array to hold thread IDs
    pthread_t threads[4];

    // Create threads to enqueue and dequeue concurrently
    struct ThreadArgs enqueueArgs[2] = {{&queue, 1}, {&queue, 2}};
    struct ThreadArgs dequeueArgs[2] = {{&queue, 1}, {&queue, 2}};

    for (int i = 0; i < 2; i++) {
        // Create threads for enqueuing
        pthread_create(&threads[i], NULL, enqueueThread, (void*)&enqueueArgs[i]);
        // Create threads for dequeuing
        pthread_create(&threads[i + 2], NULL, dequeueThread, (void*)&dequeueArgs[i]);
    }

    // Wait for the threads to finish
    for (int i = 0; i < 4; i++) {
        pthread_join(threads[i], NULL);
    }

    // Return 0 to indicate successful program execution
    return 0;
}
