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
//  Ýþletim sisteminin donaným kesmelerini yerleþtirmesi ve bu kes-
//  meler yardýmý ile sistemi denetlemesi görevlerinden sorumludur.
/////////////////////////////////////////////////////////////////////////////
#include "Timer.h"
#include "AsmDefines.h"
#include "Console.h"
#include "Ports.h"

#define LATCH (1193180/HZ)
         
//-----------------------------------------------------------------------
// Fonksiyon Adý:
//   "Init_Timer"
// Açýklama:
//	 Sistemde bulunan PIT (Programmable Interval Timer)'ý programlar
// Parametreler :
//   YOK
// Geri Dönüþ Deðeri:
//   YOK
//-----------------------------------------------------------------------
void Init_Timer()
{
	out_port(0x36,PIT_1_CONTROL_REGISTER); //binary ,
								//mode 3 (Square Wave Rate Generator)
								//0.kanal
								//LSB ve MSB
	//PIT 18.2 hertz sinyal üretmesi için programlanýyor.
	out_port(LATCH & 0xff,PIT_1_COUNTER_0) //LSB (Least significant byte) 
	out_port(LATCH >> 8 ,PIT_1_COUNTER_0)  //MSB (Most significant byte )	

	Println("<Zeugma> PIT(Programmable Interval Timer) cipi programlandi.");
}
