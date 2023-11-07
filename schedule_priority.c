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

    //add to back of list
    insert(&list, newTask);
}

//lexographically pick tasks
bool comesBefore(char *a, char *b){ return strcmp(a, b) < 0; }

//get highest priority job
bool isImportant(int a, int b){ return (a > b); }

//help organization for printing, but maintain correctness
bool isEqual(int a, int b) { return (a == b); }

//pick the highest priority task in the set
Task *pickNextTask(){
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

    return bestSoFar;
} 

//apend the tasks metric to the global string to be printed once all tasks are completed runnning 
void appendMetric(Task *thisTask){
    char addLine[50];
    sprintf(addLine, " %s| %d | %d | %d|\n", thisTask -> name, thisTask -> tat, thisTask -> wait, thisTask -> response);
    strcat(metrics, addLine);
}

//schedule the exectuion
void schedule(){
    //setup for CPU utilization
    double tTime = 0;
    double nDispatch = -1;
    
    // scheduler run queue
    while(list != NULL){
        Task *toRun = pickNextTask();

        //add task metrics for wait time and response time 
        if(toRun -> response < 0){toRun -> response = tTime;}
        toRun -> wait = tTime;

        run(toRun, toRun -> burst);
        //increment values for cpu utilization
        nDispatch++; //assuming the dispatcher takes 1 unit of time
        tTime += toRun -> burst;

        //add metrics to print string before deleting from list
        toRun -> tat = tTime;
        appendMetric(toRun);
        delete(&list, toRun);
    }

    //CPU Utilization time
    double cpuUtil = 100 * (tTime / (tTime + nDispatch));
    printf("CPU Utilization: %.2f%%\n", cpuUtil);

    //print metrics
    printf("%s\n", metrics);
}