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
#include "Scheduler.h"
#include "Console.h"
#include "Ports.h"
#include "AsmDefines.h"
#include "Descriptor.h"
#include "System.h"
#include "Lib.h"

extern void Timer_Interrupt();


//---------------------------------------------------------------
// Interrupt Descriptor Table tablosu (start.asm'den)
extern struct i386_Descriptor Interrupt_Descriptor_Table[256];
//---------------------------------------------------------------

//---------------------------------------------------------------
// Sistemdeki �al��maya hazir t�m s�re�lere ait s�re� yap�lar�n� 
// tutan liste (Ready Queue)
struct Liste Hazir_Surec_Listesi;
// Sistemdeki IO bekleyen t�m t�m s�re�lere ait s�re� yap�lar�n� 
// tutan liste (Waiting Queue);
struct Liste Bekleyen_Surec_Listesi;
// Sistemdeki �al��mas�n� bitirmi� s�re�leri tutan liste
// (Terminated Queue)
struct Liste Bitmis_Surec_Listesi;
//---------------------------------------------------------------

//---------------------------------------------------------------
//o an �al��an s�re�
struct Task *aktif_surec;
//---------------------------------------------------------------

//-----------------------------------------------------------------------
// Fonksiyon Ad�:
//   "freeTask"
// A��klama:
//	 Sistemde bitmi� s�re� listesinde bulunan s�re�lerden bir tanesini 
//  al�r ve o s�rece ait adres sahas�n� i�letim sistemine geri verir
// Parametreler :
//   YOK
// Geri D�n�� De�eri:
//   YOK
//-----------------------------------------------------------------------
void freeTask()
{
	struct Task *task;
	struct Liste *lst;

	//---------------------------------------------------------------
	// bitmi� s�re� listesinin sanal adresini al
	lst=(struct Liste *)phys_to_virt((unsigned long)&Bitmis_Surec_Listesi);
	//---------------------------------------------------------------
	
	//---------------------------------------------------------------
	// eleman yok ise ��k
	if(lst->eleman_sayisi==0)
		return;
	//---------------------------------------------------------------

	//---------------------------------------------------------------
	//e�er liste bo� de�ilse 
	task=lst->liste_basi;		//liste ba��n� al
	Remove_Task(lst,task);		//listeden ��kart
	deleteProcess(task);        //s�rece ait t�m bellek b�lgesini i�le-
								//tim sistemine ver
	//---------------------------------------------------------------
}

//-----------------------------------------------------------------------
// Fonksiyon Ad�:
//   "Scheduler"
// A��klama:
//	 S�re� organizasyonun sa�lar, i�lemciyi round-robin algoritmas�na
//	g�re s�re�lere da��t�r.
// Parametreler :
//   YOK
// Geri D�n�� De�eri:
//   YOK
//-----------------------------------------------------------------------
extern void Scheduler()
{
	struct Task *task;
	struct Liste *lst;

	//---------------------------------------------------------------
	// e�er var ise bitmi� s�re� listesinden bir s�reci ��kart ve onu
	// sistemden tamamiyle sil
	freeTask();
	//---------------------------------------------------------------

	//---------------------------------------------------------------
	// ��lemci payla��m�n� yapan kod b�lgesi ---> Haf�zada birden 
	// fazla s�recin bulundurulmas� ve �al��t�r�lmas� i�leminin 
	// merkezi
	//
	// hazir s�re� listesi, sistemde �al��maya haz�r s�re�leri tutmaktad�r.
	// algoritmada, hazir s�re� listesinin liste ba�� eleman� al�n�r ve o
	// an �al��makta olan s�re� ise yine ayn� listenin sonuna eklenir.
	// b�ylece FCFS algoritmas� ve ROUND_ROBIN algoritmalar� beraberce i�-
	// lemektedir.

	//---------------------------------------------------------------
	// hazir s�re� listesinin sanal adresini al
	lst=(struct Liste *)phys_to_virt((unsigned long)&Hazir_Surec_Listesi);
	//---------------------------------------------------------------

	//---------------------------------------------------------------
	//e�er liste bo� de�ilse 
	if (lst->eleman_sayisi!=0)
	{
		//---------------------------------------------------------------
		// hazir s�re� listesinden, liste ba�� s�recini ��kart
		task=lst->liste_basi;
		Remove_Task(lst,task);
		//---------------------------------------------------------------

		//---------------------------------------------------------------
		//aktif s�reci (e�er ge�erli bir s�re� ise) tekrar hazir s�rec 
		//listesinin sonuna ekle...
		if(aktif_surec!=NULL)
		{
			//aktif s�re�, art�k �al��m�yor...
			aktif_surec->Durum=TASK_READY;
			//hazir s�re� listesine ekle
			Insert_Task(lst,aktif_surec);
		}
		//---------------------------------------------------------------

		//---------------------------------------------------------------
		//�al��acak olan s�recimiz art�k yeni s�re�tir
		aktif_surec=task;
		//---------------------------------------------------------------

		//---------------------------------------------------------------
		//yeni s�re� �al��aca�� i�in durumu "�ALI�IYOR" yap�l�yor
		task->Durum=TASK_RUNNING;
		//---------------------------------------------------------------
		
		//---------------------------------------------------------------
		//Task switching i�lemi yap�l�yor...(�ok basit...:))
		switch_to(task->ID);
		//---------------------------------------------------------------
	}
	//---------------------------------------------------------------
}

//-----------------------------------------------------------------------
// Fonksiyon Ad�:
//   "Remove_Task"
// A��klama:
//	 Sistemde, s�re� listesinden verilen s�re� ��kart�l�r.
// Parametreler :
//   task -> ��kart�lacak s�recin adresi
//   lst  -> i�lem yap�lacak liste
// Geri D�n�� De�eri:
//   YOK
//-----------------------------------------------------------------------
void Remove_Task(struct Liste *lst,struct Task *task)
{
	//---------------------------------------------------------------
	//e�er listede sadece 1 s�re� var ise,liste ba� ve sonu 
	//i�lemleri ele al�nmal�
	if(lst->eleman_sayisi==1)
	{
		lst->liste_basi=NULL;
		lst->liste_sonu=NULL;
	}
	else //e�er liste ba�� ��kart�l�yorsa
		if(lst->liste_basi==task)
		{
			//liste ba�� ayarlamalar�...
			lst->liste_basi=task->sonraki_surec;
			lst->liste_basi->onceki_surec=NULL;
		}
		else //e�er liste sonu ��kart�l�yorsa
			if(lst->liste_sonu==task)
			{
				//liste basi ve sonu ayarlamalar�
				lst->liste_sonu=task->onceki_surec;
				lst->liste_sonu->sonraki_surec=NULL;
			} //e�er normal bir eleman ��kart�l�yorsa
			else
			{
				task->onceki_surec->sonraki_surec=task->sonraki_surec;
				task->sonraki_surec->onceki_surec=task->onceki_surec;
			}
	//---------------------------------------------------------------
	
	(lst->eleman_sayisi)--;
}
//-----------------------------------------------------------------------
// Fonksiyon Ad�:
//   "Insert_Task"
// A��klama:
//	 Sistemde, s�re� listesine verilen s�re� eklenir.
// Parametreler :
//   task -> eklenecek s�recin adresi
// Geri D�n�� De�eri:
//   YOK
//-----------------------------------------------------------------------
void Insert_Task(struct Liste *lst,struct Task *task)
{
	
	//---------------------------------------------------------------
	//listede hi� eleman yoksa
	if(lst->eleman_sayisi==0)
	{
		//---------------------------------------------------------------
		lst->liste_basi=task;
		lst->liste_sonu=task;
		task->onceki_surec=NULL;
		task->sonraki_surec=NULL;
		//---------------------------------------------------------------
	}
	else
	{
		//---------------------------------------------------------------
		//liste sonundan sonraki s�re�, yeni eklenen s�re�
		lst->liste_sonu->sonraki_surec=task;
		//eklenen s�re�ten �nceki s�re� liste sonu
		task->onceki_surec=lst->liste_sonu;
		//sonraki s�re� yok
		task->sonraki_surec=NULL;
		//yeni liste sonu, yeni s�re�
		lst->liste_sonu=task;
		//---------------------------------------------------------------
	}
	//---------------------------------------------------------------
	
	(lst->eleman_sayisi)++;
}

//-----------------------------------------------------------------------
// Fonksiyon Ad�:
//   "surecBilgisi"
// A��klama:
//	 Sistemde bulunan surecler ve s�re� listeleri hakk�nda bilgi veren
//  fonksiyondur.
// Parametreler :
//   YOK
// Geri D�n�� De�eri:
//   YOK
//-----------------------------------------------------------------------
void surecBilgisi()
{
	struct Liste *lst;
	struct Task *task;
	struct Task **aktif;

	Set_Color(13);
	Println("Sistemdeki ana surecler:");
	Set_Color(12);
	Println("         Init sureci icin ID 0");
	Println("         Shell sureci icin ID 1");

	//---------------------------------------------------------------
	// �al��maya hazir s�re�lerin tutuldu�u listeye ait sanal adres
	//al�n�yor
	lst=(struct Liste *)phys_to_virt((unsigned long)&Hazir_Surec_Listesi);
	//---------------------------------------------------------------

	//---------------------------------------------------------------
	//listeyi dolas
	Set_Color(13);
	Println("TASK_READY surecler:");
	Set_Color(12);
	if(lst->eleman_sayisi==0)
		Println("         Bu listede eleman yok.");
	else
	{
		task=lst->liste_basi;
		while(task!=NULL)
		{
			Print("         Surec ID ->");Print_Sayi_Hex(task->ID);Println("");
			task=task->sonraki_surec;
		}
		Set_Color(10);
		Print("         Toplam->");Print_Sayi_Hex(lst->eleman_sayisi);Println("");
	}
	//---------------------------------------------------------------

	//---------------------------------------------------------------
	// bekleyen s�re�lerin tutuldu�u listeye ait sanal adres
	//al�n�yor
	lst=(struct Liste *)phys_to_virt((unsigned long)&Bekleyen_Surec_Listesi);
	//---------------------------------------------------------------

	//---------------------------------------------------------------
	//listeyi dolas
	Set_Color(13);
	Println("TASK_WAITING surecler:");
	Set_Color(12);
	if(lst->eleman_sayisi==0)
		Println("         Bu listede eleman yok.");
	else
	{
		task=lst->liste_basi;
		while(task!=NULL)
		{
			Print("         Surec ID ->");Print_Sayi_Hex(task->ID);Println("");
			task=task->sonraki_surec;
		}
		Set_Color(10);
		Print("         Toplam->");Print_Sayi_Hex(lst->eleman_sayisi);Println("");
	}
	//---------------------------------------------------------------

	//---------------------------------------------------------------
	// �al��mas� bitmi� s�re�lerin tutuldu�u listeye ait sanal adres
	//al�n�yor
	lst=(struct Liste *)phys_to_virt((unsigned long)&Bitmis_Surec_Listesi);
	//---------------------------------------------------------------

	//---------------------------------------------------------------
	//listeyi dolas
	Set_Color(13);
	Println("TASK_TERMINATED surecler:");
	Set_Color(12);
	if(lst->eleman_sayisi==0)
		Println("         Bu listede eleman yok.");
	else
	{
		task=lst->liste_basi;
		while(task!=NULL)
		{
			Print("         Surec ID ->");Print_Sayi_Hex(task->ID);Println("");
			task=task->sonraki_surec;
		}
		Set_Color(10);
		Print("         Toplam->");Print_Sayi_Hex(lst->eleman_sayisi);Println("");
	}
	//---------------------------------------------------------------
	
	//---------------------------------------------------------------
	// o an �al��an s�re� al�n�yor
	aktif=(struct Task **)phys_to_virt((unsigned long)&aktif_surec);
	//---------------------------------------------------------------
	Set_Color(13);
	Println("TASK_RUNNING surec:");
	Set_Color(12);
	Print("         Surec ID ->");Print_Sayi_Hex((*aktif)->ID);Println("");

}

//-----------------------------------------------------------------------
// Fonksiyon Ad�:
//   "Init_Scheduler"
// A��klama:
//	 Sistemde bulunan timer interrupt'� �zerine yerle�tirilecek i�lem-
//	ci planlay�c�s�n�n ilkleme ve aktivasyon i�lemleri burada yap�l�yor.
// Parametreler :
//   YOK
// Geri D�n�� De�eri:
//   YOK
//-----------------------------------------------------------------------
void Init_Scheduler()
{
	char value;
	int i;

	//---------------------------------------------------------------
	//Timer kesmesini i�leyecek kod par�as� IDT'ye i�lensin
	Gate_Doldur((struct i386_Gate *)&Interrupt_Descriptor_Table[0x70],	//gate
				(unsigned long)Timer_Interrupt,	//handler
				sel_KernelCS,						//selector
				0,									//parametre sayisi
				INTERRUPT_GATE|PRESENT);			//access
	//---------------------------------------------------------------

	//---------------------------------------------------------------
	//IRQ 0 = Timer Interrupt aktif hale getiriliyor...
	//interrupt mask al�n�yor
	value=in_port(PIC_MASTER_PORT_1);
	//IRQ 0 aktif hale gelsin
	out_port((value&0xfe),PIC_MASTER_PORT_1);
	//---------------------------------------------------------------

	//---------------------------------------------------------------
	//Hazir S�re� Listesini ilkle
	Hazir_Surec_Listesi.liste_basi=NULL;
	Hazir_Surec_Listesi.liste_sonu=NULL;
	Hazir_Surec_Listesi.eleman_sayisi=0;
	//---------------------------------------------------------------

	//---------------------------------------------------------------
	//Bekleyen S�re� Listesini ilkle
	Bekleyen_Surec_Listesi.liste_basi=NULL;
	Bekleyen_Surec_Listesi.liste_sonu=NULL;
	Bekleyen_Surec_Listesi.eleman_sayisi=0;
	//---------------------------------------------------------------

	//---------------------------------------------------------------
	//Bitmi� S�re� Listesini ilkle
	Bitmis_Surec_Listesi.liste_basi=NULL;
	Bitmis_Surec_Listesi.liste_sonu=NULL;
	Bitmis_Surec_Listesi.eleman_sayisi=0;
	//---------------------------------------------------------------

	//---------------------------------------------------------------
	//ba�lang��ta, �al��an s�re� yok...
	aktif_surec=NULL;
	//---------------------------------------------------------------

	Println("<Zeugma> Scheduler ayarlari yapildi.");
}