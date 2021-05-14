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
int util = 0;
Task *prev = NULL;

bool comesBefore(char *a, char *b) { return strcmp(a, b) < 0; }

// based on traverse from list.c
// finds the task whose name comes first in dictionary
Task *pickNextTask() {
  // if list is empty, nothing to do
  if (!g_head)
    return NULL;

  struct node *temp;
  temp = g_head;
  Task *best_sofar = temp->task;

  while (temp != NULL) {
    // check if temp burst <= best_Sf burst.
    if (temp->task->burst <= best_sofar->burst) {
        if (temp->task->burst == best_sofar->burst) { // check if temp->burst == best_SF->burst.
            if (comesBefore(temp->task->name, best_sofar->name)) { //  if it is, do the comes before.
                best_sofar = temp->task;
            }
        } else { // if not, set best_sf = temp->task
            best_sofar = temp->task;
        }
    }
    temp = temp->next;
  }
  // delete the node from list, Task will get deleted later
  delete (&g_head, best_sofar);
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
        run(temp, temp->burst);
        currentTime = currentTime + temp->burst;
        printf("        Time is now: %d\n", currentTime);
        prev = temp;
        free(temp);
        temp = pickNextTask();
        if (temp != NULL && prev != temp) {
          util++;
        }
    }
    printf("CPU Utilizatoin: %.2f%%\n", 
      100*((float)currentTime / ((float)currentTime + (float)util)));
}



