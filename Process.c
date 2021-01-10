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
#include "Process.h"
#include "AsmDefines.h"
#include "Descriptor.h"
#include "Memory.h"
#include "Scheduler.h"
#include "Console.h"
#include "System.h"

//---------------------------------------------------------------
// Global Descriptor Table tablosu (Start.asm'den)
extern struct i386_Descriptor Global_Descriptor_Table[256];
//---------------------------------------------------------------

//---------------------------------------------------------------
//tüm süreçlerin adres sahasýna eklenecek kernel sayfa tablolarý
extern unsigned long *KernelPageTable_High;  
//---------------------------------------------------------------;

//---------------------------------------------------------------
//(Scheduler.c)
extern struct Liste Hazir_Surec_Listesi;
extern struct Liste Bitmis_Surec_Listesi;
extern struct Liste Bekleyen_Surec_Listesi;

//(Scheduler.c)
extern struct Task *aktif_surec;
//---------------------------------------------------------------

//---------------------------------------------------------------
static unsigned long Surec_Sayisi=0;// sistemde bulunan tüm süreclerin
									// toplam sayýsýný tutan deðiþken
//---------------------------------------------------------------

//-----------------------------------------------------------------------
// Fonksiyon Adý:
//   "LDT_Init"
// Açýklama:
//	Sistemde tüm süreçlerin kullanacaðý bir LDT tablosu oluþturur.
// Parametreler :
//	YOK
// Geri Dönüþ Deðeri:
//   YOK
//-----------------------------------------------------------------------
void initTask()
{

	//---------------------------------------------------------------
	// ilk context-switch için gerekli TSS yaratýlýyor...
	Descriptor_Doldur(&Global_Descriptor_Table[100], //rastgele bir index
					  0x68,	//104 byte=TSS uzunlugu
					  (unsigned long)0x300000,  //7.megabyte'tan baþla
					  PRESENT | TSS,
					  AVAILABLE);
	//---------------------------------------------------------------

	//---------------------------------------------------------------
	// gerekli yazmaçlarý ilkle
	ltr(95);//TR'yi yükle.verilen 100. girdi 95. süreçin 
			//TSS'sine karþýlýk geliyor
	//---------------------------------------------------------------

	Println("<Zeugma> Surec yonetimi icin GDT ilklendi.");
}

//-----------------------------------------------------------------------
// Fonksiyon Adý:
//   "fill_GDT"
// Açýklama:
//	Sistemde sürece ait TSS alanýna iliþkin tanýmlayýcýyý GDT'ye
//	ekler.
// Parametreler :
//	task -> GDT deðerleri dolacak olan süreç yapýsý
// Geri Dönüþ Deðeri:
//   YOK
//-----------------------------------------------------------------------
void fill_GDT(struct Task *task)
{
	int GDT_index;
	struct i386_Descriptor *gdt;
	
	//---------------------------------------------------------------
	//o sürecin LDT ve TSS bölgeleri için gerekli GDT selektör indexi
	//hesaplanýyor
	// 0->NULL desc.
	// 1->KernelCS desc.
	// 2->KernelDS desc.
	// 3->UserCS desc.
	// 4->UserDS desc.
	//	--------	
	// 5-> TSS0 |-- 0.süreç için TSS 
	//  --------
	// 6-> TSS1 |-- 1.süreç için TSS 
	//  --------
	// |
	// |
	// V
	//----------------------------------------------------------------
	
	GDT_index=task->ID + 5;
	gdt=(struct i386_Descriptor *)phys_to_virt((unsigned long)Global_Descriptor_Table);
	//---------------------------------------------------------------
	//sürecin TSS'sine ait GDT descriptorunu doldur
	Descriptor_Doldur(&gdt[GDT_index],
					  0x68,	//104 byte=TSS uzunlugu
					  (unsigned long)&task->Tss,
					  PRESENT | TSS,
					  AVAILABLE);
	//---------------------------------------------------------------
}

//-----------------------------------------------------------------------
// Fonksiyon Adý:
//   "fillTSS"
// Açýklama:
//	Sürecin gerekli durum bilgilerinin saklandýðý TSS yapýsýný doldur
// Parametreler :
//  task ->	süreç yapýsý
// Geri Dönüþ Deðeri:
//   YOK
//-----------------------------------------------------------------------
void fill_TSS(struct Task *task)
{
	//---------------------------------------------------------------
	//segment selektörleri GDT deðerlerini göstersin
	task->Tss.CS=sel_UserCS;
	task->Tss.DS=sel_UserDS;
	task->Tss.ES=sel_UserDS;
	task->Tss.FS=sel_UserDS;
	task->Tss.GS=sel_UserDS;
	task->Tss.SS=sel_UserDS;
	//---------------------------------------------------------------

	//---------------------------------------------------------------
	//yýðýt segmenti kullanýcý yýðýt segmentini göstersin 
	task->Tss.ESP=(unsigned long)allocUserPages(1,task->addr_space->user_ptable_1)+4095; 
	//---------------------------------------------------------------

	//---------------------------------------------------------------
	//SS0 ve ESP0 kernel yýðýt segmentini göstersin 
	task->Tss.SS0=sel_KernelDS;
	task->Tss.ESP0=(unsigned long)allocKernelPages(1)+4095;
	//---------------------------------------------------------------

	//---------------------------------------------------------------
	//sürecin tüm yazmaçlarýný ilk baþta sýfýrla	
	task->Tss.EAX=0;
	task->Tss.EBX=0;
	task->Tss.ECX=0;
	task->Tss.EDX=0;
	task->Tss.ESI=0;
	task->Tss.EDI=0;
	task->Tss.EBP=0;
	//---------------------------------------------------------------

	//---------------------------------------------------------------
	//flag deðeri yazýlýyor
	task->Tss.EFlags=EFLG_IF | EFLG_IOPL3; //interrupt enable ve IOPL
	task->Tss.EIP=0;
	task->Tss.IO_Bitmap_Base_Adress=0;
	task->Tss.LDT_selector=0;
	//---------------------------------------------------------------
}

//-----------------------------------------------------------------------
// Fonksiyon Adý:
//   "createPageTables"
// Açýklama:
//	verilen sürecin sayfa tablolarýna yer ayýrýr, onlarý doldurur.
// Parametreler :
//  task ->	süreç yapýsý
// Geri Dönüþ Deðeri:
//   YOK
//-----------------------------------------------------------------------
void createPageTables(struct Task *task)
{
	void *physical;
	void *virt;
	int i;

	//---------------------------------------------------------------
	//sürece ait sayfa dizin tablosu ve sayfa tablolarý  doldurulmalý.
	//Bu nedenle sürece ait sayfa dizin tablosu ve sayfa tablolarý için
	//gerekli yer, yine iþletim sistemi tarafýndan atanmalý
	allocPages(3,&physical);
	mapPages(KernelPageTable_High,3,PTE_PRESENT|PTE_WRITE,
			 VIRT_OFFSET,physical,&virt);	
	task->addr_space=(struct address_space *)virt;
	//---------------------------------------------------------------
	
	//---------------------------------------------------------------
	// sayfa tablolarý ilkleniyor...
	for(i=0;i<1024;i++)
	{
		task->addr_space->pdir[i]=0;
		task->addr_space->user_ptable_0[i]=i*4096|PTE_PRESENT|PTE_WRITE|PTE_USER;
		task->addr_space->user_ptable_1[i]=0;
	}

	// sayfa dizininin ilk elemaný, sürece ait ilk sayfa tablosunu göste-
	// riyor.
	task->addr_space->pdir[0]= (unsigned long)(physical+4096)|PDE_PRESENT|PDE_WRITE|PDE_USER;
	task->addr_space->pdir[1]= (unsigned long)(physical+4096*2)|PDE_PRESENT|PDE_WRITE|PDE_USER;
	// ikinci eleman ise kernel sayfa tablosunu gösteriyor(fiziksel adres)
	task->addr_space->pdir[2]= (unsigned long)KernelPageTable_High|PDE_PRESENT|PDE_WRITE;
	//---------------------------------------------------------------

	//---------------------------------------------------------------
	//sürecin sayfa tablosunu göster
	task->Tss.CR3=(unsigned long)physical;
	//---------------------------------------------------------------
}

//-----------------------------------------------------------------------
// Fonksiyon Adý:
//   "Create_Process"
// Açýklama:
//	Sistemde süreç oluþturmak için kullanýlan temel fonksiyondur.
// Parametreler :
//  task ->	yaratýlacak süreç için yapý
// Geri Dönüþ Deðeri:
//   YOK
//-----------------------------------------------------------------------
struct Task *createProcess()
{
	struct Task *yeni_surec;
	
	//---------------------------------------------------------------
	//Sürece ait bilgilerin iþletim sistemi tarafýndan tutulmasý için
	//gerekli fiziksel bellek ayýrma iþlemleri...
	//boþ bir sayfa al
	//sürece ait bilgiler bu sayfa içerisinde tutulacak
	yeni_surec=(struct Task *)allocKernelPages(1);
	//---------------------------------------------------------------

	//---------------------------------------------------------------
	//sürece ait sayfa tablolarýný doldur
	createPageTables(yeni_surec);
	//---------------------------------------------------------------

	//---------------------------------------------------------------
	//süreçin ID'sini süreç listesindeki eleman sayýsý olarak atanýyor
	yeni_surec->ID=Surec_Sayisi;
	//---------------------------------------------------------------

	//---------------------------------------------------------------
	//sürece ait sayfa durum bilgilerini doldur
	fill_TSS(yeni_surec);
	//---------------------------------------------------------------

	//---------------------------------------------------------------
	//o sürecin LDT ve TSS bölgeleri için gerekli GDT girdilerini
	//yarat
	fill_GDT(yeni_surec);
	//---------------------------------------------------------------

	//---------------------------------------------------------------
	//süreç çalýþtýrýlmaya hazýr...
	yeni_surec->Durum=TASK_READY;
	//---------------------------------------------------------------

	//sistemdeki süreç sayýsýný 1 arttýr
	Surec_Sayisi++;

	return yeni_surec;
}

//-----------------------------------------------------------------------
// Fonksiyon Adý:
//   "Exec"
// Açýklama:
//	Sistemde süreç oluþturmak için kullanýlan temel fonksiyondur.
// Parametreler :
//  code_segment_base -> süreçin kod baþlangýç adresi(fiziksel)
// Geri Dönüþ Deðeri:
//   YOK
//-----------------------------------------------------------------------
void Exec(unsigned long code_segment_base)
{
	struct Task *yeni_surec;		//oluþturulacak süreç için yapý

	//---------------------------------------------------------------
	//yeni bir süreç oluþtur
	yeni_surec=(struct Task *)createProcess();
	//---------------------------------------------------------------

	//---------------------------------------------------------------
	//süreç yapýsý dolduruluyor...
	yeni_surec->code_segment_base=code_segment_base;
	yeni_surec->Tss.EIP=code_segment_base;
	//---------------------------------------------------------------

	//---------------------------------------------------------------
	Insert_Task((struct Liste *)phys_to_virt((unsigned long)&Hazir_Surec_Listesi),
				yeni_surec);
	//süreç, süreç kuyruðuna yerleþtirildi...Çalýþtýrýlmaya hazýr...
	//---------------------------------------------------------------
}

//-----------------------------------------------------------------------
// Fonksiyon Adý:
//   "deleteProcess"
// Açýklama:
//	Sistemde bulunan bir süreçi silmek için kullanýlýr
// Parametreler :
//  task -> silinecek süreçe iþaretçi
// Geri Dönüþ Deðeri:
//   YOK
//-----------------------------------------------------------------------
void deleteProcess(struct Task *task)
{
	struct address_space *addr_spc;

	//---------------------------------------------------------------
	//süreç,o an zaten aktif süreç listesinden çýkartýlmýþtýr ve ça-
	//lýþan süreçtir.dolayýsýyla, sadece sürece ait bellek bölgeleri-
	//ni geri vermemiz, yeterli olacaktir.
	//---------------------------------------------------------------

	addr_spc=task->addr_space;

	//---------------------------------------------------------------
	// sürece ait kullanýcý yýðýtý sisteme veriliyor
	freePages(addr_spc,1,(void *)task->Tss.ESP);
	//---------------------------------------------------------------

	//---------------------------------------------------------------
	// sürece ait kernel yýðýtý sisteme veriliyor
	freePages(addr_spc,1,(void *)task->Tss.ESP0);
	//---------------------------------------------------------------

	//---------------------------------------------------------------
	// sürece ait süreç yapýsý sisteme veriliyor
	freePages(addr_spc,1,(void *)task);
	//---------------------------------------------------------------

	//---------------------------------------------------------------
	// sürece ait sayfa tablolarý için ayýrýlmýþ bölgeler de tekrar 
	// sisteme veriliyor.
	freePages(addr_spc,3,(void *)addr_spc);
	//---------------------------------------------------------------

}

//-----------------------------------------------------------------------
// Fonksiyon Adý:
//   "Exit"
// Açýklama:
//	Sistemde bulunan bir süreç,bu çaðýrým sayesinde çalýþmasýný sonlandýrýr.
// Parametreler :
//  YOK
// Geri Dönüþ Deðeri:
//   YOK
//-----------------------------------------------------------------------
void Exit()
{
	struct Task **surec;
	struct Task *terminated_task;

	//---------------------------------------------------------------
	// Kesmeleri kapat
	cli();
	//---------------------------------------------------------------

	//---------------------------------------------------------------
	// o an aktif olan surecin sanal adresini al
	surec=(struct Task **)phys_to_virt((unsigned long)&aktif_surec);
	
	terminated_task=(*surec);
	//---------------------------------------------------------------

	//---------------------------------------------------------------
	// aktif sureci NULL yap
	(*surec)=NULL;
	//---------------------------------------------------------------

	//---------------------------------------------------------------
	// sureci bitmiþ süreçler listesine koy
	terminated_task->Durum=TASK_TERMINATED;
	Insert_Task((struct Liste *)phys_to_virt((unsigned long)&Bitmis_Surec_Listesi),
				terminated_task);
	//---------------------------------------------------------------

	//---------------------------------------------------------------
	// Kesmeleri aç
	sti();
	//---------------------------------------------------------------
	
	//---------------------------------------------------------------
	// iþlemci bölüþtürücüsünü çaðýr ki o süreci çalýþtýrmayý kessin
	Scheduler();
	//---------------------------------------------------------------
}

//-----------------------------------------------------------------------
// Fonksiyon Adý:
//   "surecBul"
// Açýklama:
//	 ID'si ve liste verilen süreci geri döndürür
// Parametreler :
//   ID -> sürecin ID'si
//   lst-> aranacak liste
// Geri Dönüþ Deðeri:
//   bulunan task
//-----------------------------------------------------------------------
struct Task *surecBul(struct Liste *lst,unsigned long ID)
{
	unsigned long bulundu;
	struct Task *task;

	bulundu=0;

	//---------------------------------------------------------------
	// elemaný listede ara
	if(lst->eleman_sayisi!=0)
	{
		task=lst->liste_basi;
		while(task!=NULL)
		{
			if(task->ID==ID)
			{
				bulundu=1;
				break;
			}
			else 
				task=task->sonraki_surec;
		}
	}
	//---------------------------------------------------------------

	//---------------------------------------------------------------
	// eðer bulunduysa listeden çýkart ve döndür
	if(bulundu)
	{
		Remove_Task(lst,task);
		return task;
	}
	else
		return NULL;
	//---------------------------------------------------------------
}

//-----------------------------------------------------------------------
// Fonksiyon Adý:
//   "endProcess"
// Açýklama:
//	 verilen süreci bitmis süreçler listesine koyar
// Parametreler :
//   task -> sonlanacak süreç
// Geri Dönüþ Deðeri:
//   YOK
//-----------------------------------------------------------------------
void endTask(struct Task *task)
{
	cli();
	//---------------------------------------------------------------
	// sureci bitmiþ süreçler listesine koy
	task->Durum=TASK_TERMINATED;
	Insert_Task((struct Liste *)phys_to_virt((unsigned long)&Bitmis_Surec_Listesi),
				task);
	//---------------------------------------------------------------
	sti();

}
//-----------------------------------------------------------------------
// Fonksiyon Adý:
//   "killProcess"
// Açýklama:
//	 ID'si verilen süreci öldürür.
// Parametreler :
//   ID -> sürecin ID'si
// Geri Dönüþ Deðeri:
//   YOK
//-----------------------------------------------------------------------
void killProcess(unsigned long taskID)
{
	struct Liste *lst;
	struct Task *task;

	Set_Color(5);
	//---------------------------------------------------------------
	// eðer shell veya init öldürülmek istenirse buna izin verme
	if((taskID==0)||(taskID==1))
	{
		Println("<kill> init veya shell oldurulemez!");
		return ;
	}
	//---------------------------------------------------------------

	//---------------------------------------------------------------
	// çalýþmaya hazir süreçlerin tutulduðu listeye ait sanal adres
	//alýnýyor
	lst=(struct Liste *)phys_to_virt((unsigned long)&Hazir_Surec_Listesi);
	task=surecBul(lst,taskID);
	if(task!=NULL)
	{
		endTask(task);
		return;
	}
	//---------------------------------------------------------------

	//---------------------------------------------------------------
	// bekleyen süreçlerin tutulduðu listeye ait sanal adres
	//alýnýyor
	lst=(struct Liste *)phys_to_virt((unsigned long)&Bekleyen_Surec_Listesi);
	task=surecBul(lst,taskID);
	if(task!=NULL)
	{
		endTask(task);
		return ;
	}
	//---------------------------------------------------------------
	Println("<kill> surec yaratilmamis!");
}
