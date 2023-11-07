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

//metric storage (it is bigger than it needs to be, just being safe)
char metrics[1000] = "...|TAT| WT| RT|\n";

void add(char *name, int priority, int burst){
    Task *newTask = malloc(sizeof(Task));

    //setup task
    newTask -> name = name;
    newTask -> priority = priority;
    newTask -> burst = burst;
    //setup metrics
    newTask -> wait = 0;
    newTask -> response = -1;
    newTask -> tat = 0;
    newTask -> last = -1;

    //add to back of list
    insert(&list, newTask);
}

//lexographically pick tasks
bool comesAfter(char *a, char *b){ return strcmp(a, b) >= 0; }

//pick the shortest task in the set, like FCFS
Task *makeRunQueue(){
    //list is empty
    if(!list){return NULL;}

    struct node *temp = list;
    Task *bestSoFar = temp -> task;

    //get the best lexographically first option, as they all come in at the same time, FCFS is more the name
    while(temp != NULL){
        if(comesAfter(temp -> task -> name, bestSoFar -> name)) {bestSoFar = temp -> task;}
        temp = temp -> next;
    }

    delete(&list, bestSoFar);

    return bestSoFar;
} 

//pick the shortest task in the set, like FCFS
Task *pickNextTask(){
    Task *thisTurn = list -> task;
    //setup next round
    if(list -> next != NULL){
        list = list -> next;
    }else{
        list = queue;
    }
    return thisTurn;
} 

//apend the tasks metric to the global string to be printed once all tasks are completed runnning 
void appendMetric(Task *thisTask){
    thisTask -> wait --; //account for dispatch intialization preset
    char addLine[50];
    sprintf(addLine, " %s| %d | %d | %d|\n", thisTask -> name, thisTask -> tat, thisTask -> wait, thisTask -> response);
    strcat(metrics, addLine);
}

//schedule the exectuion
void schedule(){
    //setup the run queue for round robin, based on FCFS
    while(list != NULL){
        insert(&queue, makeRunQueue());
    }
    list = queue;

    //setup for CPU utilization
    double tTime = 0;
    double nDispatch = -1;

    //round robin run queue
    while(queue != NULL){
        Task *temp = pickNextTask();
        //for metrics table
        if(temp -> response < 0){temp -> response = tTime;}
        temp -> wait += tTime - temp -> last;

        //run quantum or less
        if(temp -> burst > QUANTUM){
            tTime += QUANTUM;
            run(temp, QUANTUM);
            temp -> burst -= QUANTUM;
        }else if (temp -> burst <= QUANTUM){
            tTime += temp -> burst;
            run(temp, temp -> burst);
            //update metrics before deleting from list
            temp -> tat = tTime;
            appendMetric(temp);
            delete(&queue, temp);
        }
        nDispatch++;
        temp -> last = tTime;
    }

     //CPU Utilization time
    double cpuUtil = 100 * (tTime / (tTime + nDispatch));
    printf("CPU Utilization: %.2f%%\n", cpuUtil);

    //print metrics
    printf("%s\n", metrics);
}