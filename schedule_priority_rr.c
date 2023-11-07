#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

//file attatch
#include "schedulers.h"
#include "list.h"
#include "cpu.h"
#include "task.h"

//list storage
struct node *list = NULL;
struct node *queue = NULL;

void add(char *name, int priority, int burst){
    Task *newTask = malloc(sizeof(Task));

    //setup task
    newTask -> name = name;
    newTask -> priority = priority;
    newTask -> burst = burst;

    //add to back of list
    insert(&list, newTask);
}

//lexographically pick tasks
bool comesBefore(char *a, char *b){ return strcmp(a, b) > 0; }

//get highest priority job
bool isImportant(int a, int b){ return (a > b); }

//help organization for printing, but maintain correctness
bool isEqual(int a, int b) { return (a == b); }

//pick the build run queue based on priority
Task *makeRunQueue(){
    //list is empty
    if(!list){return NULL;}

    struct node *temp = list;
    Task *bestSoFar = temp -> task;

    //get the best lexographically first option, as they all come in at the same time, FCFS is more the name
    while(temp != NULL){
        if(isEqual(temp -> task -> priority, bestSoFar -> priority)) {
            if(comesBefore(temp -> task -> name, bestSoFar -> name)) {bestSoFar = temp -> task; }
        }
        if(isImportant(temp -> task -> priority, bestSoFar -> priority)) {bestSoFar = temp -> task;}
        temp = temp -> next;
    }

    delete(&list, bestSoFar);

    return bestSoFar;
} 

//move through the queue to round robin, if next item is different priority, go back
Task *pickNextTask(){
    if(!list){return NULL;}

    Task *thisTurn = list -> task;

    if(list -> next == NULL || thisTurn -> burst <= QUANTUM ){
        list = queue;
    }else if(list -> next -> task -> priority != thisTurn -> priority){
        list = queue;
    }else{
        list = list -> next;
    }

    return thisTurn; //basecase
} 

//schedule the exectuion
void schedule(){
    //setup the run queue for round robin, based on priority
    while(list != NULL){
        Task *copy = makeRunQueue();
        insert(&queue, copy);
    }
    list = queue;

        //setup for CPU utilization
    double tTime = 0;
    double nDispatch = -1;

    //round robin run queue
    while(queue != NULL){
        Task *temp = pickNextTask();
        if(temp -> burst > QUANTUM){
            run(temp, QUANTUM);
            tTime += QUANTUM;
            temp -> burst -= QUANTUM;
        }else if (temp -> burst <= QUANTUM){
            run(temp, temp -> burst);
            tTime += temp -> burst;
            delete(&list, temp);
            delete(&queue, temp);
        }
        nDispatch++;
    }

    //CPU Utilization time
    double cpuUtil = 100 * (tTime / (tTime + nDispatch));
    printf("CPU Utilization: %.2f%%\n", cpuUtil);
}