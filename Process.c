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
//t�m s�re�lerin adres sahas�na eklenecek kernel sayfa tablolar�
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
static unsigned long Surec_Sayisi=0;// sistemde bulunan t�m s�reclerin
									// toplam say�s�n� tutan de�i�ken
//---------------------------------------------------------------

//-----------------------------------------------------------------------
// Fonksiyon Ad�:
//   "LDT_Init"
// A��klama:
//	Sistemde t�m s�re�lerin kullanaca�� bir LDT tablosu olu�turur.
// Parametreler :
//	YOK
// Geri D�n�� De�eri:
//   YOK
//-----------------------------------------------------------------------
void initTask()
{

	//---------------------------------------------------------------
	// ilk context-switch i�in gerekli TSS yarat�l�yor...
	Descriptor_Doldur(&Global_Descriptor_Table[100], //rastgele bir index
					  0x68,	//104 byte=TSS uzunlugu
					  (unsigned long)0x300000,  //7.megabyte'tan ba�la
					  PRESENT | TSS,
					  AVAILABLE);
	//---------------------------------------------------------------

	//---------------------------------------------------------------
	// gerekli yazma�lar� ilkle
	ltr(95);//TR'yi y�kle.verilen 100. girdi 95. s�re�in 
			//TSS'sine kar��l�k geliyor
	//---------------------------------------------------------------

	Println("<Zeugma> Surec yonetimi icin GDT ilklendi.");
}

//-----------------------------------------------------------------------
// Fonksiyon Ad�:
//   "fill_GDT"
// A��klama:
//	Sistemde s�rece ait TSS alan�na ili�kin tan�mlay�c�y� GDT'ye
//	ekler.
// Parametreler :
//	task -> GDT de�erleri dolacak olan s�re� yap�s�
// Geri D�n�� De�eri:
//   YOK
//-----------------------------------------------------------------------
void fill_GDT(struct Task *task)
{
	int GDT_index;
	struct i386_Descriptor *gdt;
	
	//---------------------------------------------------------------
	//o s�recin LDT ve TSS b�lgeleri i�in gerekli GDT selekt�r indexi
	//hesaplan�yor
	// 0->NULL desc.
	// 1->KernelCS desc.
	// 2->KernelDS desc.
	// 3->UserCS desc.
	// 4->UserDS desc.
	//	--------	
	// 5-> TSS0 |-- 0.s�re� i�in TSS 
	//  --------
	// 6-> TSS1 |-- 1.s�re� i�in TSS 
	//  --------
	// |
	// |
	// V
	//----------------------------------------------------------------
	
	GDT_index=task->ID + 5;
	gdt=(struct i386_Descriptor *)phys_to_virt((unsigned long)Global_Descriptor_Table);
	//---------------------------------------------------------------
	//s�recin TSS'sine ait GDT descriptorunu doldur
	Descriptor_Doldur(&gdt[GDT_index],
					  0x68,	//104 byte=TSS uzunlugu
					  (unsigned long)&task->Tss,
					  PRESENT | TSS,
					  AVAILABLE);
	//---------------------------------------------------------------
}

//-----------------------------------------------------------------------
// Fonksiyon Ad�:
//   "fillTSS"
// A��klama:
//	S�recin gerekli durum bilgilerinin sakland��� TSS yap�s�n� doldur
// Parametreler :
//  task ->	s�re� yap�s�
// Geri D�n�� De�eri:
//   YOK
//-----------------------------------------------------------------------
void fill_TSS(struct Task *task)
{
	//---------------------------------------------------------------
	//segment selekt�rleri GDT de�erlerini g�stersin
	task->Tss.CS=sel_UserCS;
	task->Tss.DS=sel_UserDS;
	task->Tss.ES=sel_UserDS;
	task->Tss.FS=sel_UserDS;
	task->Tss.GS=sel_UserDS;
	task->Tss.SS=sel_UserDS;
	//---------------------------------------------------------------

	//---------------------------------------------------------------
	//y���t segmenti kullan�c� y���t segmentini g�stersin 
	task->Tss.ESP=(unsigned long)allocUserPages(1,task->addr_space->user_ptable_1)+4095; 
	//---------------------------------------------------------------

	//---------------------------------------------------------------
	//SS0 ve ESP0 kernel y���t segmentini g�stersin 
	task->Tss.SS0=sel_KernelDS;
	task->Tss.ESP0=(unsigned long)allocKernelPages(1)+4095;
	//---------------------------------------------------------------

	//---------------------------------------------------------------
	//s�recin t�m yazma�lar�n� ilk ba�ta s�f�rla	
	task->Tss.EAX=0;
	task->Tss.EBX=0;
	task->Tss.ECX=0;
	task->Tss.EDX=0;
	task->Tss.ESI=0;
	task->Tss.EDI=0;
	task->Tss.EBP=0;
	//---------------------------------------------------------------

	//---------------------------------------------------------------
	//flag de�eri yaz�l�yor
	task->Tss.EFlags=EFLG_IF | EFLG_IOPL3; //interrupt enable ve IOPL
	task->Tss.EIP=0;
	task->Tss.IO_Bitmap_Base_Adress=0;
	task->Tss.LDT_selector=0;
	//---------------------------------------------------------------
}

//-----------------------------------------------------------------------
// Fonksiyon Ad�:
//   "createPageTables"
// A��klama:
//	verilen s�recin sayfa tablolar�na yer ay�r�r, onlar� doldurur.
// Parametreler :
//  task ->	s�re� yap�s�
// Geri D�n�� De�eri:
//   YOK
//-----------------------------------------------------------------------
void createPageTables(struct Task *task)
{
	void *physical;
	void *virt;
	int i;

	//---------------------------------------------------------------
	//s�rece ait sayfa dizin tablosu ve sayfa tablolar�  doldurulmal�.
	//Bu nedenle s�rece ait sayfa dizin tablosu ve sayfa tablolar� i�in
	//gerekli yer, yine i�letim sistemi taraf�ndan atanmal�
	allocPages(3,&physical);
	mapPages(KernelPageTable_High,3,PTE_PRESENT|PTE_WRITE,
			 VIRT_OFFSET,physical,&virt);	
	task->addr_space=(struct address_space *)virt;
	//---------------------------------------------------------------
	
	//---------------------------------------------------------------
	// sayfa tablolar� ilkleniyor...
	for(i=0;i<1024;i++)
	{
		task->addr_space->pdir[i]=0;
		task->addr_space->user_ptable_0[i]=i*4096|PTE_PRESENT|PTE_WRITE|PTE_USER;
		task->addr_space->user_ptable_1[i]=0;
	}

	// sayfa dizininin ilk eleman�, s�rece ait ilk sayfa tablosunu g�ste-
	// riyor.
	task->addr_space->pdir[0]= (unsigned long)(physical+4096)|PDE_PRESENT|PDE_WRITE|PDE_USER;
	task->addr_space->pdir[1]= (unsigned long)(physical+4096*2)|PDE_PRESENT|PDE_WRITE|PDE_USER;
	// ikinci eleman ise kernel sayfa tablosunu g�steriyor(fiziksel adres)
	task->addr_space->pdir[2]= (unsigned long)KernelPageTable_High|PDE_PRESENT|PDE_WRITE;
	//---------------------------------------------------------------

	//---------------------------------------------------------------
	//s�recin sayfa tablosunu g�ster
	task->Tss.CR3=(unsigned long)physical;
	//---------------------------------------------------------------
}

//-----------------------------------------------------------------------
// Fonksiyon Ad�:
//   "Create_Process"
// A��klama:
//	Sistemde s�re� olu�turmak i�in kullan�lan temel fonksiyondur.
// Parametreler :
//  task ->	yarat�lacak s�re� i�in yap�
// Geri D�n�� De�eri:
//   YOK
//-----------------------------------------------------------------------
struct Task *createProcess()
{
	struct Task *yeni_surec;
	
	//---------------------------------------------------------------
	//S�rece ait bilgilerin i�letim sistemi taraf�ndan tutulmas� i�in
	//gerekli fiziksel bellek ay�rma i�lemleri...
	//bo� bir sayfa al
	//s�rece ait bilgiler bu sayfa i�erisinde tutulacak
	yeni_surec=(struct Task *)allocKernelPages(1);
	//---------------------------------------------------------------

	//---------------------------------------------------------------
	//s�rece ait sayfa tablolar�n� doldur
	createPageTables(yeni_surec);
	//---------------------------------------------------------------

	//---------------------------------------------------------------
	//s�re�in ID'sini s�re� listesindeki eleman say�s� olarak atan�yor
	yeni_surec->ID=Surec_Sayisi;
	//---------------------------------------------------------------

	//---------------------------------------------------------------
	//s�rece ait sayfa durum bilgilerini doldur
	fill_TSS(yeni_surec);
	//---------------------------------------------------------------

	//---------------------------------------------------------------
	//o s�recin LDT ve TSS b�lgeleri i�in gerekli GDT girdilerini
	//yarat
	fill_GDT(yeni_surec);
	//---------------------------------------------------------------

	//---------------------------------------------------------------
	//s�re� �al��t�r�lmaya haz�r...
	yeni_surec->Durum=TASK_READY;
	//---------------------------------------------------------------

	//sistemdeki s�re� say�s�n� 1 artt�r
	Surec_Sayisi++;

	return yeni_surec;
}

//-----------------------------------------------------------------------
// Fonksiyon Ad�:
//   "Exec"
// A��klama:
//	Sistemde s�re� olu�turmak i�in kullan�lan temel fonksiyondur.
// Parametreler :
//  code_segment_base -> s�re�in kod ba�lang�� adresi(fiziksel)
// Geri D�n�� De�eri:
//   YOK
//-----------------------------------------------------------------------
void Exec(unsigned long code_segment_base)
{
	struct Task *yeni_surec;		//olu�turulacak s�re� i�in yap�

	//---------------------------------------------------------------
	//yeni bir s�re� olu�tur
	yeni_surec=(struct Task *)createProcess();
	//---------------------------------------------------------------

	//---------------------------------------------------------------
	//s�re� yap�s� dolduruluyor...
	yeni_surec->code_segment_base=code_segment_base;
	yeni_surec->Tss.EIP=code_segment_base;
	//---------------------------------------------------------------

	//---------------------------------------------------------------
	Insert_Task((struct Liste *)phys_to_virt((unsigned long)&Hazir_Surec_Listesi),
				yeni_surec);
	//s�re�, s�re� kuyru�una yerle�tirildi...�al��t�r�lmaya haz�r...
	//---------------------------------------------------------------
}

//-----------------------------------------------------------------------
// Fonksiyon Ad�:
//   "deleteProcess"
// A��klama:
//	Sistemde bulunan bir s�re�i silmek i�in kullan�l�r
// Parametreler :
//  task -> silinecek s�re�e i�aret�i
// Geri D�n�� De�eri:
//   YOK
//-----------------------------------------------------------------------
void deleteProcess(struct Task *task)
{
	struct address_space *addr_spc;

	//---------------------------------------------------------------
	//s�re�,o an zaten aktif s�re� listesinden ��kart�lm��t�r ve �a-
	//l��an s�re�tir.dolay�s�yla, sadece s�rece ait bellek b�lgeleri-
	//ni geri vermemiz, yeterli olacaktir.
	//---------------------------------------------------------------

	addr_spc=task->addr_space;

	//---------------------------------------------------------------
	// s�rece ait kullan�c� y���t� sisteme veriliyor
	freePages(addr_spc,1,(void *)task->Tss.ESP);
	//---------------------------------------------------------------

	//---------------------------------------------------------------
	// s�rece ait kernel y���t� sisteme veriliyor
	freePages(addr_spc,1,(void *)task->Tss.ESP0);
	//---------------------------------------------------------------

	//---------------------------------------------------------------
	// s�rece ait s�re� yap�s� sisteme veriliyor
	freePages(addr_spc,1,(void *)task);
	//---------------------------------------------------------------

	//---------------------------------------------------------------
	// s�rece ait sayfa tablolar� i�in ay�r�lm�� b�lgeler de tekrar 
	// sisteme veriliyor.
	freePages(addr_spc,3,(void *)addr_spc);
	//---------------------------------------------------------------

}

//-----------------------------------------------------------------------
// Fonksiyon Ad�:
//   "Exit"
// A��klama:
//	Sistemde bulunan bir s�re�,bu �a��r�m sayesinde �al��mas�n� sonland�r�r.
// Parametreler :
//  YOK
// Geri D�n�� De�eri:
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
	// sureci bitmi� s�re�ler listesine koy
	terminated_task->Durum=TASK_TERMINATED;
	Insert_Task((struct Liste *)phys_to_virt((unsigned long)&Bitmis_Surec_Listesi),
				terminated_task);
	//---------------------------------------------------------------

	//---------------------------------------------------------------
	// Kesmeleri a�
	sti();
	//---------------------------------------------------------------
	
	//---------------------------------------------------------------
	// i�lemci b�l��t�r�c�s�n� �a��r ki o s�reci �al��t�rmay� kessin
	Scheduler();
	//---------------------------------------------------------------
}

//-----------------------------------------------------------------------
// Fonksiyon Ad�:
//   "surecBul"
// A��klama:
//	 ID'si ve liste verilen s�reci geri d�nd�r�r
// Parametreler :
//   ID -> s�recin ID'si
//   lst-> aranacak liste
// Geri D�n�� De�eri:
//   bulunan task
//-----------------------------------------------------------------------
struct Task *surecBul(struct Liste *lst,unsigned long ID)
{
	unsigned long bulundu;
	struct Task *task;

	bulundu=0;

	//---------------------------------------------------------------
	// eleman� listede ara
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
	// e�er bulunduysa listeden ��kart ve d�nd�r
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
// Fonksiyon Ad�:
//   "endProcess"
// A��klama:
//	 verilen s�reci bitmis s�re�ler listesine koyar
// Parametreler :
//   task -> sonlanacak s�re�
// Geri D�n�� De�eri:
//   YOK
//-----------------------------------------------------------------------
void endTask(struct Task *task)
{
	cli();
	//---------------------------------------------------------------
	// sureci bitmi� s�re�ler listesine koy
	task->Durum=TASK_TERMINATED;
	Insert_Task((struct Liste *)phys_to_virt((unsigned long)&Bitmis_Surec_Listesi),
				task);
	//---------------------------------------------------------------
	sti();

}
//-----------------------------------------------------------------------
// Fonksiyon Ad�:
//   "killProcess"
// A��klama:
//	 ID'si verilen s�reci �ld�r�r.
// Parametreler :
//   ID -> s�recin ID'si
// Geri D�n�� De�eri:
//   YOK
//-----------------------------------------------------------------------
void killProcess(unsigned long taskID)
{
	struct Liste *lst;
	struct Task *task;

	Set_Color(5);
	//---------------------------------------------------------------
	// e�er shell veya init �ld�r�lmek istenirse buna izin verme
	if((taskID==0)||(taskID==1))
	{
		Println("<kill> init veya shell oldurulemez!");
		return ;
	}
	//---------------------------------------------------------------

	//---------------------------------------------------------------
	// �al��maya hazir s�re�lerin tutuldu�u listeye ait sanal adres
	//al�n�yor
	lst=(struct Liste *)phys_to_virt((unsigned long)&Hazir_Surec_Listesi);
	task=surecBul(lst,taskID);
	if(task!=NULL)
	{
		endTask(task);
		return;
	}
	//---------------------------------------------------------------

	//---------------------------------------------------------------
	// bekleyen s�re�lerin tutuldu�u listeye ait sanal adres
	//al�n�yor
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
