#define RTC_RST PORTB_PB7
#define RTC_RSTTRIS DDRB_DDRB7
#define RTC_IO PORTB_PB6
#define RTC_IOTRIS DDRB_DDRB6
#define RTC_SCLK PORTB_PB5
#define RTC_SCLKTRIS DDRB_DDRB5



void __delay_us(unsigned int n)
{	
    unsigned int m;
    m=n*10;
	  if (m == 0) 
	  {
		    return ;
		}
	  while (--m);
} 

void write_ds1302_byte(unsigned char cmd) {
    unsigned char i;
    //RTC_IOTRIS = 0; //IO pini ?ykyt olarak ayarlanyyor
    RTC_IOTRIS = 1;
    //EnterCritical();
    for(i=0;i<=7;i++) {
        RTC_IO = cmd & 0x01; //Bit 1 ise yazdyrylyyor
        RTC_SCLK = 1;
        RTC_SCLK = 0;
        cmd = cmd>>1; //Byte bir saea kaydyrylyyor
   }
   //ExitCritical();
}

void write_ds1302(unsigned char cmd, unsigned char data) {
    EnterCritical();
    RTC_RST = 1;
    write_ds1302_byte(cmd); //Komut g?nderiliyor
    write_ds1302_byte(data); //Data g?nderiliyor
    RTC_RST = 0;
    ExitCritical();
}

unsigned char read_ds1302(unsigned char cmd) {
   unsigned char i,data;
   EnterCritical();
   //RTC_IOTRIS = 1;
   RTC_RST = 1;
   write_ds1302_byte(cmd); //Okuma i?in komut g?nderiliyor
   
   //RTC_IOTRIS = 1; //IO pini girit olarak ayarlanyyor
   RTC_IOTRIS = 0;
   for(i=0;i<=7;++i) {
        data = data>>1; //Byte bir bit saea kaydyrylyyor
        if (RTC_IO) {
            data = data | 0x80; //Okunan bit 1 ise yazylyyor
        }
        RTC_SCLK = 1;
        __delay_us(2);
        RTC_SCLK = 0;
        __delay_us(2);
   }
   RTC_RST = 0;
   ExitCritical();
   return(data);
}

void rtc_init() {
    
    unsigned char x;
    //RTC_SCLKTRIS = 0;
    //RTC_RSTTRIS = 0;
    //RTC_IOTRIS = 0;
    RTC_SCLKTRIS = 1;
    RTC_RSTTRIS = 1;
    RTC_IOTRIS =1;
            
    //RTC_RST = 0;
    RTC_RST = 1;
    __delay_us(2);
    RTC_SCLK = 0;
    
    write_ds1302(0x8e,0); //WP biti syfyrlanyyor
    //write_ds1302(0x90,0xa4); //Tarj bitleri ayarlanyyor
    //write_ds1302(0x90,0x09);
    write_ds1302(0x90,0x09);
    x=read_ds1302(0x81); //CH biti kontrol edililiyor
    //hk1=x;
    if((x & 0x80)!=0){
      
        write_ds1302(0x80,0); //Eeer CH biti 0 deeilse 0'lanyyor
    }
}

unsigned char get_bcd(unsigned char data)
{
   int nibh;
   int nibl;

   nibh=data/10; //Yüksek 4 bit
   nibl=data-(nibh*10); //Al?al 4 bit

   return((nibh<<4)|nibl);
}

unsigned char rm_bcd(unsigned char data)
{
   int i;

   i=data;
   data=(i>>4)*10; //10 ile ?arpylan yüksek 4 bit okunuyor
   data=data+(i & 0x0F); //Al?ak 4 bit ile toplanyyor

   return data;
}

void rtc_set_date(unsigned char day, unsigned char month, unsigned char year, unsigned char dow) {

    write_ds1302(0x86,get_bcd(day));
    write_ds1302(0x88,get_bcd(month));
    write_ds1302(0x8c,get_bcd(year));
    write_ds1302(0x8a,get_bcd(dow));
}

void rtc_set_clock(unsigned char hour, unsigned char min, unsigned char sec) {

    write_ds1302(0x84,get_bcd(hour));
    write_ds1302(0x82,get_bcd(min));
    write_ds1302(0x80,get_bcd(sec));
}

unsigned char rtc_get_day() {
    
    unsigned char day;
    day = rm_bcd(read_ds1302(0x87));
    
    return day;    
}

unsigned char rtc_get_month() {
    
    unsigned char month;
    month = rm_bcd(read_ds1302(0x89));
    
    return month;    
}

unsigned char rtc_get_year() {
    
    unsigned char year;
    year = rm_bcd(read_ds1302(0x8D));
    
    return year;    
}

unsigned char rtc_get_dow() {
    
    unsigned char dow;
    dow = rm_bcd(read_ds1302(0x8B));
    
    return dow;    
}

unsigned char rtc_get_hour() {
    
    unsigned char hr;
    hr = rm_bcd(read_ds1302(0x85));
    
    return hr;
}

unsigned char rtc_get_min() {
    
    unsigned char min;
    min = rm_bcd(read_ds1302(0x83));
    
    return min;
}

unsigned char rtc_get_sec() {
    
    unsigned char sec;
    sec = rm_bcd(read_ds1302(0x81));
    
    return sec;
}

/*
void rtc_write_nvr(BYTE address, BYTE data) {
   write_ds1302(address|0xc0,data);
}
BYTE rtc_read_nvr(BYTE address) {
    return(read_ds1302(address|0xc1));
}
*/
