/////////////////////////////////////////////////////////////////////////////
//
//     zzzzzzzzzz
//           zzz eeee  u  u gggg mmmmm  aaa
//         zzz   eeee  u  u g  g m m m a  a	  T�RK�E ��LET�M S�STEM�
//	     zzz	 e     u  u gggg m m m a  a	         ver 0.1
//     zzz  	 eeee  uuuu    g m m m  aaaa
//    zzzzzzzzzzzzzzzzzzzzzzzzzgzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz
//                             g
//                          gggg
//
//  Sistem genelinde kullan�lan �nemli tan�mlamalar� ve sabitleri i�erir.
/////////////////////////////////////////////////////////////////////////////
#ifndef PORTS
#define PORTS
//-------------------------------------------------------------------------------
//PIC programlanmas� i�in gerekli olan port adresleri
//Bu port numaralar� kullan�larak donan�m kesmeleri ta��nacakt�r. Ayr�ca sistemin kesme-
//lere tam olarak yan�t vermesi i�in, PIC programlamas�n�n yap�lmas� ve gereken paramet-
//relerin verilmesi �artt�r.BIOS bunu a��l��ta yapsa da , uyumluluk a��s�ndan tekrar ya-
//p�lmas� daha uygundur.
#define PIC_MASTER_PORT_0 0x20    //ICW1'in g�nderilece�i port numaras� (PIC1)
#define PIC_MASTER_PORT_1 0x21    //ICW2,ICW3 ve ICW4'�n g�nderilece�i port numaras� (PIC1)
#define PIC_SLAVE_PORT_0 0x0A0    //PIC2 i�in ilk port
#define PIC_SLAVE_PORT_1 0x0A1    //PIC2 i�in 2. port
#define EOI	0x20
//------------------------------------------------------------------------------
//PIT programlanmas� i�in gerekli portlar
#define PIT_1_COUNTER_0  0x40
#define PIT_1_COUNTER_1  0x41
#define PIT_1_COUNTER_2  0x42
#define PIT_1_CONTROL_REGISTER 0x43
#define PIT_2_COUNTER_0  0x48
#define PIT_2_COUNTER_1  0x49
#define PIT_2_COUNTER_2  0x4A
#define PIT_2_CONTROL_REGISTER 0x4B
//------------------------------------------------------------------------------
//8042 Klavye denetleyicisinin programlanabilmesi i�in gerekli olan portlar
//Bu port numaralar� A20 adres baca��n�n aktif hale getirilmesi i�in kullan�lacakt�r.
#define KEYBOARD_DATA_REGISTER  0x60
#define KEYBOARD_COMMAND_REGISTER  0x64  //komut yazma
#define KEYBOARD_STATUS_REGISTER  0x64   //durum bilgisi alma

#endif