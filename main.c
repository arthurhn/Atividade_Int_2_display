#include <stdint.h>
#include "Nokia5110.h"

#include "inc/tm4c123gh6pm.h"
#include "driverlib/debug.h"
#include "driverlib/timer.h"
#include "inc/hw_gpio.h"
#include "driverlib/adc.h"

#include <stdlib.h>
#include<time.h>
#include "bitmaps.h"

#include "driverlib/hibernate.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"

#define red GPIO_PIN_1
#define blue GPIO_PIN_2
#define green GPIO_PIN_3
#define sw_1 GPIO_PIN_4
#define sw_2 GPIO_PIN_0
#define LOW 0x00
#define HIGH 0xFF

int estado, tempo, lock, MAX, MIN, AUX;

void trata_Systick(){
    tempo--;
}

void tela1(int ds, int d, int m, int h, int mi){

    Nokia5110_PrintBMP2(60,1,bar,5,5);
    Nokia5110_DisplayBuffer();

    while(tempo > 0){

//dia da semana
        Nokia5110_PrintBMP2(11,1,semana[ds],35,5);
        Nokia5110_DisplayBuffer();


//dia do mes nao deram problema
        Nokia5110_PrintBMP2(52,1,numeros[d/10],3,5);
        Nokia5110_DisplayBuffer();

        Nokia5110_PrintBMP2(56,1,numeros[d%10],3,5);
        Nokia5110_DisplayBuffer();

        Nokia5110_PrintBMP2(66,1,numeros[m/10],3,5);
        Nokia5110_DisplayBuffer();

        Nokia5110_PrintBMP2(70,1,numeros[m/10],3,5);
        Nokia5110_DisplayBuffer();

//horas
        Nokia5110_PrintBMP2(11,13,numg[(h+3)/10],13,24);
        Nokia5110_DisplayBuffer();

        Nokia5110_PrintBMP2(25,13,numg[(h+3)%10],13,24);
        Nokia5110_DisplayBuffer();

//pontos piscando
        if(tempo%2 == 0){
        Nokia5110_PrintBMP2(40,18,on,5,15);
        Nokia5110_DisplayBuffer();
        }else{
            Nokia5110_PrintBMP2(40,18,off,5,15);
            Nokia5110_DisplayBuffer();
        }

        Nokia5110_PrintBMP2(47,13,numg[(mi)/10],13,24);
        Nokia5110_DisplayBuffer();

        Nokia5110_PrintBMP2(61,13,numg[mi%10],13,24);
        Nokia5110_DisplayBuffer();

    }

    if(tempo == 0){
        lock = 0;
        estado++;
        SysTickDisable();
    }
}

void tela2(float tnow, float tx, float tn){
    Nokia5110_PrintBMP2(53,31,dot,5,5);
    Nokia5110_DisplayBuffer();

    MAX = (int) 10* tx;
    MIN = (int) 10* tn;
    AUX = (int) 10* tnow;

    while(tempo > 0){

//termometro
        if(AUX/100 >= 0 && AUX/100 <= 1){
            Nokia5110_PrintBMP2(12,8,termo[0],9,28);
            Nokia5110_DisplayBuffer();
        }else if(AUX/100 >=1  && AUX/100 <= 2){
            Nokia5110_PrintBMP2(12,8,termo[1],9,28);
            Nokia5110_DisplayBuffer();
        }else if(AUX/100 >=2  && AUX/100 <= 3){
            Nokia5110_PrintBMP2(12,8,termo[2],9,28);
            Nokia5110_DisplayBuffer();
        }else if(AUX/100 >=3  && AUX/100 <= 4){
            Nokia5110_PrintBMP2(12,8,termo[3],9,28);
            Nokia5110_DisplayBuffer();
    }

//numeros
        Nokia5110_PrintBMP2(25,12,numg[AUX/100],13,24);
        Nokia5110_DisplayBuffer();

        Nokia5110_PrintBMP2(39,12,numg[(AUX%100)/10],13,24);
        Nokia5110_DisplayBuffer();

        Nokia5110_PrintBMP2(59,12,numg[AUX%10],13,24);
        Nokia5110_DisplayBuffer();

        }

    if(tempo == 0){
        lock = 0;
        estado++;
        SysTickDisable();
    }
}

void tela3(){

    Nokia5110_PrintBMP2(0,0,screen3,84,48);
    Nokia5110_DisplayBuffer();

    while(tempo > 0){
//max
        Nokia5110_PrintBMP2(43,23,numeros[MAX/100],3,5);
        Nokia5110_DisplayBuffer();

        Nokia5110_PrintBMP2(47,23,numeros[(MAX%100)/10],3,5);
        Nokia5110_DisplayBuffer();

        Nokia5110_PrintBMP2(53,23,numeros[MAX%10],3,5);
        Nokia5110_DisplayBuffer();

//agr
        Nokia5110_PrintBMP2(43,30,numeros[AUX/100],3,5);
        Nokia5110_DisplayBuffer();

        Nokia5110_PrintBMP2(47,30,numeros[(AUX%100)/10],3,5);
        Nokia5110_DisplayBuffer();

        Nokia5110_PrintBMP2(53,30,numeros[AUX%10],3,5);
        Nokia5110_DisplayBuffer();

//min
        Nokia5110_PrintBMP2(43,37,numeros[MIN/100],3,5);
        Nokia5110_DisplayBuffer();

        Nokia5110_PrintBMP2(47,37,numeros[(MIN%100)/10],3,5);
        Nokia5110_DisplayBuffer();

        Nokia5110_PrintBMP2(53,37,numeros[MIN%10],3,5);
        Nokia5110_DisplayBuffer();

        }

    if(tempo == 0){
        lock = 0;
        estado = 0;
        SysTickDisable();
    }
}

int main(void){

    Nokia5110_Init();
    SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, red|blue|green);

    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= 0x01;
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;
    GPIODirModeSet(GPIO_PORTF_BASE, sw_1|sw_2, GPIO_DIR_MODE_IN);
    GPIOPadConfigSet(GPIO_PORTF_BASE, sw_1|sw_2, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_INT_PIN_0|GPIO_INT_PIN_4,GPIO_FALLING_EDGE);
    IntEnable(INT_GPIOF);
    GPIOIntEnable(GPIO_PORTF_BASE, GPIO_INT_PIN_0|GPIO_INT_PIN_4);
    IntMasterEnable();

    int dia = 0, mes = 0, hora = 0, min = 0, dsem = 0;
    estado = 0; lock = 0;

//tempo
        time_t t = time(NULL);

        struct tm* timePtr = localtime(&t);
        SysCtlPeripheralEnable(SYSCTL_PERIPH_HIBERNATE);
        HibernateEnableExpClk(SysCtlClockGet());
        HibernateRTCSet(t);
        HibernateRTCEnable();

        dsem = timePtr->tm_wday;
        dia    = timePtr->tm_mday;
        mes        = timePtr->tm_mon + 1;
        hora       = timePtr->tm_hour;
        min    = timePtr->tm_min;

//temperatura
	    float aux, max = 0, tmin = 100;

	    uint32_t temp_vetor[1];

	    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
        ADCSequenceConfigure(ADC0_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);
        ADCSequenceStepConfigure(ADC0_BASE, 1, 0, ADC_CTL_TS);
        ADCSequenceStepConfigure(ADC0_BASE, 1, 1, ADC_CTL_TS);
        ADCSequenceStepConfigure(ADC0_BASE, 1, 2, ADC_CTL_TS);
        ADCSequenceStepConfigure(ADC0_BASE,1,3,ADC_CTL_TS|ADC_CTL_IE|ADC_CTL_END);
        ADCSequenceEnable(ADC0_BASE, 1);

	while(1){

        ADCIntClear(ADC0_BASE, 1);
        ADCProcessorTrigger(ADC0_BASE, 1);
        while(!ADCIntStatus(ADC0_BASE, 1, false)){}


        ADCSequenceDataGet(ADC0_BASE, 1, temp_vetor);
        aux = (1475 - ((2475 * (float)temp_vetor[0])) / 4096.0)/10.0;

// Obtem temperaturas maximas e minimas
        if(aux > max) max = aux;
        if(aux < tmin) tmin = aux;


	    if (estado >= 0 && estado <= 2 && lock == 0){
	                lock = 1;
	                SysTickIntEnable();
	                SysTickPeriodSet ((SysCtlClockGet() / 3600 )* 10000000);
	                SysTickEnable ();
	            }

	            if (estado == 0){
	                tempo = 35;
	                Nokia5110_ClearBuffer();
	                tela1(dsem, dia, mes, hora, min);
	            }else if (estado == 1){
	                tempo = 35;
	                Nokia5110_ClearBuffer();
	                tela2(aux,max,tmin);
	            }else if (estado == 2){
	                tempo = 35;
	                Nokia5110_ClearBuffer();
	                tela3();
	            }
	}
}
