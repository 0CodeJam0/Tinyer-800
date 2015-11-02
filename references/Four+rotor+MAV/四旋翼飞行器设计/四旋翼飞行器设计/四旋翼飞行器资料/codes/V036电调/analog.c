/*############################################################################

############################################################################*/

#include "main.h"

//############################################################################
//Init ADC
void ADC_Init(void)
//############################################################################
{
  ADCSRA = 0xA6;  // �������� & 1MHZ
  ADMUX = 7;      // ͨ��
  ADCSRA |= 0x40; // Start
}

//############################################################################
//�ɼ�����ֵ
void AdConvert(void)
//############################################################################
{
 unsigned int i=0;  
 unsigned char sense;
  sense = ADMUX;    
  ADMUX  =  0x06;  // ����ͨ��
  SFIOR  =  0x00;   
  ADCSRA =  0xD3;  
  ADCSRA |= 0x10;  // ת������
  ADMUX  =  0x06;  
  ADCSRA |= 0x40;  // ��ʼ
  while (((ADCSRA & 0x10) == 0));
  ADMUX = sense;   
  i = ADCW * 4;
  Strom = (i + Strom * 7) / 8; 
  if (Strom_max < Strom) Strom_max = Strom;
  ADCSRA = 0x00;  
  SFIOR = 0x08;  
}



//############################################################################
//�ɼ�ģ����
unsigned int MessAD(unsigned char channel)
//############################################################################
{
 unsigned int i=0;  
 unsigned char sense;
  sense = ADMUX;   
  ADMUX  =  channel;  
  SFIOR  =  0x00;  
  ADCSRA =  0xD3;  
  ADCSRA |= 0x10;  
  ADMUX  =  channel;  
  ADCSRA |= 0x40;  // Start
  while (((ADCSRA & 0x10) == 0));
  ADMUX = sense;   
  ADCSRA = 0x00;  
  SFIOR = 0x08;  
return(ADCW);
}

//############################################################################
void FastADConvert(void)
//############################################################################
{
 unsigned int i=0;  
 unsigned char sense;
  i = MessAD(6) * 4;
//  i = ADCW * 4;
  if(i > 200) i = 200;
  Strom = i;//(i + Strom * 1) / 2; 
  if (Strom_max < Strom) Strom_max = Strom;
  ADCSRA = 0x00;  
  SFIOR = 0x08;  
}
