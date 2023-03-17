#ifndef __PREEMPTIVE_H__
#define __PREEMPTIVE_H__

#define MAXTHREADS 4  

#define CNAME(s) _ ## s

__data __at (0x20) char car;
__data __at (0x21) char car_temp;
__data __at (0x22) char car_name[4];

__data __at (0x26) char empty;
__data __at (0x27) char mutex;
__data __at (0x28) char thread;
__data __at (0x29) char space1;
__data __at (0x2A) char space2;

__data __at (0x30) unsigned char time;
__data __at (0x31) unsigned char time_unit;
__data __at (0x32) unsigned char time_car[6];
__data __at (0x38) unsigned char time_temp[4];

__data __at (0x2B) char saved_sp[4];
__data __at (0x2F) char mask;
__data __at (0x3C) char cur_thread;
__data __at (0x3D) char i;
__data __at (0x3E) char temp;
__data __at (0x3F) char new_thread;

#define SemaphoreCreate(s, n)\
            s = n\                               

#define SemaphoreSignal(s)\
            __asm \
            INC CNAME(s)\
            __endasm;\


#define SemaphoreWaitBody(s, label) \
    { __asm \
        label:\
        MOV A, CNAME(s)\
        JZ  label\
        JB ACC.7, label\
        dec  CNAME(s) \
      __endasm; }

typedef char ThreadID;
typedef void (*FunctionPtr)(void);

ThreadID ThreadCreate(FunctionPtr);
void ThreadYield(void);
void ThreadExit(void);
void myTimer0Handler();

unsigned char now(void);


#endif // __PREEMPTIVE_H__