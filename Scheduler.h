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
//  ��letim sistemimiz birden fazla s�recin ayn� anda �al��mas�na izin
// verecek �ekilde tasarlanm���tr.Bu sebeple, timer donan�m kesmesine
// yerle�tirilecek olan ve sistemde s�re�lere i�lemciyi e�it zaman
// aral�klari ile da��tacak olan fonksiyon, burada tan�mlanm��t�r.
/////////////////////////////////////////////////////////////////////////////
#ifndef SCHEDULER
#define SCHEDULER

#include "Process.h"

void Remove_Task(struct Liste *lst,struct Task *task);
void Insert_Task(struct Liste *lst,struct Task *task);
void Init_Scheduler();
void surecBilgisi();

#endif