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
//  Ýþletim sistemimiz birden fazla sürecin ayný anda çalýþmasýna izin
// verecek þekilde tasarlanmýþýtr.Bu sebeple, timer donaným kesmesine
// yerleþtirilecek olan ve sistemde süreçlere iþlemciyi eþit zaman
// aralýklari ile daðýtacak olan fonksiyon, burada tanýmlanmýþtýr.
/////////////////////////////////////////////////////////////////////////////
#ifndef SCHEDULER
#define SCHEDULER

#include "Process.h"

void Remove_Task(struct Liste *lst,struct Task *task);
void Insert_Task(struct Liste *lst,struct Task *task);
void Init_Scheduler();
void surecBilgisi();

#endif