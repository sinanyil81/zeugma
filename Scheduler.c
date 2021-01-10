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
// Sistemdeki çalýþmaya hazir tüm süreçlere ait süreç yapýlarýný 
// tutan liste (Ready Queue)
struct Liste Hazir_Surec_Listesi;
// Sistemdeki IO bekleyen tüm tüm süreçlere ait süreç yapýlarýný 
// tutan liste (Waiting Queue);
struct Liste Bekleyen_Surec_Listesi;
// Sistemdeki çalýþmasýný bitirmiþ süreçleri tutan liste
// (Terminated Queue)
struct Liste Bitmis_Surec_Listesi;
//---------------------------------------------------------------

//---------------------------------------------------------------
//o an çalýþan süreç
struct Task *aktif_surec;
//---------------------------------------------------------------

//-----------------------------------------------------------------------
// Fonksiyon Adý:
//   "freeTask"
// Açýklama:
//	 Sistemde bitmiþ süreç listesinde bulunan süreçlerden bir tanesini 
//  alýr ve o sürece ait adres sahasýný iþletim sistemine geri verir
// Parametreler :
//   YOK
// Geri Dönüþ Deðeri:
//   YOK
//-----------------------------------------------------------------------
void freeTask()
{
	struct Task *task;
	struct Liste *lst;

	//---------------------------------------------------------------
	// bitmiþ süreç listesinin sanal adresini al
	lst=(struct Liste *)phys_to_virt((unsigned long)&Bitmis_Surec_Listesi);
	//---------------------------------------------------------------
	
	//---------------------------------------------------------------
	// eleman yok ise çýk
	if(lst->eleman_sayisi==0)
		return;
	//---------------------------------------------------------------

	//---------------------------------------------------------------
	//eðer liste boþ deðilse 
	task=lst->liste_basi;		//liste baþýný al
	Remove_Task(lst,task);		//listeden çýkart
	deleteProcess(task);        //sürece ait tüm bellek bölgesini iþle-
								//tim sistemine ver
	//---------------------------------------------------------------
}

//-----------------------------------------------------------------------
// Fonksiyon Adý:
//   "Scheduler"
// Açýklama:
//	 Süreç organizasyonun saðlar, iþlemciyi round-robin algoritmasýna
//	göre süreçlere daðýtýr.
// Parametreler :
//   YOK
// Geri Dönüþ Deðeri:
//   YOK
//-----------------------------------------------------------------------
extern void Scheduler()
{
	struct Task *task;
	struct Liste *lst;

	//---------------------------------------------------------------
	// eðer var ise bitmiþ süreç listesinden bir süreci çýkart ve onu
	// sistemden tamamiyle sil
	freeTask();
	//---------------------------------------------------------------

	//---------------------------------------------------------------
	// Ýþlemci paylaþýmýný yapan kod bölgesi ---> Hafýzada birden 
	// fazla sürecin bulundurulmasý ve çalýþtýrýlmasý iþleminin 
	// merkezi
	//
	// hazir süreç listesi, sistemde çalýþmaya hazýr süreçleri tutmaktadýr.
	// algoritmada, hazir süreç listesinin liste baþý elemaný alýnýr ve o
	// an çalýþmakta olan süreç ise yine ayný listenin sonuna eklenir.
	// böylece FCFS algoritmasý ve ROUND_ROBIN algoritmalarý beraberce iþ-
	// lemektedir.

	//---------------------------------------------------------------
	// hazir süreç listesinin sanal adresini al
	lst=(struct Liste *)phys_to_virt((unsigned long)&Hazir_Surec_Listesi);
	//---------------------------------------------------------------

	//---------------------------------------------------------------
	//eðer liste boþ deðilse 
	if (lst->eleman_sayisi!=0)
	{
		//---------------------------------------------------------------
		// hazir süreç listesinden, liste baþý sürecini çýkart
		task=lst->liste_basi;
		Remove_Task(lst,task);
		//---------------------------------------------------------------

		//---------------------------------------------------------------
		//aktif süreci (eðer geçerli bir süreç ise) tekrar hazir sürec 
		//listesinin sonuna ekle...
		if(aktif_surec!=NULL)
		{
			//aktif süreç, artýk çalýþmýyor...
			aktif_surec->Durum=TASK_READY;
			//hazir süreç listesine ekle
			Insert_Task(lst,aktif_surec);
		}
		//---------------------------------------------------------------

		//---------------------------------------------------------------
		//çalýþacak olan sürecimiz artýk yeni süreçtir
		aktif_surec=task;
		//---------------------------------------------------------------

		//---------------------------------------------------------------
		//yeni süreç çalýþacaðý için durumu "ÇALIÞIYOR" yapýlýyor
		task->Durum=TASK_RUNNING;
		//---------------------------------------------------------------
		
		//---------------------------------------------------------------
		//Task switching iþlemi yapýlýyor...(çok basit...:))
		switch_to(task->ID);
		//---------------------------------------------------------------
	}
	//---------------------------------------------------------------
}

//-----------------------------------------------------------------------
// Fonksiyon Adý:
//   "Remove_Task"
// Açýklama:
//	 Sistemde, süreç listesinden verilen süreç çýkartýlýr.
// Parametreler :
//   task -> çýkartýlacak sürecin adresi
//   lst  -> iþlem yapýlacak liste
// Geri Dönüþ Deðeri:
//   YOK
//-----------------------------------------------------------------------
void Remove_Task(struct Liste *lst,struct Task *task)
{
	//---------------------------------------------------------------
	//eðer listede sadece 1 süreç var ise,liste baþ ve sonu 
	//iþlemleri ele alýnmalý
	if(lst->eleman_sayisi==1)
	{
		lst->liste_basi=NULL;
		lst->liste_sonu=NULL;
	}
	else //eðer liste baþý çýkartýlýyorsa
		if(lst->liste_basi==task)
		{
			//liste baþý ayarlamalarý...
			lst->liste_basi=task->sonraki_surec;
			lst->liste_basi->onceki_surec=NULL;
		}
		else //eðer liste sonu çýkartýlýyorsa
			if(lst->liste_sonu==task)
			{
				//liste basi ve sonu ayarlamalarý
				lst->liste_sonu=task->onceki_surec;
				lst->liste_sonu->sonraki_surec=NULL;
			} //eðer normal bir eleman çýkartýlýyorsa
			else
			{
				task->onceki_surec->sonraki_surec=task->sonraki_surec;
				task->sonraki_surec->onceki_surec=task->onceki_surec;
			}
	//---------------------------------------------------------------
	
	(lst->eleman_sayisi)--;
}
//-----------------------------------------------------------------------
// Fonksiyon Adý:
//   "Insert_Task"
// Açýklama:
//	 Sistemde, süreç listesine verilen süreç eklenir.
// Parametreler :
//   task -> eklenecek sürecin adresi
// Geri Dönüþ Deðeri:
//   YOK
//-----------------------------------------------------------------------
void Insert_Task(struct Liste *lst,struct Task *task)
{
	
	//---------------------------------------------------------------
	//listede hiç eleman yoksa
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
		//liste sonundan sonraki süreç, yeni eklenen süreç
		lst->liste_sonu->sonraki_surec=task;
		//eklenen süreçten önceki süreç liste sonu
		task->onceki_surec=lst->liste_sonu;
		//sonraki süreç yok
		task->sonraki_surec=NULL;
		//yeni liste sonu, yeni süreç
		lst->liste_sonu=task;
		//---------------------------------------------------------------
	}
	//---------------------------------------------------------------
	
	(lst->eleman_sayisi)++;
}

//-----------------------------------------------------------------------
// Fonksiyon Adý:
//   "surecBilgisi"
// Açýklama:
//	 Sistemde bulunan surecler ve süreç listeleri hakkýnda bilgi veren
//  fonksiyondur.
// Parametreler :
//   YOK
// Geri Dönüþ Deðeri:
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
	// çalýþmaya hazir süreçlerin tutulduðu listeye ait sanal adres
	//alýnýyor
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
	// bekleyen süreçlerin tutulduðu listeye ait sanal adres
	//alýnýyor
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
	// çalýþmasý bitmiþ süreçlerin tutulduðu listeye ait sanal adres
	//alýnýyor
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
	// o an çalýþan süreç alýnýyor
	aktif=(struct Task **)phys_to_virt((unsigned long)&aktif_surec);
	//---------------------------------------------------------------
	Set_Color(13);
	Println("TASK_RUNNING surec:");
	Set_Color(12);
	Print("         Surec ID ->");Print_Sayi_Hex((*aktif)->ID);Println("");

}

//-----------------------------------------------------------------------
// Fonksiyon Adý:
//   "Init_Scheduler"
// Açýklama:
//	 Sistemde bulunan timer interrupt'ý üzerine yerleþtirilecek iþlem-
//	ci planlayýcýsýnýn ilkleme ve aktivasyon iþlemleri burada yapýlýyor.
// Parametreler :
//   YOK
// Geri Dönüþ Deðeri:
//   YOK
//-----------------------------------------------------------------------
void Init_Scheduler()
{
	char value;
	int i;

	//---------------------------------------------------------------
	//Timer kesmesini iþleyecek kod parçasý IDT'ye iþlensin
	Gate_Doldur((struct i386_Gate *)&Interrupt_Descriptor_Table[0x70],	//gate
				(unsigned long)Timer_Interrupt,	//handler
				sel_KernelCS,						//selector
				0,									//parametre sayisi
				INTERRUPT_GATE|PRESENT);			//access
	//---------------------------------------------------------------

	//---------------------------------------------------------------
	//IRQ 0 = Timer Interrupt aktif hale getiriliyor...
	//interrupt mask alýnýyor
	value=in_port(PIC_MASTER_PORT_1);
	//IRQ 0 aktif hale gelsin
	out_port((value&0xfe),PIC_MASTER_PORT_1);
	//---------------------------------------------------------------

	//---------------------------------------------------------------
	//Hazir Süreç Listesini ilkle
	Hazir_Surec_Listesi.liste_basi=NULL;
	Hazir_Surec_Listesi.liste_sonu=NULL;
	Hazir_Surec_Listesi.eleman_sayisi=0;
	//---------------------------------------------------------------

	//---------------------------------------------------------------
	//Bekleyen Süreç Listesini ilkle
	Bekleyen_Surec_Listesi.liste_basi=NULL;
	Bekleyen_Surec_Listesi.liste_sonu=NULL;
	Bekleyen_Surec_Listesi.eleman_sayisi=0;
	//---------------------------------------------------------------

	//---------------------------------------------------------------
	//Bitmiþ Süreç Listesini ilkle
	Bitmis_Surec_Listesi.liste_basi=NULL;
	Bitmis_Surec_Listesi.liste_sonu=NULL;
	Bitmis_Surec_Listesi.eleman_sayisi=0;
	//---------------------------------------------------------------

	//---------------------------------------------------------------
	//baþlangýçta, çalýþan süreç yok...
	aktif_surec=NULL;
	//---------------------------------------------------------------

	Println("<Zeugma> Scheduler ayarlari yapildi.");
}