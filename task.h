/**
 * Representation of a task in the system.
 */

#ifndef TASK_H
#define TASK_H

// representation of a task
typedef struct task {
    char *name;
    int tid;
    int priority;
    int burst;
    //metrics for scheudler
    int wait;
    int response; //-1 = base case, check for errors
    int tat;
    int last; //only used with round robin schedulers
} Task;

#endif
