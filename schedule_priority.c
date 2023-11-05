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

//get highest priority job
bool isImportant(int a, int b){ return (a < b); }

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

//schedule the exectuion
void schedule(){
    while(list != NULL){
        Task *toRun = pickNextTask();
        run(toRun, toRun -> burst);
        delete(&list, toRun);
    }
}