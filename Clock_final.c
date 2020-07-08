#include <avr/io.h>
#define F_CPU 16000000
#include <util/delay.h>
#include <stdlib.h>

//deklaracja portow wyswietlacza LCD
#define DDR_LCD  DDRD
#define PORT_LCD PORTD
#define RS_LCD   PD2
#define EN_LCD   PD3
#define DB4_LCD  PD4
#define DB5_LCD  PD5
#define DB6_LCD  PD6
#define DB7_LCD  PD7

//deklaracje funkcji zegara
void budzik(void);
void zegarek(int8_t,int8_t,int8_t,int8_t,int8_t);
void intro(void);
void mode_set(void);
void alarm_set_f(void);
void month_set(void);
void day_set(void);
void year_set(void);
void hour_set(void);
void minute_set(void);
void Ahour_set(void);
void Aminute_set(void);
void display1(void);
void display2(void);
void display3(void);

//funkcje biblioteki do LCD
void Wyslij_do_LCD(int8_t);
void WlaczLCD(void);
void CzyscLCD(void);
void WyswietlLCD(char*, int8_t);

//deklaracja zmiennych globalnych uzywanych w funkcjach
struct {
int8_t rok;
int8_t miesiac;
int8_t dzien;
int8_t godz;
int8_t min;
int8_t sec;
}global;

struct {
int8_t set_h;
int8_t set_m;
int8_t set_y_start;
int8_t set_mo_start;
int8_t set_d_start;
int8_t set_h_start;
int8_t set_m_start;
int8_t alarm_set;
int8_t mode;
}settings;

//funkcja glowna
int main(void)
{
	settings.set_y_start=20;
	settings.set_mo_start=1;
	settings.set_d_start=1;
	settings.set_h_start=0;
	settings.set_m_start=0;
	settings.mode = 1;
	DDRC |= 0b000000; // ustawianie portow jako we
	DDRB |= 0b010000; // ustawianie portow jako we/wy
	WlaczLCD();
	intro();
	while(1)
	{
		mode_set();
		alarm_set_f();		
			switch(settings.mode){
				case 1: //tryb ustawiania daty na zegarku
					day_set();
					month_set();
					year_set();
					display1();
					break;
				case 2: //tryb ustawiania godziny na zegarku
					hour_set();
					minute_set();
					display2();
					break;
				case 3: //tryb ustawiania alarmu
					Ahour_set();
					Aminute_set();
					display3();
					break;
				case 4://odpalenie naszego zegarka z ustalonymi wczesniej parametrami
					zegarek(settings.set_h_start,settings.set_m_start,settings.set_d_start,settings.set_mo_start,settings.set_y_start);
					break;
				default: settings.mode = 1; break;
			}
		
	}
	return 0;
}

////////////////////////////////////////////// FUNKCJE ZEGARA
void intro(void)//funkcja intra wyswietlanego podczas uruchomienia 
{
	for(int8_t i = 0;i<2;i++)Wyslij_do_LCD(0b00011000);
	WyswietlLCD("P",1);
	_delay_ms(250);
	WyswietlLCD("r",1);
	_delay_ms(250);
	WyswietlLCD("o",1);
	_delay_ms(250);
	WyswietlLCD("j",1);
	_delay_ms(250);
	WyswietlLCD("e",1);
	_delay_ms(250);
	WyswietlLCD("k",1);
	_delay_ms(250);
	WyswietlLCD("t",1);
	_delay_ms(250);
	Wyslij_do_LCD(0b00011000);
	_delay_ms(250);
	WyswietlLCD("T",1);
	_delay_ms(250);
	WyswietlLCD("M",1);
	_delay_ms(250);
	WyswietlLCD("P",1);
	_delay_ms(250);
	for(int8_t y = 0; y<8;y++)_delay_ms(200);
	CzyscLCD();
	for(int8_t i = 0;i<4;i++)Wyslij_do_LCD(0b00011000);
	WyswietlLCD("Kacper",6);
	for(int8_t i = 0;i<35;i++)Wyslij_do_LCD(0b00011000);
	WyswietlLCD("Klepacz",7);
	for(int8_t y = 0; y<8;y++)_delay_ms(200);
	CzyscLCD();
	for(int8_t i = 0;i<4;i++)Wyslij_do_LCD(0b00011000);
	WyswietlLCD("Bartosz",7);
	for(int8_t i = 0;i<34;i++)Wyslij_do_LCD(0b00011000);
	WyswietlLCD("Pietrzyk",8);
	for(int8_t y = 0; y<8;y++)_delay_ms(200);
	CzyscLCD();
	
}

void zegarek(int8_t hour_passed,int8_t min_passed, int8_t day_passed, int8_t month_passed, int8_t year_passed) //definicja funkcji zegarka
{
	int8_t first_run1 = 1;//zmienna first run uzyta do zaimplementowania innych parametrow podczas iterowania
	int8_t first_run2 = 1;
	int8_t first_run3 = 1;
	int8_t first_run4 = 1;
	int8_t minuty_start; //ponizej parametry startowe funkcji
	int8_t day_start;
	int8_t month_start;
	int8_t hour_start;
	int8_t year_start = year_passed;
	int8_t special; //zmienna decydujaca o tym do jakiej wartosci wykonywac sie bedzie petla dni z wiadomych kalendarzowych przyczyn ;)
	int8_t hsize;
	int8_t msize;
	int8_t mosize;
	int8_t dsize;
	start1:
	for(int8_t year = year_start;year < 41;year++){
		global.rok = year;
		char yearc[3];
		itoa(global.rok,yearc,10);
		if(first_run1 == 1){month_start = month_passed;first_run1=0;}else{month_start=1;}
		for(int8_t month = month_start;month<13;month++){
			global.miesiac = month;
			if(global.miesiac>9){mosize=2;}else{mosize=1;}
			char monthc[3];
			itoa(global.miesiac,monthc,10);
			if(global.miesiac == 1 ||global.miesiac == 3 ||global.miesiac == 5 ||global.miesiac == 7 ||global.miesiac == 8 ||global.miesiac == 10 ||global.miesiac == 12) special = 32;
			if(global.miesiac == 4 ||global.miesiac == 6 ||global.miesiac == 9 ||global.miesiac == 11) special = 31;
			if(global.miesiac == 2) special = 29;
			if(first_run2 == 1){day_start = day_passed;first_run2=0;}else{day_start=1;}
			for(int8_t day = day_start;day<special;day++){
				global.dzien = day;
				if(global.dzien>9){dsize=2;}else{dsize=1;}
				char dayc[3];
				itoa(global.dzien,dayc,10);
				if(first_run3 == 1){hour_start = hour_passed;first_run3=0;}else{hour_start=0;}
				for(int8_t hour = hour_start;hour<24;hour++){
					global.godz = hour;
					if(global.godz>9){hsize=2;}else{hsize=1;}
					char godziny[3];
					itoa(global.godz,godziny,10);
					if(first_run4 == 1){minuty_start = min_passed;first_run4=0;}else{minuty_start=0;}
					for(int8_t mint = minuty_start;mint<60;mint++){
						global.min = mint;
						if(global.min>9){msize=2;}else{msize=1;}
						char minuty[3];
						itoa(global.min,minuty,10);
						CzyscLCD();//poczatek wyswietlania tego co zobaczymy na wyswietlaczu
						for(int8_t i = 0;i<5;i++)Wyslij_do_LCD(0b00011000);
						if (global.godz<10){;WyswietlLCD("0", 1);}
						WyswietlLCD(godziny, hsize);
						WyswietlLCD(":", 1);
						if (global.min<10){WyswietlLCD("0", 1);}
						WyswietlLCD(minuty, msize);
						for(int8_t i = 0;i<5;i++)Wyslij_do_LCD(0b00011000);
						alarm_set_f();
						if(settings.alarm_set == 1){WyswietlLCD("A",1);}else{WyswietlLCD("-",1);}
						for(int8_t i = 0;i<27;i++)Wyslij_do_LCD(0b00011000);
						if (day<10){;WyswietlLCD("0", 1);}
						WyswietlLCD(dayc, dsize);
						WyswietlLCD("/", 1);
						if (month<10){WyswietlLCD("0", 1);}
						WyswietlLCD(monthc, mosize);
						WyswietlLCD("/", 1);
						WyswietlLCD("20", 2);
						WyswietlLCD(yearc, 2);
						if(global.min == settings.set_m && global.godz == settings.set_h && settings.alarm_set == 1){
							budzik();}
						for(int8_t secs=0;secs<60;secs++){global.sec = secs;for(int8_t y = 0; y<4;y++)_delay_ms(250);}//petla realnych sekund
					}
				}
			}
		}
	}year_start = 20;goto start1;
}
void budzik(void){ //budzik dzialajacy na zasadzie wysylania stanu wysokiego lub niskiego na buzzer sterujac czy ma wydawac dzwiek czy nie
	while(!(bit_is_set(PINB,5))){//petla while dzialajaca do momentu az nie wylaczymy dzwonienia budzika przyciskiem
		PORTB |= 0b010000;//stan wysoki na porcie podpietym do buzzera
		_delay_ms(250);
		PORTB &= 0b000000;//stan niski na porcie podpietym do buzzera
		_delay_ms(125);
		PORTB |= 0b010000;
		_delay_ms(250);
		_delay_ms(250);
		PORTB &= 0b000000;
		_delay_ms(125);
		if(bit_is_set(PINB,5))break;
	}
}
void mode_set(void){
	if(bit_is_set(PINB,3) && settings.mode < 6 && settings.mode >= 0){ //warunek inkrementacji zmiennej obslugujacej switch'a
		settings.mode++;
		if(settings.mode == 5)settings.mode = 1;
	_delay_ms(100);}
}
void alarm_set_f(void){
	if(bit_is_set(PINB,2) && settings.alarm_set < 3 && settings.alarm_set >= 0){ //zmiana trybu alarmu: dziala lub nie
		settings.alarm_set++;
		_delay_ms(10);
	if(settings.alarm_set == 2)settings.alarm_set = 0;}
}
void month_set(void){
	if(bit_is_set(PINB,0) && settings.set_mo_start < 61 && settings.set_mo_start >= 0){
		settings.set_mo_start++;
		_delay_ms(10);
		if(settings.set_mo_start == 13)settings.set_mo_start = 1;
	}
}
void day_set(void){
	if(bit_is_set(PINB,1)){
		settings.set_d_start++;
		_delay_ms(10);
		if(settings.set_d_start == 32)settings.set_d_start = 1;
	}
}
void year_set(void){
	if(bit_is_set(PINC,5)){
		settings.set_y_start++;
		_delay_ms(10);
		if(settings.set_y_start == 41)settings.set_y_start = 20;
	}
}
void hour_set(void){
	if(bit_is_set(PINB,0) && settings.set_h_start < 25 && settings.set_h_start >= 0){
		settings.set_h_start++;
		_delay_ms(10);
		if(settings.set_h_start == 24)settings.set_h_start = 0;
	}
}
void Ahour_set(void){
	if(bit_is_set(PINB,0) && settings.set_h < 25 && settings.set_h >= 0){
		settings.set_h++;
		_delay_ms(10);
		if(settings.set_h == 24)settings.set_h = 0;
	}
}
void minute_set(void){
	if(bit_is_set(PINB,1) && settings.set_m_start < 61 && settings.set_m_start >= 0){
		settings.set_m_start++;
		_delay_ms(10);
		if(settings.set_m_start == 60)settings.set_m_start = 0;
	}
}
void Aminute_set(void){
	if(bit_is_set(PINB,1) && settings.set_m < 61 && settings.set_m >= 0){
		settings.set_m++;
		_delay_ms(10);
		if(settings.set_m == 60)settings.set_m = 0;
	}
}
void display1(void){
	int8_t mosize;
	int8_t dsize;
	char set_ystartc[3];
	itoa(settings.set_y_start,set_ystartc,10); //konwersja zmiennej typu int do tablicy char
	if(settings.set_mo_start>9){mosize=2;}else{mosize=1;}
	char set_mostartc[3];
	itoa(settings.set_mo_start,set_mostartc,10);
	if(settings.set_d_start>9){dsize=2;}else{dsize=1;}
	char set_dstartc[3];
	itoa(settings.set_d_start,set_dstartc,10);
	CzyscLCD();//czesc graficzna
	WyswietlLCD("SET:", 4);
	Wyslij_do_LCD(0b00011000); //Przesuniecie kursora w prawo o jedna pozycje
	WyswietlLCD("Year:",5);
	WyswietlLCD("20",2);
	WyswietlLCD(set_ystartc,2);
	for(int8_t i = 0;i<26;i++)Wyslij_do_LCD(0b00011000);
	WyswietlLCD("Month:",6);
	WyswietlLCD(set_mostartc,mosize);
	Wyslij_do_LCD(0b00011000);
	WyswietlLCD("Day:",4);
	WyswietlLCD(set_dstartc,dsize);
	_delay_ms(250);
}
void display2(void){
	int8_t hsize;
	int8_t msize;
	if(settings.set_h_start>9){hsize=2;}else{hsize=1;}
	char set_hstartc[3];
	itoa(settings.set_h_start,set_hstartc,10);
	if(settings.set_m_start>9){msize=2;}else{msize=1;}
	char set_mstartc[3];
	itoa(settings.set_m_start,set_mstartc,10);
	CzyscLCD();
	WyswietlLCD("SET:", 4);//czesc graficzna
	Wyslij_do_LCD(0b00011000); //Przesuniecie kursora w prawo o jedna pozycje
	WyswietlLCD("H:",2);
	WyswietlLCD(set_hstartc,hsize);
	Wyslij_do_LCD(0b00011000);
	WyswietlLCD("M:",2);
	WyswietlLCD(set_mstartc,msize);
	_delay_ms(250);
}
void display3(void){
	int8_t Ahsize;
	int8_t Amsize;
	if(settings.set_h>9){Ahsize=2;}else{Ahsize=1;}
	char set_hc[3];
	itoa(settings.set_h,set_hc,10);
	if(settings.set_m>9){Amsize=2;}else{Amsize=1;}
	char set_mc[3];
	itoa(settings.set_m,set_mc,10);
	char alarm_setc[2];
	itoa(settings.alarm_set,alarm_setc,10);
	CzyscLCD();//czesc graficzna
	WyswietlLCD("ALARM: ON/OFF:", 14);
	WyswietlLCD(alarm_setc, 1);
	for(int8_t i = 0;i<24;i++)Wyslij_do_LCD(0b00011000);
	Wyslij_do_LCD(0b00011000);
	WyswietlLCD("H:",2);
	WyswietlLCD(set_hc, Ahsize);
	Wyslij_do_LCD(0b00011000);
	WyswietlLCD("M:",2);
	WyswietlLCD(set_mc, Amsize);
	_delay_ms(250);
}
////////////////////////////////////////////// OBSŁUGA LCD
void Wyslij_do_LCD(int8_t bajt)
{
	//ustawienie EN
	PORT_LCD |= _BV(EN_LCD);

	//wyslanie 4 najstarszych bitow danych
	PORT_LCD = (bajt & 0xF0)|(PORT_LCD & 0x0F);

	//potwierdzenie wyslania danych (opadajacym zboczem EN)
	PORT_LCD &= ~(_BV(EN_LCD));

	//zaczekaj cykl
	asm volatile("nop");
	
	//ustawienie EN
	PORT_LCD |= _BV(EN_LCD);

	//wyslanie 4 najmlodszych bitow danych
	PORT_LCD = ((bajt & 0x0F)<<4)|(PORT_LCD & 0x0F);

	//potwierdzenie wyslania danych (opadającym zboczem EN)
	PORT_LCD &= ~(_BV(EN_LCD));

	//zaczekaj 40 us
	_delay_us(40);
}


void WlaczLCD()
{
	//ustawienie kierunku wyjsciowego linii podlaczonych do LCD
	DDR_LCD = (0xF0)|(1<<RS_LCD)|(1<<EN_LCD);
	//stan niski na liniach
	PORT_LCD = 0;

	//zaczekaj co najmniej 45 ms na ustabilizowanie sie napiecia
	_delay_ms(45);

	//1
	//ustaw linię EN
	PORT_LCD |= (1<<EN_LCD);
	//załaduj sekwencje 0011
	PORT_LCD |= (1<<DB4_LCD)|(1<<DB5_LCD);
	
	//potwierdź opadającym zboczem EN
	PORT_LCD &= ~(_BV(EN_LCD));

	//zaczekaj co najmniej 4.1 ms
	_delay_ms(4.1);

	//2
	//ustaw linię EN
	PORT_LCD |= (1<<EN_LCD);
	//potwierdz opadajacym zboczem EN sekwencje 0011
	PORT_LCD &= ~(_BV(EN_LCD));
	
	//zaczekaj co najmniej 100 us
	_delay_us(100);
	
	//3
	//ustaw linię EN
	PORT_LCD |= (1<<EN_LCD);
	//potwierdz opadajacym zboczem EN sekwencje 0011
	PORT_LCD &= ~(_BV(EN_LCD));
	
	//zaczekaj co najmniej 100 us
	_delay_us(100);
	
	//4
	//ustaw linie EN
	PORT_LCD |= (1<<EN_LCD);
	//ustawienie interfejsu 4-bitowego
	PORT_LCD &= ~(_BV(DB4_LCD));

	//potwierdz opadającym zboczem EN
	PORT_LCD &= ~(_BV(EN_LCD));

	//ustaw parametry wyswietlacza
	//bit 4 = 0 (slowo danych ma 4 bity)
	//bit 3 = 1 (2 wiersze znakow)
	//bit 2 = 0 (matryca 5x8 pikseli)
	PORT_LCD &= ~(_BV(RS_LCD));
	Wyslij_do_LCD(0b00101000);
	PORT_LCD |= _BV(RS_LCD);

	//ustaw tryb pracy wyświetlacza
	//bit 2 = 1 (inkrementowanie adresu zapisu danych)
	//bit 1 = 1 (wylaczenie przesuwania w prawo)
	PORT_LCD &= ~(_BV(RS_LCD));
	Wyslij_do_LCD(0b00000110);
	PORT_LCD |= _BV(RS_LCD);

	//wlacz wyswietlacz
	//bit 2 = 1 (wlaczenie wyswietlania)
	//bit 1 = 0 (kursor nieaktywny)
	//bit 0 = 0 (kursor nie miga)
	PORT_LCD &= ~(_BV(RS_LCD));
	Wyslij_do_LCD(0b00001100);
	PORT_LCD |= _BV(RS_LCD);
	CzyscLCD();
}

void CzyscLCD()
{
	PORT_LCD &= ~(_BV(RS_LCD));
	Wyslij_do_LCD(1);
	PORT_LCD |= _BV(RS_LCD);
	//czekaj 1.64 ms
	_delay_ms(1.64);
}

void WyswietlLCD(char *napis, int8_t ile)
{
	int8_t k = 0;
	while(k<ile)
	{
		Wyslij_do_LCD(napis[k]);
		k++;
	}
}
