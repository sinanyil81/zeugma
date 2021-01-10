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
//  S�re�ler ile ilgili tan�mlamalar�n yap�ld��� kod b�lgesidir.
/////////////////////////////////////////////////////////////////////////////
#ifndef PROCESS
#define PROCESS

#include "Memory.h"

#define TASK_RUNNING	1
#define TASK_READY		2
#define	TASK_WAITING	3
#define	TASK_TERMINATED	4

//------------------------------------------------------------------------
//Intel 386 ve sonras� i�lemcileri i�in donan�msal olarak tan�mlanm�� 
// TSS (task state segment) yani s�re� durum bilgilerini tutan yap�
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

// ��letim sistemimizde yer alacak olan bir s�recin tamamen i� yap�s�n�
// tutan C yap�s�
struct Task
{
	//S�rece ait haf�za tablolar�nu tutan yap�---
	struct address_space *addr_space;
	//-------------------------------------------

	//s�rece ait �zel veriler--------------------
	unsigned long ID;			//s�re� ID
	char Durum;					//s�re�in durumu
	//-------------------------------------------

	//-------------------------------------------
	//s�recin klavye tamponu
	char keyboard_buffer[256];
	//-------------------------------------------

	// s�recin segment bilgileri...--------------
	//(fiziksel adres olarak)
	unsigned long code_segment_base;
	//-------------------------------------------

	//s�recin durum b�lgesi----------------------
	struct task_state_segment Tss; //s�recin durumu saklanacak
	//-------------------------------------------
	
	//-------------------------------------------
	// o s�recin s�rec kuyru�undaki di�er elemanlar 
	// ile ba�lant�s�n� sa�layan de�i�kenler
	struct Task *onceki_surec;
	struct Task *sonraki_surec;
};

//------------------------------------------------------------------------
// sistemdeki s�re�leri tutan s�re� listesi i�in bir yap�
struct Liste
{
	struct Task *liste_basi;
	struct Task *liste_sonu;
	unsigned long eleman_sayisi;
};

//---------------------------------------------------------------
// Baz� �nemli makrolar...
#define FIRST_TSS_ENTRY 5

//verilen indexi GDT'deki indexe d�n��t�ren makrolar
//TSS= (ilkTSS + n) << 3
#define _TSS(n) (((unsigned long) n + FIRST_TSS_ENTRY) << 3)

//Task ve LDT yazma�lar�n� y�kleyen makrolar.
#define ltr(n) __asm__("ltr %%ax"::"a" (_TSS(n)))
#define lldt() __asm__("lldt %%ax"::"a" (sel_LDT))
//---------------------------------------------------------------

//---------------------------------------------------------------
// s�re�ler aras� ge�i�i sa�layan makro
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