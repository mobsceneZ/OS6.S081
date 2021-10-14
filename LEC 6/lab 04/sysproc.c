#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);

  backtrace();
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

uint64
sys_sigreturn(void)
{
  struct proc *p = myproc();

  p->trapframe->sp = p->save_regs[0];
  p->trapframe->s0 = p->save_regs[1];
  p->trapframe->ra = p->save_regs[2];
  p->trapframe->a4 = p->save_regs[4];
  p->trapframe->a5 = p->save_regs[5];
  p->trapframe->epc = p->save_regs[6];
  p->trapframe->s1 = p->save_regs[7];
  p->trapframe->a1 = p->save_regs[8];
  p->trapframe->a2 = p->save_regs[9];

  p->in_handler = 0;
  return p->save_regs[3];
}

uint64
sys_sigalarm(void)
{
  int ticks;
  uint64 handler;
  struct proc *p = myproc();

  if(argint(0, &ticks) < 0)
    return -1;
  p->total_ticks = ticks;
  if(argaddr(1, &handler) < 0)
    return -1;
  p->handler = handler;
  p->current_ticks = 0;

  return 0; 
}
