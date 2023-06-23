#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define MAX_PROCESSES 5
#define MAX_RESOURCES 5

int all_resources[MAX_RESOURCES];
int available[MAX_RESOURCES];
int allocated[MAX_PROCESSES][MAX_RESOURCES];
int requested[MAX_PROCESSES][MAX_RESOURCES];

typedef struct Node { //Node for keeping process value
    int process;
    struct Node* next;
} Node;

typedef struct List { //Single linked list for completed processes and deadlock processes
    Node* head;
} List;

void insert(int new_process, Node** head) { //recursive function to insert to lists
    if (*head == NULL) { //if current head is null, it dynamically allocates memory for each process to insert to list
        Node* newnode = (Node*)malloc(sizeof(Node));
        newnode->process = new_process;
        newnode->next = NULL;
        *head = newnode;
    } else {
        insert(new_process, &((*head)->next)); //if memory is not null, it passes head->next to the function
    }
}

void print_information() { //prints information of each process
    for (int j = 0; j < MAX_PROCESSES; j++) {
        printf("Information for process: P%d\n", j + 1);
        printf("Allocated Resources:");
        for (int i = 0; i < MAX_RESOURCES; i++) {
            printf(" R%d:%d", i + 1, allocated[j][i]);
        }
        printf("\nResource Request:   ");
        for (int i = 0; i < MAX_RESOURCES; i++) {
            printf(" R%d:%d", i + 1, requested[j][i]);
        }
        printf("\n\n");
    }
}

bool isSafe(int process_number) { //checks the safety of a process
    for (int i = 0; i < MAX_RESOURCES; i++) {
        if (requested[process_number][i] != -1 && requested[process_number][i] <= (allocated[process_number][i] + available[i])) { 
            continue; //checks requested[process_number][i] != -1 because I set all elements to -1 to separate completed ones
        } else {
            return false; //if even one of them does not require the need, process is not safe
        }
    }
    return true; //if all of them requires the need, process is safe
}

bool isSafeState() { //with using isSafe function to check safety of a process, isSafeState function checks all process' safety
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (isSafe(i)) {
            return true; //even if there is one safe process, system can be considered safe
        } else {
            continue;
        }
    }
    return false; //if there is no safe process, system is not safe
}

void runProcess(int process_number) {
    for (int i = 0; i < MAX_RESOURCES; i++) {
        available[i] = available[i] + allocated[process_number][i]; //allocated resources are free now so I updated the number of available resources
    }
    for (int i = 0; i < MAX_RESOURCES; i++) {
        requested[process_number][i] = -1; //I set all elements to -1 to separate completed ones
    }
}

void print_list(Node* head) { //prints list, implemented for printing completed processes and deadlock processes lists
    while (head != NULL) {
        printf(" P%d", head->process);
        head = head->next;
    }
}

void free_list(Node* head) { //recursive function to free the allocated memory, to prevent memory leak
    if (head == NULL) {
        return;
    }
    free_list(head->next);
    free(head);
}

int main() {
    FILE* resources = fopen("resources.txt", "r");
    FILE* allocations = fopen("allocations.txt", "r");
    FILE* requests = fopen("requests.txt", "r");

    for (int i = 0; i < MAX_RESOURCES; i++) {
        fscanf(resources, "%d", &all_resources[i]);
    }

    for (int i = 0; i < MAX_PROCESSES; i++) {
        for (int j = 0; j < MAX_RESOURCES; j++) {
            fscanf(allocations, "%d", &allocated[i][j]);
        }
    }

    for (int i = 0; i < MAX_PROCESSES; i++) {
        for (int j = 0; j < MAX_RESOURCES; j++) {
            fscanf(requests, "%d", &requested[i][j]);
        }
    }

    int total_allocated = 0;

    for (int i = 0; i < MAX_RESOURCES; i++) {
        for (int j = 0; j < MAX_PROCESSES; j++) {
            total_allocated = total_allocated + allocated[j][i];
        }
        available[i] = all_resources[i] - total_allocated; //available resources is equal to all resources - allocated resources
        total_allocated = 0;
    }

    fclose(resources);
    fclose(allocations);
    fclose(requests);

    List* completed = (List*)malloc(sizeof(List)); //dynamically allocated
    List* deadlock = (List*)malloc(sizeof(List)); //dynamically allocated
    completed->head = NULL;
    deadlock->head = NULL;

    print_information(); //at the beginning, prints information of all processes

    printf("Available Resources\n"); //prints available number of each resource
    for (int i = 0; i < MAX_RESOURCES; i++) {
        printf("P%d ", i+1);
    }
    printf("\n");
    for (int i = 0; i < MAX_RESOURCES; i++) {
        printf("%d  ", available[i]);
    }
    printf("\n\n");


    while (isSafeState()) { //if system is safe, the while loop continues
        for (int i = 0; i < MAX_PROCESSES; i++) {
            if (isSafe(i) == true) { //if process is safe, it can be ran
                runProcess(i);
                printf("---P%d RAN---\n", i + 1);
                insert(i + 1, &(completed->head));
                printf("Available Resources\n"); //after each running process, it prints available number of each resource
                for (int i = 0; i < MAX_RESOURCES; i++) {
                    printf("P%d ", i+1);
                }
                printf("\n");
                for (int i = 0; i < MAX_RESOURCES; i++) {
                    printf("%d  ", available[i]);
                }
                printf("\n\n");

            }
        }
    }

    for (int i = 0; i < MAX_PROCESSES; i++) { 
        if (requested[i][0] != -1) { //processes whose element is -1 (elements made -1 to seperate completed processes) is added to the deadlock list
            insert(i + 1, &(deadlock->head));
        }
    }

    printf("Running Order for Processes:");
    print_list(completed->head);
    printf("\nThere is a deadlock. Processes");
    print_list(deadlock->head);
    printf(" are the cause of deadlock.\n");

    free_list(completed->head); //frees the allocated memory 
    free_list(deadlock->head); //frees the allocated memory 
    free(completed); //frees the allocated memory 
    free(deadlock); //frees the allocated memory 

    return 0;
}
