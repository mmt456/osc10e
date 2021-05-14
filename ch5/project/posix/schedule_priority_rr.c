#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cpu.h"
#include "list.h"
#include "schedulers.h"
#include "task.h"

struct node *g_head = NULL; 
int tid = 1;
int currentTime = 0;
struct node *ready_head = NULL;
struct node *sorting_head = NULL;
int util = 0;
Task *prev = NULL;


bool comesBefore(char *a, char *b) { return strcmp(a, b) < 0; }

// based on traverse from list.c
// finds the task whose name comes first in dictionary
Task *pickNextTask() {
  // if list is empty, nothing to do
  if (ready_head == NULL && g_head == NULL && sorting_head == NULL) {
    return NULL;
  }
  /*if (!g_head) {
    g_head = ready_head;
    ready_head = NULL;
  }*/

  if (sorting_head == NULL && ready_head == NULL) {
    struct node *temp;
    temp = g_head;
    Task *best_sofar = temp->task;

        // find highest priority
        while (temp != NULL) {
            if (temp->task->priority > best_sofar->priority) {
                    best_sofar = temp->task;
            }
            temp = temp->next;
        }
        insert(&sorting_head, best_sofar);
        delete(&g_head, best_sofar);
        temp = g_head;
        // find duplicates of highest priority
        while (temp != NULL) {
            if (best_sofar->priority == temp->task->priority) {
                insert(&sorting_head, temp->task);
                delete(&g_head, temp->task);
            }
            temp = temp->next;
        }
  } 
    if (sorting_head == NULL) {
        sorting_head = ready_head;
        ready_head = NULL;
    } 
    struct node *temp;
    temp = sorting_head;
    Task *best_sofar = temp->task;

    while (temp != NULL) {
        if (comesBefore(temp->task->name, best_sofar->name))
            best_sofar = temp->task;
        temp = temp->next;
    }
    if (best_sofar->burst - QUANTUM > 0) {
        insert(&ready_head, best_sofar);
    }
    delete(&sorting_head, best_sofar);
    return best_sofar;
}

// add a task to the list 
void add(char *name, int priority, int burst) {
    Task *temp = (Task *)malloc(sizeof(Task));
    temp->name = name;
    temp->priority = priority;
    temp->burst = burst;
    temp->tid = tid;
    tid++; 

    insert(&g_head, temp);
}

// invoke the scheduler
void schedule() {
    Task *temp = pickNextTask();
    while (temp != NULL) {
        if (temp->burst >= QUANTUM) {
            run(temp, QUANTUM);
            temp->burst = temp->burst - QUANTUM;
            currentTime = currentTime + QUANTUM;
            prev = temp;
        } else {
            run(temp, temp->burst);
            currentTime = currentTime + temp->burst;
            temp->burst = 0;
            prev = temp;
            free(temp);
        }
        printf("        Time is now: %d\n", currentTime);
        temp = pickNextTask();
        if (temp != NULL && prev != temp) {
          util++;
        }
    }
    printf("CPU Utilizatoin: %.2f%%\n", 
      100*((float)currentTime / ((float)currentTime + (float)util)));
}