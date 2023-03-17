#include <8051.h>
#include "preemptive.h"


#define L(x) LABEL(x)

#define LABEL(x) x##$

// void delay(unsigned char n){
//     time_temp[cur_thread] = time + n;
//     while( time != time_temp[cur_thread] ){}
// }

#define delay(n)\
        time_temp[cur_thread] = time + n ;\
        while( time != time_temp[cur_thread] ){}\

unsigned char now(void){
        return time;
}




#define log(num,type,space){\
        for(i = 0; i < 8 ; i++){\
                if( i == 0 ) SBUF = num;\
                else if( i == 1 ) SBUF = type;\
                else if( i == 2 ) SBUF = space;\
                else if( i == 3 ) SBUF = ':';\
                else if( i == 4 ) SBUF = ' ';\
                else if( i == 5 ){\
                        temp = (now()&240)>>4;\
                        if( temp >= 10) SBUF = temp - 10 + 'A';\
                        else SBUF = temp + '0';\
                }\
                else if( i == 6 ){\
                        temp = (now()&15);\
                        if( temp >= 10) SBUF = temp - 10 + 'A';\
                        else SBUF = temp + '0';\
                }\
                else if( i == 7 ) SBUF = '\n';\
                while( !TI ){}\
                TI = 0;\
        }\
}\

void Producer(void) {
        SemaphoreWaitBody(empty, L(__COUNTER__) );
        SemaphoreWaitBody(mutex, L(__COUNTER__) );
        EA = 0;
        if(space1=='0'){
                space1 = car_name[cur_thread];
                log(car_name[cur_thread],'P','1');
        }
        else if (space2=='0'){
                space2 = car_name[cur_thread];
                log(car_name[cur_thread],'P','2');
        } 

        EA = 1;
        SemaphoreSignal(mutex);

        i = car_name[cur_thread]-'0';
        delay(time_car[i]);
        // delay(2);
        
        EA = 0;
        if(space1==car_name[cur_thread]){
                space1 = '0';
                log(car_name[cur_thread],'L','1');
        }
        else if (space2==car_name[cur_thread]){
                space2 = '0';
                log(car_name[cur_thread],'L','2');
        }
        EA = 1;
        SemaphoreSignal(empty);
        SemaphoreSignal(thread);
        ThreadExit();
}



/* [5 pts for this function]
 * main() is started by the thread bootstrapper as thread-0.
 * It can create more thread(s) as needed:
 * one thread can act as producer and another as consumer.
 */

void main(void) {
        SemaphoreCreate(mutex, 1);
        SemaphoreCreate(empty, 2);
        SemaphoreCreate(thread, 3);

        for(i=1 ; i<=5; i++){
                if(i==2) time_car[i] = 10;
                else time_car[i] = 4;
        }
        car = '1';
        space1 = '0';
        space2 = '0';

        TMOD |= 0x20;\
        TR1 = 1;\
        TH1 = -6;\
        SCON = 0x50;\

        while(time<=0x30){
                SemaphoreWaitBody(thread, L(__COUNTER__) );
                car_temp = ThreadCreate(Producer);
                car_name[car_temp] = car;
                if(car == '5') car = '1';
                else car += 1;
        }
        ThreadExit();
}

void _sdcc_gsinit_startup(void) {
        __asm
                ljmp  _Bootstrap
        __endasm;
}

void _mcs51_genRAMCLEAR(void) {}
void _mcs51_genXINIT(void) {}
void _mcs51_genXRAMCLEAR(void) {}
void timer0_ISR(void) __interrupt(1) {
        __asm
            ljmp _myTimer0Handler
        __endasm;
}