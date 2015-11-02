/*#######################################################################################
无刷电机控制系统
#######################################################################################*/

#include "main.h"

unsigned int  PWM = 0;
unsigned int  Strom = 0,RuheStrom; //0.1A
unsigned char Strom_max = 0;
unsigned char Mittelstrom = 0; 
unsigned int  Drehzahl = 0;  //  100Hz  60 = 6000
unsigned int  KommutierDelay = 10;
unsigned int  I2C_Timeout = 0;
unsigned int SIO_Timeout = 0;
unsigned int  SollDrehzahl = 0;
unsigned int  IstDrehzahl = 0;
unsigned int  DrehZahlTabelle[256];//转动值
unsigned char ZeitFuerBerechnungen = 1;
unsigned char MotorAnwerfen = 0;
unsigned char MotorGestoppt = 1;
unsigned char MaxPWM = MAX_PWM;
unsigned int  CntKommutierungen = 0;
unsigned int  SIO_Drehzahl = 0;
unsigned char ZeitZumAdWandeln = 1;
unsigned char MotorAdresse = 1;
unsigned char PPM_Betrieb = 1;

//############################################################################
void SetPWM(void)
//############################################################################
{
    unsigned char tmp_pwm; 
    tmp_pwm = PWM;
    if(tmp_pwm > MaxPWM)    // 电流限制
        {
        tmp_pwm = MaxPWM;
        PORTC |= ROT; 
        } 
    if(Strom > MAX_STROM)   // Strombegrenzung
        {
        OCR1A = 0; OCR1B = 0; OCR2  = 0;
        PORTC |= ROT; 
        Strom--;
        }
    else 
        {
        #ifdef  _32KHZ 
        OCR1A =  tmp_pwm; OCR1B =  tmp_pwm; OCR2  = tmp_pwm;
        #endif 

        #ifdef  _16KHZ 
        //OCR1A = 2 * (int)tmp_pwm; OCR1B = 2 * (int)tmp_pwm; OCR2  = tmp_pwm;
        OCR1A =  tmp_pwm; OCR1B =  tmp_pwm; OCR2  = tmp_pwm;
        #endif 
        }
}

//############################################################################
//
void PWM_Init(void)
//############################################################################
{
    PWM_OFF;
    TCCR1B = (1 << CS10) | (0 << CS11) | (0 << CS12) | (0 << WGM12) | 
             (0 << WGM13) | (0<< ICES1) | (0 << ICNC1);
/*    TCCR1B = (1 << CS10) | (0 << CS11) | (0 << CS12) | (1 << WGM12) | 
             (0 << WGM13) | (0<< ICES1) | (0 << ICNC1);
*/
}

//############################################################################
//
void Wait(unsigned char dauer)
//############################################################################
{
    dauer = (unsigned char)TCNT0 + dauer;
    while((TCNT0 - dauer) & 0x80);
}

//############################################################################
//
void Anwerfen(unsigned char pwm)
//############################################################################
{
    unsigned long timer = 300,i;
    DISABLE_SENSE_INT;
    PWM = 5;
    SetPWM();
    Manuell();
    Delay_ms(200);
    PWM = pwm;
    while(1)
        {
        for(i=0;i<timer; i++) 
            {
            if(!UebertragungAbgeschlossen)  SendUart();
            else DatenUebertragung();
            Wait(100);  // wait
            } 
        timer-= timer/15+1;
        if(timer < 25) { if(TEST_MANUELL) timer = 25; else return; }

        Manuell();
        Phase++;
        Phase %= 6;
        AdConvert();
        PWM = pwm;
        SetPWM();
        if(SENSE) 
            {
            PORTD ^= GRUEN;
            } 
        }
}

//**********************故障灯*************************//

void RotBlink(unsigned char anz)
{
sei(); // Interrupts ein
 while(anz--)
  {
   PORTC |= ROT;
   Delay_ms(300);    
   PORTC &= ~ROT;
   Delay_ms(300);    
  }
   Delay_ms(1000);    
}

#define TEST_STROMGRENZE 120
unsigned char DelayM(unsigned int timer)
{
 while(timer--)
  {
   FastADConvert();
   if(Strom > (TEST_STROMGRENZE + RuheStrom))
       {
        FETS_OFF;
        return(1);
       } 
  }
 return(0);  
}

unsigned char Delay(unsigned int timer)
{
 while(timer--)
  {
//   if(SENSE_H) { PORTC |= ROT; } else { PORTC &= ~ROT;}
  }
 return(0);  
}

/*
void ShowSense(void)
{
 if(SENSE_H) { PORTC |= ROT; } else { PORTC &= ~ROT;}

}
*/
#define HIGH_A_EIN PORTB |= 0x08
#define HIGH_B_EIN PORTB |= 0x04
#define HIGH_C_EIN PORTB |= 0x02
#define LOW_A_EIN  PORTD |= 0x08
#define LOW_B_EIN  PORTD |= 0x10
#define LOW_C_EIN  PORTD |= 0x20

void MotorTon(void)
//############################################################################
{
    unsigned char ADR_TAB[5] = {0,0,2,1,3};
    unsigned int timer = 300,i;
    unsigned int t = 0;
    unsigned char anz = 0,MosfetOkay = 0, grenze = 50;

    PORTC &= ~ROT;
    Delay_ms(300 * ADR_TAB[MotorAdresse]);    
    DISABLE_SENSE_INT;
    cli();//Globale Interrupts Ausschalten
    uart_putchar('\n');
    STEUER_OFF;
    Strom_max = 0;
    DelayM(50);
    RuheStrom = Strom_max;

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+ 高端功率管短路测试                                    +//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    Strom = 0;
    LOW_B_EIN; 
    HIGH_A_EIN; 
    if(DelayM(3))
       {
        anz = 1;
        uart_putchar('1');
       }
    FETS_OFF;
    Delay(1000);
    Strom = 0;
    LOW_A_EIN;
    HIGH_B_EIN; 
    if(DelayM(3))
       {
        anz = 2;
        uart_putchar('2');
       }
    FETS_OFF;
    Delay(1000);
    Strom = 0;
    LOW_B_EIN; // Low C ein
    HIGH_C_EIN; // High B ein
    if(DelayM(3))
       {
        anz = 3;
        uart_putchar('3');
       }
    FETS_OFF;
    Delay(1000);
    LOW_A_EIN; // Low  A ein; und A gegen C
    HIGH_C_EIN; // High C ein
    if(DelayM(3))
       {
        anz = 3;
        uart_putchar('7');
       }
    FETS_OFF;
    DelayM(10000);
if(anz) while(1) RotBlink(anz);  

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+ 低端功率管短路测试
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 if(UDR == ' ') {t = 65535; grenze = 40; uart_putchar('_');} else t = 1000; 
 Strom = 0;
 for(i=0;i<t;i++)
 {
  LOW_A_EIN; 
  DelayM(1);
  FETS_OFF;
  Delay(5);
  HIGH_A_EIN;
  DelayM(1);
  FETS_OFF;
  if(Strom > grenze + RuheStrom) {anz = 4; uart_putchar('4'); break;}
  Delay(5);
 }
 Delay(10000);

 Strom = 0;
 for(i=0;i<t;i++)
 {
  LOW_B_EIN; 
  DelayM(1);
  FETS_OFF;
  Delay(5);
  HIGH_B_EIN;
  DelayM(1);
  FETS_OFF;
  if(Strom > grenze + RuheStrom) {anz = 5; uart_putchar('5'); break;}
  Delay(5);
 } 

 Strom = 0;
 Delay(10000);

 for(i=0;i<t;i++)
 {
  LOW_C_EIN; 
  DelayM(1);
  FETS_OFF;
  Delay(5);
  HIGH_C_EIN;
  DelayM(1);
  FETS_OFF;
  if(Strom > grenze + RuheStrom) {anz = 6; uart_putchar('6'); break;}
  Delay(5);
 } 

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+ 高端功率管开关测试
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    SENSE_A;
    FETS_OFF;
    LOW_B_EIN; // Low B ein
    LOW_C_EIN; // Low C ein
    Strom = 0;
#define TONDAUER  40000    
#define SOUND_E 2
#define SOUND1_A 300
#define SOUND2_A 330
#define SOUND3_A 360

    for(i=0; i< (TONDAUER / SOUND2_A) ; i++)
     {
      HIGH_A_EIN; // Test A
      Delay(SOUND_E);
      if(MessAD(0) > 50) { MosfetOkay |= 0x01; } else { MosfetOkay &= ~0x01;};
      PORTB = 0;
      Delay(SOUND1_A);
     }
    FETS_OFF;

    LOW_A_EIN; // Low A ein
    LOW_C_EIN; // Low C ein
    for(i=0; i<(TONDAUER / SOUND1_A); i++)
     {
      HIGH_B_EIN; // Test B
      Delay(SOUND_E);
      if(MessAD(1) > 50) { MosfetOkay |= 0x02; } else { MosfetOkay &= ~0x02;};
      PORTB = 0;
      Delay(SOUND1_A);
     }

    FETS_OFF;
    LOW_A_EIN; // Low A ein
    LOW_B_EIN; // Low B ein
    for(i=0; i<(TONDAUER / SOUND3_A); i++)
     {
      HIGH_C_EIN; // Test C
      Delay(SOUND_E);
      if(MessAD(2) > 50) { MosfetOkay |= 0x04; } else { MosfetOkay &= ~0x04;};
      PORTB = 0;
      Delay(SOUND2_A);
     }
    FETS_OFF;
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+ 低端功率管开关测试
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    SENSE_B;
    LOW_A_EIN; // Low A ein
    for(i=0; i< (TONDAUER / SOUND2_A) ; i++)
     {
      HIGH_B_EIN; // Test B
      Delay(SOUND_E);
      if(MessAD(0) > 50) { MosfetOkay &= ~0x08;} else { MosfetOkay |= 0x08;};
      PORTB = 0;
      Delay(SOUND2_A);
     }

//++++++++++++++++++++++++++++++++++++
    LOW_C_EIN; // Low C ein
    for(i=0; i<(TONDAUER / SOUND1_A); i++)
     {
      HIGH_B_EIN; // Test B
      Delay(SOUND_E);
      if(MessAD(2) > 50) { MosfetOkay &= ~0x20;} else { MosfetOkay |= 0x20;};
      PORTB = 0;
      Delay(SOUND3_A);
     }
    FETS_OFF;
//++++++++++++++++++++++++++++++++++++
    FETS_OFF;
    LOW_B_EIN; // Low B ein
    for(i=0; i<(TONDAUER / SOUND3_A); i++)
     {
      HIGH_C_EIN; // Test C
      Delay(SOUND_E);
      if(MessAD(1) > 50) { MosfetOkay &= ~0x10;} else { MosfetOkay |= 0x10;};
      PORTB = 0;
      Delay(SOUND3_A);
     }
    FETS_OFF;

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 sei();
 Delay_ms(300 * (4-ADR_TAB[MotorAdresse]));    
 if(!(MosfetOkay & 0x01))  { anz = 1; UDR='A'; } else
 if(!(MosfetOkay & 0x02))  { anz = 2; UDR='B'; } else
 if(!(MosfetOkay & 0x04))  { anz = 3; UDR='C'; } else
 if(!(MosfetOkay & 0x08))  { anz = 4; UDR='a'; } else
 if(!(MosfetOkay & 0x10))  { anz = 5; UDR='b'; } else
 if(!(MosfetOkay & 0x20))  { anz = 6; UDR='c'; }  

 if(anz) Delay_ms(1000); 

 RotBlink(anz);
 uart_putchar('.');
}

//############################################################################
//
unsigned char SollwertErmittlung(void)
//############################################################################
{
    static unsigned int sollwert = 0;
    unsigned int ppm;
    if(!I2C_Timeout)   // 等待过零点
        {
        if(SIO_Timeout)  
            {
             sollwert =  (MAX_PWM * (unsigned int) SIO_Sollwert) / 200;  // skalieren auf 0-200 = 0-255
             PPM_Betrieb = 0;
             ICP_INT_DISABLE;
             PORTC &= ~ROT;
            }
        else
            if(anz_ppm_werte > 20)  //PPM
                {
                PPM_Betrieb = 1;
                ppm = PPM_Signal;
                if(ppm > 300) ppm =   0;  
                if(ppm > 200) ppm = 200;
                if(ppm <= MIN_PPM) sollwert = 0;
                else 
                    {
                    sollwert = (int) MIN_PWM + ((MAX_PWM - MIN_PWM) * (ppm - MIN_PPM)) / (190 - MIN_PPM);
                    }
                PORTC &= ~ROT;
                }
            else   
                {
                 if(!TEST_SCHUB) { if(sollwert) sollwert--; }   
                 PORTC |= ROT; 
                }
        }
    else 
        {
        sollwert = I2C_RXBuffer; 
        PPM_Betrieb = 0;
        PORTC &= ~ROT;
        ICP_INT_DISABLE;
        }
    if(sollwert > MAX_PWM) sollwert = MAX_PWM;
    return(sollwert); 
}

void DebugAusgaben(void)
{
    DebugOut.Analog[0] = Strom;
    DebugOut.Analog[1] = Mittelstrom;
    DebugOut.Analog[2] = SIO_Drehzahl;
    DebugOut.Analog[3] = PPM_Signal;
}


//############################################################################
//Hauptprogramm
int main (void)
//############################################################################
{
    char altPhase = 0;
    int test = 0;
    unsigned int MinUpmPulse,Blink,TestschubTimer;
    unsigned int Blink2,MittelstromTimer,DrehzahlMessTimer,MotorGestopptTimer;

    DDRC  = 0x08;
    PORTC = 0x08;
    DDRD  = 0xBA;
    PORTD = 0x00;
    DDRB  = 0x0E;
    PORTB = 0x31;
	
#if (MOTORADRESSE == 0)
    PORTB |= (ADR1 + ADR2);   // 地址码
    for(test=0;test<500;test++);
    if (PINB & ADR1)
	 {
	   if (PINB & ADR2) MotorAdresse = 1;
	    else MotorAdresse = 2;
	 }
	 else
	 {
	   if (PINB & ADR2) MotorAdresse = 3;
	    else MotorAdresse = 4;
	 }
#else
    MotorAdresse  = MOTORADRESSE;
#endif
    
    UART_Init();
    Timer0_Init();
    sei();//全局中断打开
    
    
    PWM_Init();	

    InitIC2_Slave(0x50);			    
    InitPPM();

    Blink             = SetDelay(101);    
    Blink2            = SetDelay(102);
    MinUpmPulse       = SetDelay(103);
    MittelstromTimer  = SetDelay(254);
    DrehzahlMessTimer = SetDelay(1005);
    TestschubTimer    = SetDelay(1006);
    while(!CheckDelay(MinUpmPulse))
    {
     if(SollwertErmittlung()) break;
    }
    ;
    PORTD |= GRUEN;
    PWM = 0;

    SetPWM();

    SFIOR = 0x08;  // Analog Comperator ein
    ADMUX = 1; 

    MinUpmPulse = SetDelay(10);
    DebugOut.Analog[1] = 1;
    PPM_Signal = 0;

    if(!SollwertErmittlung()) MotorTon();
//MotorTon();    
    PORTB = 0x31; // Pullups wieder einschalten

    // zum Test der Hardware; Motor dreht mit konstanter Drehzahl ohne Regelung
    if(TEST_MANUELL)    Anwerfen(TEST_MANUELL);  // kommt von dort nicht wieder

    while (1)
        {
//ShowSense();

        if(!TEST_SCHUB)   PWM = SollwertErmittlung();
        //I2C_TXBuffer = PWM; // Antwort 黚er I2C-Bus
        if(MANUELL_PWM)   PWM = MANUELL_PWM;

        // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        if(Phase != altPhase)   
            {
            MotorGestoppt = 0;
            ZeitFuerBerechnungen = 0;     
            MinUpmPulse = SetDelay(250);  
            altPhase = Phase;
            }
        // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        if(!PWM)    // Sollwert == 0
            {
            MotorAnwerfen = 0;      // kein Startversuch
            ZeitFuerBerechnungen = 0;
            // nach 1,5 Sekunden den Motor als gestoppt betrachten 
            if(CheckDelay(MotorGestopptTimer)) 
                {
                DISABLE_SENSE_INT;
                MotorGestoppt = 1;  
                STEUER_OFF;
                } 
            }
        else 
            {
            if(MotorGestoppt) MotorAnwerfen = 1;	// Startversuch
            MotorGestopptTimer = SetDelay(1500);
            }

        if(MotorGestoppt && !TEST_SCHUB) PWM = 0;
        SetPWM();
        // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        if(!ZeitFuerBerechnungen++)
            {
            if(MotorGestoppt) PORTD |= GRUEN; 
            if(SIO_DEBUG)
                {
                DebugAusgaben();  
                if(!UebertragungAbgeschlossen)  SendUart(); 
                else DatenUebertragung();
                }
            
            if(CheckDelay(MittelstromTimer))   
                {
                MittelstromTimer = SetDelay(50); //  50ms
                if(Mittelstrom <  Strom) Mittelstrom++;// 设置平均电流值
                else if(Mittelstrom >  Strom) Mittelstrom--;
                if(Strom > MAX_STROM) MaxPWM -= MaxPWM / 32;               
                if((Mittelstrom > LIMIT_STROM))// 限流
                    {
                    if(MaxPWM) MaxPWM--;
                    PORTC |= ROT; 
                    }
                else 
                    {
                    if(MaxPWM < MAX_PWM) MaxPWM++;
                    }
                }

            if(CheckDelay(DrehzahlMessTimer))   // 转速
                {
                DrehzahlMessTimer = SetDelay(10);
                SIO_Drehzahl = CntKommutierungen;//(6 * CntKommutierungen) / (POLANZAHL / 2);
                CntKommutierungen = 0;
                if(PPM_Timeout == 0) // 无PPM信号
                ZeitZumAdWandeln = 1;
                }

          if(TEST_SCHUB)
           {
            if(CheckDelay(TestschubTimer))  
                {
                TestschubTimer = SetDelay(1500);
                    switch(test) 
                        {
                        case 0: PWM = 50; test++; break;
                        case 1: PWM = 130; test++; break;
                        case 2: PWM = 60;  test++; break;
                        case 3: PWM = 140; test++; break;
                        case 4: PWM = 150; test = 0; break;
                        default: test = 0;
                        } 
                }
            }  
          // Motor Stehen geblieben
            if((CheckDelay(MinUpmPulse) && SIO_Drehzahl == 0) || MotorAnwerfen) 
                {
                MotorGestoppt = 1;    
                DISABLE_SENSE_INT;
                MinUpmPulse = SetDelay(100);         
                if(MotorAnwerfen)
                    {
                    PORTC &= ~ROT;
                    MotorAnwerfen = 0;
                    Anwerfen(10);
                    PORTD |= GRUEN;
                    MotorGestoppt = 0;    
                    Phase--;
                    PWM = 1;
                    SetPWM();
                    SENSE_TOGGLE_INT;
                    ENABLE_SENSE_INT;
                    MinUpmPulse = SetDelay(20);
                    while(!CheckDelay(MinUpmPulse)); 
                    PWM = 15;
                    SetPWM();
                    MinUpmPulse = SetDelay(300);
                    while(!CheckDelay(MinUpmPulse)); 
                   
				    
                    DrehzahlMessTimer = SetDelay(50);
                    altPhase = 7;
                    }
                }
            } 
        } // while(1) 
}

