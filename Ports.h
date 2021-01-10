/////////////////////////////////////////////////////////////////////////////
//
//     zzzzzzzzzz
//           zzz eeee  u  u gggg mmmmm  aaa
//         zzz   eeee  u  u g  g m m m a  a	  TÜRKÇE ÝÞLETÝM SÝSTEMÝ
//	     zzz	 e     u  u gggg m m m a  a	         ver 0.1
//     zzz  	 eeee  uuuu    g m m m  aaaa
//    zzzzzzzzzzzzzzzzzzzzzzzzzgzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz
//                             g
//                          gggg
//
//  Sistem genelinde kullanýlan önemli tanýmlamalarý ve sabitleri içerir.
/////////////////////////////////////////////////////////////////////////////
#ifndef PORTS
#define PORTS
//-------------------------------------------------------------------------------
//PIC programlanmasý için gerekli olan port adresleri
//Bu port numaralarý kullanýlarak donaným kesmeleri taþýnacaktýr. Ayrýca sistemin kesme-
//lere tam olarak yanýt vermesi için, PIC programlamasýnýn yapýlmasý ve gereken paramet-
//relerin verilmesi þarttýr.BIOS bunu açýlýþta yapsa da , uyumluluk açýsýndan tekrar ya-
//pýlmasý daha uygundur.
#define PIC_MASTER_PORT_0 0x20    //ICW1'in gönderileceði port numarasý (PIC1)
#define PIC_MASTER_PORT_1 0x21    //ICW2,ICW3 ve ICW4'ün gönderileceði port numarasý (PIC1)
#define PIC_SLAVE_PORT_0 0x0A0    //PIC2 için ilk port
#define PIC_SLAVE_PORT_1 0x0A1    //PIC2 için 2. port
#define EOI	0x20
//------------------------------------------------------------------------------
//PIT programlanmasý için gerekli portlar
#define PIT_1_COUNTER_0  0x40
#define PIT_1_COUNTER_1  0x41
#define PIT_1_COUNTER_2  0x42
#define PIT_1_CONTROL_REGISTER 0x43
#define PIT_2_COUNTER_0  0x48
#define PIT_2_COUNTER_1  0x49
#define PIT_2_COUNTER_2  0x4A
#define PIT_2_CONTROL_REGISTER 0x4B
//------------------------------------------------------------------------------
//8042 Klavye denetleyicisinin programlanabilmesi için gerekli olan portlar
//Bu port numaralarý A20 adres bacaðýnýn aktif hale getirilmesi için kullanýlacaktýr.
#define KEYBOARD_DATA_REGISTER  0x60
#define KEYBOARD_COMMAND_REGISTER  0x64  //komut yazma
#define KEYBOARD_STATUS_REGISTER  0x64   //durum bilgisi alma

#endif