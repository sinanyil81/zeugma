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
//  Sayfalama ve s�re�ler i�in bellek ay�rma i�lemlerinin yap�ld���
// mod�ld�r.
/////////////////////////////////////////////////////////////////////////////
#include "Memory.h"
#include "AsmDefines.h"
#include "Console.h"

//---------------------------------------------------------------
// sistemdeki fiziksel bellek par�alar�n�n takibinin yap�labilmesi i-
// �in gereken dizi.�lk 16 sayfa Kernel kodu i�in ayr�lm��t�r.buradan sonraki
// 2 sayfa ise 2 adet sayfa tablomuza ayr�lacakt�r.
static char FrameMap[NUM_FRAMES]={0,};
//---------------------------------------------------------------

//---------------------------------------------------------------
// Sistemdeki kernel sayfa dizin tablosu (Start.asm'den)
// Sayfa dizin tablosunun fiziksel adresi = 0x000000
extern unsigned long Page_Directory_Table[1024];
//---------------------------------------------------------------

//---------------------------------------------------------------
// 12 MB'l�k bellek b�lgesine eri�im i�in 3 adet sayfa tablosu 
// ayarlanm��t�r.
unsigned long *KernelPageTable_Low_0; 
unsigned long *KernelPageTable_Low_1;
unsigned long *KernelPageTable_High;  
//---------------------------------------------------------------

//-----------------------------------------------------------------------
// Fonksiyon Ad�:
//   "Init_Paging"
// A��klama:
//	Sayfalama i�leminin yap�labilmesi i�in gerekli olan tablolar� dol-
// durur, sayfala mekanizmas�n� �al��t�r�r.
// Parametreler :
//   YOK
// Geri D�n�� De�eri:
//   YOK
//-----------------------------------------------------------------------
void initPaging()
{
	int i;

	//---------------------------------------------------------------
	// Sistemdeki sayfa tablolar�n�n fiziksel adresleri atan�yor. 
	// 12 MB'l�k bellek adreslenebilecek...
	KernelPageTable_Low_0  =(unsigned long *)0x00010000;
	KernelPageTable_Low_1  =(unsigned long *)0x00011000;
	KernelPageTable_High   =(unsigned long *)0x00012000;
	//---------------------------------------------------------------

	//---------------------------------------------------------------
	// Kernel Page Directory bo�alt�l�yor
	// Sayfa tablolar� bire bir fiziksel adresler ile dolduruluyor
	for(i=0;i<1024;i++)
	{
		Page_Directory_Table[i]=0;
		KernelPageTable_Low_0[i]=i*4096|PTE_PRESENT|PTE_WRITE|PTE_USER;
		KernelPageTable_Low_1[i]=0;
		if((i<19) ||(i==0xb8))
			KernelPageTable_High[i]=i*4096|PTE_PRESENT|PTE_WRITE;
		else
			KernelPageTable_High[i]=0;
	}
	//---------------------------------------------------------------

	//---------------------------------------------------------------
	//Kernel Sayfa dizin tablosuna gerekli girdiler konuluyor...
	Page_Directory_Table[0]=(unsigned long)KernelPageTable_Low_0|PDE_PRESENT|PDE_WRITE;
	Page_Directory_Table[1]=(unsigned long)KernelPageTable_Low_1|PDE_PRESENT|PDE_WRITE;
	Page_Directory_Table[2]=(unsigned long)KernelPageTable_High|PDE_PRESENT|PDE_WRITE;
	//---------------------------------------------------------------

	//---------------------------------------------------------------
	//Kernel frame'leri ve PT frameleri dolu olarak i�aretleniyor
	for(i=0;i<19;i++)
		FrameMap[i]=1;

	FrameMap[0xb8]=1; //video bellek b�lgesi
	//---------------------------------------------------------------

	//---------------------------------------------------------------
	//Art�k 12MB'l�k bellek b�lgesine eri�im i�in gerekli sayfalama tablolar� olu�-
	//turuldu.

	//Sayfalama mekanizmas�n� aktif hale getir.
	//Start_Paging();
	__asm__ ("mov %0, %%cr3"::"r" (Page_Directory_Table));
	__asm__ ("mov %%cr0,%%eax ;"
			 "orl $0x80000000,%%eax ;"
			 "mov %%eax,%%cr0 ;"
			 "jmp 1f ;"
			 "1: movl $1f,%%eax ;"
			 " jmp *%%eax ;"
			 "1:  ":: );
	
	//---------------------------------------------------------------	
	Println("<Zeugma> Hafiza sistemi ilklendi.");
	Println("<Zeugma> Sayfalama mekanizmasi aktif halde.");
}

//-----------------------------------------------------------------------
// Fonksiyon Ad�:
//   "allocPages"
// A��klama:
//	��letim sisteminden parametre olarak verilen kadar sayfa al�nmas� 
//  i�lemini yapar.Sayfan�n fiziksel ba�lang�� adresini d�nd�r�r.
// Parametreler :
//   num_pages -> ka� adet sayfa al�nacak
//   physical_address -> d�nen sayfalar�n fiziksel ba�lang�� adresi
// Geri D�n�� De�eri:
//   YOK
//-----------------------------------------------------------------------
unsigned long allocPages(unsigned long num_pages,
						 void **physical_address)
{
	unsigned long i, j;

	//---------------------------------------------------------------
	// verilen sayfa say�s� kadar ard���k sayfay�, haf�za sistemi i�e-
	// risinde bul
	for(i = 0; i < (NUM_FRAMES - num_pages); i++)
	{
		//---------------------------------------------------------------
		//ard���k sayfalar�n da bo� olup olmad���n� kontrol et
		for(j = 0; j < num_pages; j++)
		{
			//e�er sayfa dolu ise, art�k aramaya o sayfadan itibaren 
			//ba�la
			if(FrameMap[i + j]==1)
			{
				i += j;
				break;
			}
		}
		//---------------------------------------------------------------

		//---------------------------------------------------------------
		// e�er istenen sayfa kadar ard���k sayfa var ise
		if(j == num_pages)
		{
			//sayfalar� dolu olarak i�aretle
			for(j = 0; j < num_pages; j++)
			{
				FrameMap[i + j]=1;
			}
			
			//fiziksel adresi ata
			*physical_address = (void *) (i << 12); 
			
			return STATUS_SUCCESS;
		}
		//---------------------------------------------------------------
	}
	//---------------------------------------------------------------

	Println("<allocPages>: Haf�za doldu!!!");
	return STATUS_OUT_OF_MEMORY;
}

//-----------------------------------------------------------------------
// Fonksiyon Ad�:
//   "mapPages"
// A��klama:
//	verilen sayfa tablosuna, verilen fiziksel adres kadar sayfa ekle
// Parametreler :
//   page_table -> girdilerin eklenece�i sayfa tablosu
//   num_pages -> ka� adet girdi var
//	 attributes -> girdi �zellikleri
//   offset -> sanal adrese eklenecek ofset
//   physical_address -> eklenecek fiziksel adres girdisi
//   virtual_address -> eklenen girdilere g�re d�nen sanal adres
// Geri D�n�� De�eri:
//   YOK
//-----------------------------------------------------------------------
unsigned long mapPages(unsigned long *page_table,
					   unsigned long num_pages,
					   unsigned long attributes,
					   unsigned long offset,
					   void *physical_address,
					   void **virtual_address)
{

	unsigned long i,j;

	//---------------------------------------------------------------
	// o sayfa tablosu i�inde sayfa say�s� kadar,ard���k bo� girdi bul
	for(i = 0; i < (1024 - num_pages); i++)
	{
		for(j = 0; j < num_pages; j++)
		{
			//e�er dolu ise d�ng�den ��k
			if(page_table[i + j])
			{
				i += j;
				break;
			}
		}

		if(j == num_pages)
		{
			//---------------------------------------------------------------
			//e�er bulunduysa, sayfa tablosuna kopyala 
			for(j = 0; j < num_pages; j++)
			{
				page_table[i + j] = (((unsigned long)physical_address) + (j<<12)) | attributes;
			}
			//---------------------------------------------------------------

			//---------------------------------------------------------------
			// hesaplanan sanal adres, art�k o fiziksel adresin, o sayfa tab-
			// losuna kar��l�k gelen sanal adrestir.
			*virtual_address = (void *) (offset + (i<<12));
			//---------------------------------------------------------------

			return STATUS_SUCCESS;
		}
	}
	//---------------------------------------------------------------

	return STATUS_OUT_OF_VIRTUAL_MEM;

}

//-----------------------------------------------------------------------
// Fonksiyon Ad�:
//   "allocateUserPages"
// A��klama:
//	 sistemden bo� sayfa al�n�r ve verilen sayfa tablosuna girdi olarak
//   eklenir.
// Parametreler :
//   num_pages -> ka� adet girdi eklenecek
//   page_table -> girdilerin eklenece�i sayfa tablosu
// Geri D�n�� De�eri:
//   YOK
//-----------------------------------------------------------------------
void *allocUserPages(unsigned long num_pages,
					unsigned long *page_table)
{
	void *physical_address;
	void *virtual_address;

	allocPages(num_pages,&physical_address);
	mapPages(page_table,
			 num_pages,
			 PTE_PRESENT|PTE_WRITE|PTE_USER,
			 0x400000,
			 physical_address,
			 &virtual_address);

	return virtual_address;		 
}

//-----------------------------------------------------------------------
// Fonksiyon Ad�:
//   "allocateKernelPages"
// A��klama:
//	sistemden 
// Parametreler :
//   num_pages -> ka� adet girdi eklenecek
// Geri D�n�� De�eri:
//   YOK
//-----------------------------------------------------------------------
void *allocKernelPages(unsigned long num_pages)
{
	void *physical_address;
	void *virtual_address;

	allocPages(num_pages,&physical_address);
	mapPages((unsigned long *)phys_to_virt((unsigned long)KernelPageTable_High),
			 num_pages,
			 PTE_PRESENT|PTE_WRITE,
			 0x800000,
			 physical_address,
			 &virtual_address);

	return virtual_address;		 
}

//-----------------------------------------------------------------------
// Fonksiyon Ad�:
//   "freePages"
// A��klama:
//	��letim sisteminden daha �nce alinmi� bir sayfay�, tekrar sisteme
//	geri vermek i�in kullan�l�r. Bu i�lemde s�recin sayfa tablolar�nda,
//  kernel sayfa tablolar�nda ve sistemde frame takibi yapan listede
//  de�i�iklikler meydana gelebilir.
// Parametreler :
//   page_table ->hangi sayfa dizin tablosundaki o adrese ait girdi si-
//				  linecek
//   num_pages -> ard���k ka� sayfa silinecek
//   address   -> sisteme verilecek adres ba�lang�c� 
// Geri D�n�� De�eri:
//   YOK
//-----------------------------------------------------------------------
void freePages(struct address_space *addr_spc,
			   unsigned long num_pages,
			   void *address)
{
	unsigned long index,i,j;
	void *physical;
	unsigned long *table;

	//---------------------------------------------------------------
	// hangi sayfa tablosundan o eleman�n ��kart�laca�� ve o elemana
	// kar��l�k gelen fiziksel adres bulunuyor.

	//e�er kernel adres sahas� i�inden bir adres sisteme verilecek ise
	if((unsigned long)address >= 0x800000)
	{
		index=(virt_to_phys((unsigned long)address) >>PTE_SHIFT) & PTE_MASK;
		table=(unsigned long *)phys_to_virt((unsigned long)KernelPageTable_High);
	}
	//e�er kullan�c� adres sahas�ndan bir sayfa ��kart�lacak ise 
	else
	{
		if((unsigned long)address>=0x400000)
		{
			index=(((unsigned long)address-0x400000)>>PTE_SHIFT) & PTE_MASK;
			table=addr_spc->user_ptable_1;
		}
		else
		{
			index=((unsigned long)address>>PTE_SHIFT) & PTE_MASK;
			table=addr_spc->user_ptable_0;
		}

	}
	//---------------------------------------------------------------

	//---------------------------------------------------------------
	// o sanal adrese kar��l�k gelen fiziksel adresi, sayfa tablosunu
	// kullanarak bul
	physical=(void *)(table[index] & 0xFFFFF000);
	//---------------------------------------------------------------

	//---------------------------------------------------------------
	// sayfa tablosunda, o elemana ait girdiyi 0'la
	for(i=0;i<num_pages;i++)
	{
		table[index+i]=0;
	}
	//---------------------------------------------------------------

	//---------------------------------------------------------------
	//frame takibini yapan listeden de o sayfay� ��kart.
	j=(unsigned long)physical >> 12;

	for(i=0;i<num_pages;i++)
	{
		// o sayfa artik bo�..
		FrameMap[j+i]=0;
	}
	//---------------------------------------------------------------
}
//-----------------------------------------------------------------------
// Fonksiyon Ad�:
//   "getPageDirectoryEntry"
// A��klama:
//	Verilen sayfa dizin tablosundan,verilen adrese ili�kin sayfa tablo-
//	sunun fiziksel adresini d�nd�r�r.
// Parametreler :
//   page_directory   -> sayfa dizin tablosu
//   virtual_address  -> sanal adres
// Geri D�n�� De�eri:
//   YOK
//-----------------------------------------------------------------------
unsigned long getPageDirectoryEntry(unsigned long *page_directory,
							        unsigned long virtual_address)
{	
	unsigned long page_table_address;
	
	//---------------------------------------------------------------
	//sayfa dizin tablosunadaki, o adrese kar��l�k gelen eleman� d�nd�r
	virtual_address=(virtual_address>>PDE_SHIFT) & PDE_MASK;
	page_table_address = page_directory[virtual_address] & PDE_PT_BASE;
	//---------------------------------------------------------------

	//---------------------------------------------------------------
	return page_table_address;
	//---------------------------------------------------------------
}

//-----------------------------------------------------------------------
// Fonksiyon Ad�:
//   "Get_Page_Table_Entry"
// A��klama:
//	Verilen sayfa dizin tablosundan,verilen adrese ili�kin sayfa tablo-
//	sundaki fiziksel adres de�erini d�nd�r�r.
// Parametreler :
//   page_directory   -> sayfa dizin tablosu
//   virtual_address  -> sanal adres
// Geri D�n�� De�eri:
//   YOK
//-----------------------------------------------------------------------
unsigned long getPageTableEntry(unsigned long *page_directory,
								unsigned long virtual_address)
{
	unsigned long *page_table;	

	//---------------------------------------------------------------
	//o adrese ait sayfa dizin tablosu girdisini al
	page_table=(unsigned long *)getPageDirectoryEntry(page_directory,
													  virtual_address);
	page_table=(unsigned long *)phys_to_virt((unsigned long)page_table);
	//---------------------------------------------------------------

	virtual_address=(virtual_address>>PTE_SHIFT) & PTE_MASK;
	//---------------------------------------------------------------
	// sayfa dizin tablosu girdisindeki de�eri d�nd�r.
	return (page_table[virtual_address] & PTE_P_BASE);
	//---------------------------------------------------------------
}

