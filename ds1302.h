
void write_ds1302_byte(char cmd) {
    
    RTC_IOTRIS = 0; //IO pini çýkýþ olarak ayarlanýyor
    char i;
    for(i=0;i<=7;i++) {
        RTC_IO = cmd & 0x01; //Bit 1 ise yazdýrýlýyor
        RTC_SCLK = 1;
        RTC_SCLK = 0;
        cmd = cmd>>1; //Byte bir saða kaydýrýlýyor
   }
}

void write_ds1302(char cmd, char data) {

    RTC_RST = 1;
    write_ds1302_byte(cmd); //Komut gönderiliyor
    write_ds1302_byte(data); //Data gönderiliyor
    RTC_RST = 0;
}

char read_ds1302(char cmd) {
   char i,data;

   RTC_RST = 1;
   write_ds1302_byte(cmd); //Okuma için komut gönderiliyor
   
   RTC_IOTRIS = 1; //IO pini giriþ olarak ayarlanýyor

   for(i=0;i<=7;++i) {
        data = data>>1; //Byte bir bit saða kaydýrýlýyor
        if (RTC_IO) {
            data = data | 0x80; //Okunan bit 1 ise yazýlýyor
        }
        RTC_SCLK = 1;
        __delay_us(2);
        RTC_SCLK = 0;
        __delay_us(2);
   }
   RTC_RST = 0;

   return(data);
}

void rtc_init() {
    
    char x;
    RTC_SCLKTRIS = 0;
    RTC_RSTTRIS = 0;
    RTC_IOTRIS = 0;
            
    RTC_RST = 0;
    __delay_us(2);
    RTC_SCLK = 0;
    
    write_ds1302(0x8e,0); //WP biti sýfýrlanýyor
    write_ds1302(0x90,0xa4); //Þarj bitleri ayarlanýyor
    x=read_ds1302(0x81); //CH biti kontrol edililiyor
    if((x & 0x80)!=0)
        write_ds1302(0x80,0); //Eðer CH biti 0 deðilse 0'lanýyor
}

char get_bcd(char data)
{
   int nibh;
   int nibl;

   nibh=data/10; //Yüksek 4 bit
   nibl=data-(nibh*10); //Alçal 4 bit

   return((nibh<<4)|nibl);
}

char rm_bcd(char data)
{
   int i;

   i=data;
   data=(i>>4)*10; //10 ile çarpýlan yüksek 4 bit okunuyor
   data=data+(i & 0x0F); //Alçak 4 bit ile toplanýyor

   return data;
}

void rtc_set_date(char day, char month, char year, char dow) {

    write_ds1302(0x86,get_bcd(day));
    write_ds1302(0x88,get_bcd(month));
    write_ds1302(0x8c,get_bcd(year));
    write_ds1302(0x8a,get_bcd(dow));
}

void rtc_set_clock(char hour, char min, char sec) {

    write_ds1302(0x84,get_bcd(hour));
    write_ds1302(0x82,get_bcd(min));
    write_ds1302(0x80,get_bcd(sec));
}

char rtc_get_day() {
    
    char day;
    day = rm_bcd(read_ds1302(0x87));
    
    return day;    
}

char rtc_get_month() {
    
    char month;
    month = rm_bcd(read_ds1302(0x89));
    
    return month;    
}

char rtc_get_year() {
    
    char year;
    year = rm_bcd(read_ds1302(0x8D));
    
    return year;    
}

char rtc_get_dow() {
    
    char dow;
    dow = rm_bcd(read_ds1302(0x8B));
    
    return dow;    
}

char rtc_get_hour() {
    
    char hr;
    hr = rm_bcd(read_ds1302(0x85));
    
    return hr;
}

char rtc_get_min() {
    
    char min;
    min = rm_bcd(read_ds1302(0x83));
    
    return min;
}

char rtc_get_sec() {
    
    char sec;
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