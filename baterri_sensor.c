/*
ESCRITO POR : MANUEL ALFONSO MANJARRES RIVERA
FECHA:13/03/2021
*/

#define FACTOR (66.0/1000)
#define QOV (0.5*5.0)
#define RESOL (5.0/1023.0)
#define OFFSET_MAG (25/1000)
#define offset_estandar 0.5
#define TRUE 1
#define RESISTOR_SERIE 10000
#define KELVIN 273.15
#define MAXIMO 250
#define TIME_  500

sbit offset_mas at RB1_bit;
sbit offset_menos at RB2_bit;

/*VARIABLES GLOBALES*/
int offset=0;

/*PROTOTIPOS DE FUNCIONES*/
void configuracion(void);
void interrupcion();
int ADC_lectura(const char  bit0,const char bit1,const char bit2,const char  bit3);
float prom_corrinte( const unsigned int max);
float lectura_WSC1800(void);
float lectura_voltaje(void);
float lectura_temperatura(void);

void main(void) {

      float corriente_promedio=0.0;
      float voltaje=0.0;
      float temperatura=0.0;
      unsigned int maximo=0;
      unsigned char str[16]={0};

    configuracion(void);
   
       UART1_Init(9600);
        Delay_ms(50);
       UART1_Write_Text("LABEL,Time,V_bateria,I_bateria,T_bateria\r\n");
   
     while(TRUE){
             Delay_ms(TIME_);
             UART1_Write_Text("DATA,TIME,");
     
          corriente_promedio=prom_corrinte(300);
             FloatToStr(corriente_promedio,str);
             UART1_Write_Text(str);
             UART1_Write_Text(",") ;
          
          voltaje=lectura_voltaje(void);
             floatToStr(voltaje,str);
             UART1_Write_Text(str);
             UART1_Write_Text(",");

           temperatura=lectura_temperatura(void);
            floatToStr(temperatura,str);
             UART1_Write_Text(str);
             UART1_Write_Text("\r\n") ;


             while(maximo==MAXIMO);
             maximo++;
          }

}/*FIN DEL PROGRAMA PRINCIPAL*/

/*LECTURA DEL ADC*/
int ADC_lectura(const char  bit0,const char bit1,const char bit2,const char  bit3){

     unsigned  int pasos=0;
     CHS0_bit=bit3;
     CHS1_bit=bit2;
     CHS2_bit=bit1;
     CHS3_bit=bit0;
     GO_DONE_bit=1;
    while(GO_DONE_bit);
     pasos= (ADRESH<<8) | (ADRESL);

       return(pasos);
   }

  /*LECTURA DEL SENSOR WSC1800*/
float lectura_WSC1800(void){
      float voltaje=0.0;
      float corriente=0.0;
      float voltaje_puro=0.0;
      int pasos=ADC_lectura(0,1,0,1);

     if(pasos>=512){
        voltaje_puro=(float)pasos*RESOL;
        voltaje=(float)voltaje_puro-QOV;
        corriente=(float)(voltaje/FACTOR)-(OFFSET_MAG);

        }else corriente=0.0;

     return(corriente);
    }

/*CALCULAMOS EL PROMEDIO DE LAS MEDICIONES DE CORRIENTE*/
float prom_corrinte( const unsigned int max){
       unsigned int cont=0;
       float sum=0.0;
       float aux=0.0;

     for(cont=0;cont<max;cont++){
             aux=lectura_WSC1800(void);
             sum+=(aux-(offset_estandar+((float)offset/10)));
         }

      return ((sum/cont));
    }
    
float lectura_voltaje(void){
      unsigned int pasos=0;
     pasos=ADC_lectura(0,0,0,0);
     return (((float)pasos*RESOL+0.0001)*5);
    }

float lectura_temperatura(void){
      unsigned int pasos=0;
      float resistencia=0;
      float temp=0.0;
     pasos=ADC_lectura(0,0,1,0);
     resistencia=(RESISTOR_SERIE*((1023.0/pasos)-1));
     temp=log(resistencia);
     temp= 1/ (0.001129148 + (0.000234125 * temp) + (0.0000000876741 * temp * temp * temp));
     return (temp-KELVIN);
   }

void interrupcion() iv 0x0004 ics ICS_AUTO {

    if(TMR0IF_bit){
          if(offset_mas==0){
             while(offset_mas==0);
             offset+=1;
          }
            if(offset_menos==0){
             while(offset_menos==0);
             offset-=1;
          }
      TMR0=156;
      TMR0IF_bit=0;
      }


    }


void configuracion(void){

/*CONFIGURACION DE LOS PUERTOS GPIO*/
       ANSELH=0X00;
     PORTB=0X00;
      TRISB=0XFF;//
     PORTD=0X00;
      TRISD=0x00;//
     PORTE=0x00;
      TRISE=0x03;//
       ANSEL=0x63;
     PORTA=0x00;
      TRISA=0x0F;//
     PORTC=0x00;
      TRISC=0xFF;//

   /*CONFIGURACION DEL ADC*/
     ADCON0=0x81;     //10 0000 01
     ADCON1=0x80;    // 10000000
     ADRESH=0x00;
     ADRESL=0x00;
  //INTERRUPCION POR TIMER 0
     INTCON=0xE0;  //111 00000
     OPTION_REG=0x06; //10000110  //maximo tiempo 16.387 ms
     TMR0=156;
 //PULL-UP internas del microcontrolador activas

}