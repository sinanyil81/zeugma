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
//  Süreçler ile ilgili tanýmlamalarýn yapýldýðý kod bölgesidir.
/////////////////////////////////////////////////////////////////////////////
#ifndef PROCESS
#define PROCESS

#include "Memory.h"

#define TASK_RUNNING	1
#define TASK_READY		2
#define	TASK_WAITING	3
#define	TASK_TERMINATED	4

//------------------------------------------------------------------------
//Intel 386 ve sonrasý iþlemcileri için donanýmsal olarak tanýmlanmýþ 
// TSS (task state segment) yani süreç durum bilgilerini tutan yapý
struct task_state_segment
{
	long	Previous_Link;
	long	ESP0;
	long	SS0;	
	long	ESP1;
	long	SS1;	
	long	ESP2;
	long	SS2;	
	long	CR3;
	long	EIP;
	long	EFlags;
	long	EAX;
	long	ECX;
	long	EDX;
	long	EBX;
	long	ESP;
	long	EBP;
	long	ESI;
	long	EDI;
	long	ES;		
	long	CS;		
	long	SS;		
	long	DS;		
	long	FS;		
	long	GS;		
	long	LDT_selector;	
	long	IO_Bitmap_Base_Adress;	//31-16 I/O Bitmap, Bit 0->debug trap
};

// Ýþletim sistemimizde yer alacak olan bir sürecin tamamen iç yapýsýný
// tutan C yapýsý
struct Task
{
	//Sürece ait hafýza tablolarýnu tutan yapý---
	struct address_space *addr_space;
	//-------------------------------------------

	//sürece ait özel veriler--------------------
	unsigned long ID;			//süreç ID
	char Durum;					//süreçin durumu
	//-------------------------------------------

	//-------------------------------------------
	//sürecin klavye tamponu
	char keyboard_buffer[256];
	//-------------------------------------------

	// sürecin segment bilgileri...--------------
	//(fiziksel adres olarak)
	unsigned long code_segment_base;
	//-------------------------------------------

	//sürecin durum bölgesi----------------------
	struct task_state_segment Tss; //sürecin durumu saklanacak
	//-------------------------------------------
	
	//-------------------------------------------
	// o sürecin sürec kuyruðundaki diðer elemanlar 
	// ile baðlantýsýný saðlayan deðiþkenler
	struct Task *onceki_surec;
	struct Task *sonraki_surec;
};

//------------------------------------------------------------------------
// sistemdeki süreçleri tutan süreç listesi için bir yapý
struct Liste
{
	struct Task *liste_basi;
	struct Task *liste_sonu;
	unsigned long eleman_sayisi;
};

//---------------------------------------------------------------
// Bazý önemli makrolar...
#define FIRST_TSS_ENTRY 5

//verilen indexi GDT'deki indexe dönüþtüren makrolar
//TSS= (ilkTSS + n) << 3
#define _TSS(n) (((unsigned long) n + FIRST_TSS_ENTRY) << 3)

//Task ve LDT yazmaçlarýný yükleyen makrolar.
#define ltr(n) __asm__("ltr %%ax"::"a" (_TSS(n)))
#define lldt() __asm__("lldt %%ax"::"a" (sel_LDT))
//---------------------------------------------------------------

//---------------------------------------------------------------
// süreçler arasý geçiþi saðlayan makro
#define switch_to(n) {\
					  struct {long a,b;} __tmp; \
					  __asm__("movw %%dx,%1\n\t" \
					 "ljmp %0" \
					 ::"m" (*&__tmp.a),"m" (*&__tmp.b),"d" _TSS(n));}
//---------------------------------------------------------------

void initTask();
void Exit();
void Exec(unsigned long code_segment_base);
void deleteProcess(struct Task *task);
void killProcess(unsigned long taskID);
#endif