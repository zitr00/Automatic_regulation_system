#include <LiquidCrystal.h>

#define BUTTON_OK 13
#define PWM_PIN 10
#define SAMPLE_TIME 0.1
#define BUTTON_CANCEL 3
#define BUTTON_PREV 11
#define BUTTON_NEXT 12

void adc_init()
{
//set division factor between system clock frequency and the input clock to the ADC‐ 128
ADCSRA |= ((1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0));//set division factor between system clock
ADMUX |= (1<<REFS0); //AVcc with external capacitor at Aref pin
ADCSRA |= (1<<ADEN);//enable ADC
ADCSRA |= (1<<ADSC);//pornire conversie ADC
}
uint16_t read_adc(uint8_t channel)
{
ADMUX &= 0xF0; // selectare input A0->A5
ADMUX |= channel; // selectare canal
ADCSRA |= (1<<ADSC); //pornire conversie
while(ADCSRA & (1<<ADSC));//asteptam pana conversiile nu sunt actualizate
return ADC; //returneaza tensiunea
}

enum Buttons {
  EV_OK,
  EV_CANCEL,
  EV_NEXT,
  EV_PREV,
  EV_NONE,
  EV_MAX_NUM
};

enum Menus {
  MENU_MAIN = 0,
  MENU_RESET,
  MENU_TEMP,
  MENU_TEMP_CRESTERE,
  MENU_TEMP_MEN,
  MENU_TEMP_RACIRE,
  MENU_KP,
  MENU_KD,
  MENU_KI,
  MENU_START,
  MENU_MAX_NUM
};

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
float temp = 39,tempactual=0;
int temp_cre=50;//secunde
int temp_men=50;
int temp_rac=200;
float setpoint;
float ut;
float temp_init;
char counter=0;
int numarator_stare;
float kp =125, ki=0.5, kd=0.25;; 
float eroare = 0, sum_erori = 0, eroare_ant = 0; 
double deriv = 0; 
boolean startok=false;
double output;
int n;
float v,t;
Menus scroll_menu = MENU_MAIN;
Menus current_menu =  MENU_MAIN;

void state_machine(enum Menus menu, enum Buttons button);
Buttons GetButtons(void);
void print_menu(enum Menus menu);

typedef void (state_machine_handler_t)(void);

void print_menu(enum Menus menu)
{
  lcd.clear();
  switch(menu)
  {
    case MENU_KP:
    	lcd.print("KP = ");
    	lcd.print(kp+counter);
    	break;
    case MENU_RESET:
      lcd.print("Resetati?");
      break;
    case MENU_KD:
    	lcd.print("KD = ");
    	lcd.print(kd+counter);
    	break;
    case MENU_KI:
    	lcd.print("KI = ");
    	lcd.print(ki+counter);
    	break;
    case MENU_TEMP:
    	lcd.print("TEMP = ");
    	lcd.print(temp+counter);
    	break;
    case MENU_TEMP_CRESTERE:
    	lcd.print("TEMP CRESTERE= ");
    	lcd.print(temp_cre+counter);
    	break;
    case MENU_TEMP_MEN:
    	lcd.print("TEMP MENTINE= ");
    	lcd.print(temp_men+counter);
    	break;
    case MENU_TEMP_RACIRE:
    	lcd.print("TEMP RACIRE = ");
    	lcd.print(temp_rac+counter);
    	break;
    case MENU_START:
      lcd.print("START PROGRAM");
      break;
    default:
    	lcd.print("PS 2022");
   		break;
  }
  if(current_menu != MENU_MAIN && current_menu != MENU_START && current_menu != MENU_RESET)
  {
  	lcd.setCursor(0,1);
  	lcd.print("modifica: ");
  }
  else if(current_menu != MENU_MAIN)
  {
    lcd.setCursor(0,1);
  	lcd.print("Confirmare...");
  }
}

void enter_menu(void)
{
  current_menu = scroll_menu;
}

void go_home(void)
{
  scroll_menu = MENU_MAIN;
  current_menu = scroll_menu;
  counter=0;
}

void go_next(void)
{
  scroll_menu = (Menus) ((int)scroll_menu + 1);
  scroll_menu = (Menus) ((int)scroll_menu % MENU_MAX_NUM);
}

void go_prev(void)
{
  scroll_menu = (Menus) ((int)scroll_menu - 1);
  scroll_menu = (Menus) ((int)scroll_menu % MENU_MAX_NUM);
}

void save_kp(void)
{
  kp=kp+counter;
  go_home();
}

void inc_kp(void)
{
  counter++;
}

void dec_kp(void)
{
  counter--;
}

void inc_kd(void)
{
  counter++;
}

void dec_kd(void)
{
  counter--;
}

void save_kd(void)
{
  kd=kd+counter;
  go_home();
}

void inc_ki(void)
{
  counter++;
}

void dec_ki(void)
{
  counter--;
}

void save_ki(void)
{
  ki=ki+counter;
  go_home();
}

void inc_temp(void)
{
  counter++;
}

void dec_temp(void)
{
  counter--;
}

void save_temp(void)
{
  temp=temp+counter;
  go_home();
}

void inc_temp_cre(void)
{
  counter++;
}

void dec_temp_cre(void)
{
  counter--;
}

void save_temp_cre(void)
{
  temp_cre=temp_cre+counter;
  go_home();
}

void inc_temp_men(void)
{
  counter++;
}

void dec_temp_men(void)
{
  counter--;
}

void save_temp_men(void)
{
  temp_men=temp_men+counter;
  go_home();
}

void inc_temp_rac(void)
{
  counter++;
}

void dec_temp_rac(void)
{
  counter--;
}

void save_temp_rac(void)
{
  temp_rac=temp_rac+counter;
  go_home();
}

void start(void)//start program
{
  lcd.clear();
  startok=true;
  //scroll_menu = MENU_MAIN;
  //current_menu = scroll_menu;
}

void resetare(void)
{
   temp = 30.0;
   temp_cre=5;
   temp_men=5;
   temp_rac=5;
   counter=0;
   kp = 125; 
   kd = 0.25;   
   ki = 0.5; 
   lcd.clear(); lcd.print("Resetare..."); delay(2000);
   scroll_menu = MENU_MAIN;
   current_menu = scroll_menu;

}

state_machine_handler_t* sm[MENU_MAX_NUM][EV_MAX_NUM] = 
{ //events: OK , CANCEL , NEXT, PREV
  {enter_menu, go_home, go_next, go_prev},  // MENU_MAIN
  {resetare, go_home, go_next, go_prev},  // MENU_RESET
  {save_temp, go_home, inc_temp, dec_temp},   // MENU_TEMP
  {save_temp_cre, go_home, inc_temp_cre, dec_temp_cre},   // MENU_TEMP_CRESTERE
  {save_temp_men, go_home, inc_temp_men, dec_temp_men},   // MENU_TEMP_MEN
  {save_temp_rac, go_home, inc_temp_rac, dec_temp_rac},   // MENU_TEMP_RACIRE
  {save_kp, go_home, inc_kp, dec_kp},       // MENU_KP
  {save_kd, go_home, inc_kd, dec_kd},       // MENU_KD
  {save_ki, go_home, inc_ki, dec_ki},       // MENU_KI
  {start, go_home, go_next, go_prev},       // MENU_START
};

void state_machine(enum Menus menu, enum Buttons button)
{
  sm[menu][button]();
}

Buttons GetButtons(void)
{
  enum Buttons ret_val = EV_NONE;
  if (digitalRead(BUTTON_OK))
  {
    ret_val = EV_OK;
  }
  else if (digitalRead(BUTTON_CANCEL))
  {
    ret_val = EV_CANCEL;
  }
  else if (digitalRead(BUTTON_NEXT))
  {
    ret_val = EV_NEXT;
  }
  else if (digitalRead(BUTTON_PREV))
  {
    ret_val = EV_PREV;
  }

  return ret_val;
}

void setup()
{
  T1();
  adc_init();
  n=read_adc(0); 
  v = (n * 5) / 1023.0;
  temp_init = v / 0.01;
setpoint=temp_init;
 pinMode(PWM_PIN, OUTPUT);
  Serial.begin(9600);
  lcd.begin(16,2);
  pinMode(13, INPUT);
  digitalWrite(13, LOW); // pull-down
    pinMode(3, INPUT);
  digitalWrite(3, LOW); // pull-down
    pinMode(11, INPUT);
  digitalWrite(11, LOW); // pull-down
    pinMode(12, INPUT);
  digitalWrite(12, LOW); // pull-down
}

void T1()
{
 cli(); 
 TCCR1A=0;//setarea valorilor registrului pe 0
 TCCR1B=0;//setarea valorilor registrului pe 0
 TCNT1=0;//initializam valoarea counter-ului pe 0
 OCR1A=15624;//1 secunda
 TCCR1B|=(1<<WGM12);//CTC
 TCCR1B|=(1<<CS12)|(1<<CS10);//prescaler 1024
 TIMSK1|=(1<<OCIE1A);//enable timer compare interrupt
 sei();
} 


void loop()
{
  if(startok)//in isr merge pe crestere si mentinere 
  {
  calcPID();
  
  }
  
}

void calcTimpi()
{
  numarator_stare++;

  if(numarator_stare <= temp_cre) {  
      setpoint = temp_init + (((double)temp - temp_init)/(double)temp_cre) * (double)numarator_stare;    
    }

    else if(numarator_stare <= temp_cre + temp_men) {
      setpoint = (double)temp; 
     
      
      
    }

    else if(numarator_stare<= temp_cre + temp_men + temp_rac) {
      setpoint = temp - ((double)temp - temp_init)/((double)temp_rac) * ((double)(numarator_stare - temp_cre - temp_men));
      
      
    }

   else {
      setpoint = temp_init; 
      startok=false;
      analogWrite(PWM_PIN, 0);
    }
    Serial.println(tempactual);
}

void calcPID()
{


n=read_adc(0);


v=(n*5)/1023.0;//calcul tensiune

  tempactual=v/0.01;//calcul temperatura


  eroare = setpoint  - tempactual;

  sum_erori += (eroare * SAMPLE_TIME);

  deriv = (eroare - eroare_ant) / SAMPLE_TIME;

  ut = (kp * eroare) + (ki * sum_erori) + (kd * deriv);

   eroare_ant = eroare;

   if(ut < 0)
    output = 0;
   else if(ut >= 0 && ut <= 255)
      output = ut;
   else if(ut > 255)
      output = 255;
      
   analogWrite(PWM_PIN, (int)output);


}

ISR(TIMER1_COMPA_vect)
{
    if(startok)
    {
   calcTimpi();
    } 
    else
    {
  volatile Buttons event = GetButtons();
  if (event != EV_NONE)
  {
    state_machine(current_menu, event);
  }
    print_menu(scroll_menu);
    }
  
}