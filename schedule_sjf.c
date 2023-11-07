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
bool comesBefore(char *a, char *b){ return strcmp(a, b) < 0; }

//get shortest job
bool isShorter(int a, int b){ return (a < b); }

//help organization for printing, but maintain correctness
bool isEqual(int a, int b) { return (a == b); }

//pick the shortest task in the set
Task *pickNextTask(){
    //list is empty
    if(!list){return NULL;}

    struct node *temp = list;
    Task *bestSoFar = temp -> task;

    //get the best lexographically first option, as they all come in at the same time, FCFS is more the name
    while(temp != NULL){
        if(isEqual(temp -> task -> burst, bestSoFar -> burst)) {
            if(comesBefore(temp -> task -> name, bestSoFar -> name)) {bestSoFar = temp -> task; }
        }
        if(isShorter(temp -> task -> burst, bestSoFar -> burst)) {bestSoFar = temp -> task;}
        temp = temp -> next;
    }

    return bestSoFar;
} 

//schedule the exectuion
void schedule(){
    //setup for CPU utilization
    double tTime = 0;
    double nDispatch = -1;
    
    //scheudler run queue
    while(list != NULL){
        Task *toRun = pickNextTask();
        run(toRun, toRun -> burst);
        //increment values for cpu utilization
        nDispatch++; //assuming the dispatcher takes 1 unit of time
        tTime += toRun -> burst;
        delete(&list, toRun);
    }

     //CPU Utilization time
    double cpuUtil = 100 * (tTime / (tTime + nDispatch));
    printf("CPU Utilization: %.2f%%\n", cpuUtil);
}