#define MAX_PROC 10

struct ps_proc {
    int state;
    int pid;
    int priority;
    char name[16];
    int ready;
    int running;
    int waiting;
};