#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

/* Possible states of a process: */
#define FREE        0x0
#define RUNNING     0x1
#define RUNNABLE    0x2

#define STACK_SIZE  8192
#define NPROCS  4

struct context {
  uint64 ra;
  uint64 sp;

  // callee-saved
  uint64 s0;
  uint64 s1;
  uint64 s2;
  uint64 s3;
  uint64 s4;
  uint64 s5;
  uint64 s6;
  uint64 s7;
  uint64 s8;
  uint64 s9;
  uint64 s10;
  uint64 s11;
};

struct process {
  struct context context;
  char       stack[STACK_SIZE]; /* the process's stack */
  int        state;             /* FREE, RUNNING, RUNNABLE */
};

struct process ptable[NPROCS];
struct process *current_process;
extern void context_switch(uint64, uint64);
              
void 
process_init(void)
{
  // main() is process 0, which will make the first invocation to
  // scheduler().  it needs a stack so that the first context_switch() can
  // save process 0's state.  scheduler() won't run the main process ever
  // again, because its state is set to RUNNING, and scheduler() selects
  // a RUNNABLE process.
  current_process = &ptable[0];
  current_process->state = RUNNING;
}

void 
scheduler(void)
{
  struct process *p, *next;
  p = current_process + 1;
  next = p;

  for (int i = 0; i < NPROCS; i++)
  {
    // if reach the end of table, reset p to start of table
    if (i == NPROCS-1)
    {
      p = &ptable[0];
    }
    
    // if p is runnable, set next to p and break out of this loop
    if (p->state == RUNNABLE)
    {
      // next->state = p->state;
      next = p;
      break;
    }

    // update p to keep traversing the table
    p = &ptable[i];
  }

  // if no runnable process is found, print statement and exit
  if (p->state != RUNNABLE)
  {
    printf("scheduler: no runnable processes\n");
    exit(0);
  }

  // if current process is NOT next, then perform context switch stuff
  if (p != current_process)
  {
    next->state = RUNNING;
    p = current_process;
    current_process = next;
    context_switch((uint64) &p->context, (uint64) &current_process->context);
  }
}

void 
sched(void (*func)())
{
  struct process *p;
  for (p = ptable; p < ptable + NPROCS; p++)
  {
    // if p state is FREE, break out of loop
    if (p->state == FREE)
    {
      break;
    }
  }

  // rest of function
  p->state = RUNNABLE;

  // set the ra and sp registers
  // set ra to function being passed in
  // set sp to whatever p's stack is adding onto it stack size
  p->context.ra = (uint64)(*func);
  p->context.sp = (uint64)p->stack + STACK_SIZE;
}

void 
yield(void)
{
  current_process->state = RUNNABLE;
  scheduler();
}

volatile int a_started, b_started, c_started;
volatile int a_n, b_n, c_n;

void 
process_a(void)
{
  int i;
  printf("process_a started\n");
  a_started = 1;
  while(b_started == 0 || c_started == 0)
    yield();
  
  for (i = 0; i < 100; i++) {
    printf("process_a %d\n", i);
    a_n += 1;
    yield();
  }
  printf("process_a: exit after %d\n", a_n);

  current_process->state = FREE;
  scheduler();
}

void 
process_b(void)
{
  int i;
  printf("process_b started\n");
  b_started = 1;
  while(a_started == 0 || c_started == 0)
    yield();
  
  for (i = 0; i < 100; i++) {
    printf("process_b %d\n", i);
    b_n += 1;
    yield();
  }
  printf("process_b: exit after %d\n", b_n);

  current_process->state = FREE;
  scheduler();
}

void 
process_c(void)
{
  int i;
  printf("process_c started\n");
  c_started = 1;
  while(a_started == 0 || b_started == 0)
    yield();
  
  for (i = 0; i < 100; i++) {
    printf("process_c %d\n", i);
    c_n += 1;
    yield();
  }
  printf("process_c: exit after %d\n", c_n);

  current_process->state = FREE;
  scheduler();
}

int 
main(int argc, char *argv[]) 
{
  a_started = b_started = c_started = 0;
  a_n = b_n = c_n = 0;
  process_init();
  sched(process_a);
  sched(process_b);
  sched(process_c);
  scheduler();
  exit(0);
}