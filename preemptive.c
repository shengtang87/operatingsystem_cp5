#include <8051.h>

#include "preemptive.h"

/*
 * @@@ [2 pts] declare the static globals here using 
 *        __data __at (address) type name; syntax
 * manually allocate the addresses of these variables, for
 * - saved stack pointers (MAXTHREADS)
 * - current thread ID
 * - a bitmap for which thread ID is a valid thread; 
 *   maybe also a count, but strictly speaking not necessary
 * - plus any temporaries that you need.
 */



/*
 * @@@ [8 pts]
 * define a macro for saving the context of the current thread by
 * 1) push ACC, B register, Data pointer registers (DPL, DPH), PSW
 * 2) save SP into the saved Stack Pointers array
 *   as indexed by the current thread ID.
 * Note that 1) should be written in assembly, 
 *     while 2) can be written in either assembly or C
 */
#define SAVESTATE {\
         __asm \
            PUSH ACC\
            PUSH B\
            PUSH DPL\
            PUSH DPH\
            PUSH PSW\
         __endasm; \
         saved_sp[cur_thread] = SP;\
        }
         
#define RESTORESTATE {\
            SP = saved_sp[cur_thread];\
            __asm \
               POP PSW\
               POP DPH\
               POP DPL\
               POP B\
               POP ACC\
            __endasm; \
         }



extern void main(void);


void Bootstrap(void) {
      /*
       * @@@ [2 pts] 
       * initialize data structures for threads (e.g., mask)
       *
       * optional: move the stack pointer to some known location
       * only during bootstrapping. by default, SP is 0x07.
       *
       * @@@ [2 pts]
       *     create a thread for main; be sure current thread is
       *     set to this thread ID, and restore its context,
       *     so that it starts running main().
       */
      mask = 0;
      TMOD = 0;  // timer 0 mode 0
      IE = 0x82;  // enable timer 0 interrupt; keep consumer polling
                  // EA  -  ET2  ES  ET1  EX1  ET0  EX0
      TR0 = 1; // set bit TR0 to start running timer 0
      cur_thread = ThreadCreate(main);
      RESTORESTATE;
}

ThreadID ThreadCreate(FunctionPtr fp) {
   EA = 0;
   if(mask==15)
      return -1;
   //a,b
   if( !( mask & 1 ) ){
      mask = mask | 1;
      new_thread = 0;
   }
   else if( !( mask & 2 ) ){
      mask = mask | 2;
      new_thread = 1;
   }
   else if( !( mask & 4 ) ){
      mask = mask | 4;
      new_thread = 2;
   }
   else if( !( mask & 8 ) ){
      mask = mask | 8;
      new_thread = 3;
   }
   //c
   temp = SP;
   SP = (0x3F) + (0x10) * new_thread;
   //d
   __asm
      PUSH DPL
      PUSH DPH
   __endasm;
   //e
   __asm 
      ANL A, #0
      PUSH ACC
      PUSH ACC
      PUSH ACC
      PUSH ACC
   __endasm;
   //f
   PSW = new_thread << 3;
   __asm 
      PUSH PSW
   __endasm;
   //g
   saved_sp[new_thread] = SP;
   //h
   SP = temp;
   EA = 1;
   //i
   return new_thread;         
}




void ThreadYield(void) {
   EA = 0;   
   SAVESTATE;
   do{
      if(cur_thread == 3) cur_thread = 0;
      else cur_thread += 1;
      if( cur_thread == 0 && mask & 1) break;
      else if( cur_thread == 1 && mask & 2) break;
      else if( cur_thread == 2 && mask & 4) break;
      else if( cur_thread == 3 && mask & 8) break;
   }while(1);
   RESTORESTATE;
   EA = 1;   
}



void ThreadExit(void) {
   EA = 0;   
   mask ^= (1<<cur_thread);
   if(mask == 0){
      while(1){}
   }
   else{
      do{
         if(cur_thread == 3) cur_thread = 0;
         else cur_thread += 1;
         if( cur_thread == 0 && mask & 1) break;
         else if( cur_thread == 1 && mask & 2) break;
         else if( cur_thread == 2 && mask & 4) break;
         else if( cur_thread == 3 && mask & 8) break;
      }while(1);
   }
   RESTORESTATE;
   EA = 1;
}


void myTimer0Handler(){
      EA = 0;
      SAVESTATE;
      __asm
         MOV A, R0
         PUSH ACC
         MOV A, R1
         PUSH ACC
         MOV A, R2
         PUSH ACC
         MOV A, R3
         PUSH ACC
         MOV A, R4
         PUSH ACC
         MOV A, R5
         PUSH ACC
         MOV A, R6
         PUSH ACC
         MOV A, R7
         PUSH ACC
      __endasm;
      
      time_unit += 1;
      if(time_unit==16){
         time += 1;
         time_unit = 0;
      }
      
      do{
         if(cur_thread == 3) cur_thread = 0;
         else cur_thread += 1;
         if( cur_thread == 0 && mask & 1) break;
         else if( cur_thread == 1 && mask & 2) break;
         else if( cur_thread == 2 && mask & 4) break;
         else if( cur_thread == 3 && mask & 8) break;
      }while(1);
      __asm
         POP ACC
         MOV R7, A
         POP ACC
         MOV R6, A
         POP ACC
         MOV R5, A
         POP ACC
         MOV R4, A
         POP ACC
         MOV R3, A
         POP ACC
         MOV R2, A
         POP ACC
         MOV R1, A
         POP ACC
         MOV R0, A
      __endasm;  
      RESTORESTATE;
      EA = 1;
      __asm 
         RETI
      __endasm;       
}