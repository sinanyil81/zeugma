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
//  Sayfalama ve süreçler için bellek ayýrma iþlemlerinin yapýldýðý
// modüldür.
/////////////////////////////////////////////////////////////////////////////
#include "Memory.h"
#include "AsmDefines.h"
#include "Console.h"

//---------------------------------------------------------------
// sistemdeki fiziksel bellek parçalarýnýn takibinin yapýlabilmesi i-
// çin gereken dizi.Ýlk 16 sayfa Kernel kodu için ayrýlmýþtýr.buradan sonraki
// 2 sayfa ise 2 adet sayfa tablomuza ayrýlacaktýr.
static char FrameMap[NUM_FRAMES]={0,};
//---------------------------------------------------------------

//---------------------------------------------------------------
// Sistemdeki kernel sayfa dizin tablosu (Start.asm'den)
// Sayfa dizin tablosunun fiziksel adresi = 0x000000
extern unsigned long Page_Directory_Table[1024];
//---------------------------------------------------------------

//---------------------------------------------------------------
// 12 MB'lýk bellek bölgesine eriþim için 3 adet sayfa tablosu 
// ayarlanmýþtýr.
unsigned long *KernelPageTable_Low_0; 
unsigned long *KernelPageTable_Low_1;
unsigned long *KernelPageTable_High;  
//---------------------------------------------------------------

//-----------------------------------------------------------------------
// Fonksiyon Adý:
//   "Init_Paging"
// Açýklama:
//	Sayfalama iþleminin yapýlabilmesi için gerekli olan tablolarý dol-
// durur, sayfala mekanizmasýný çalýþtýrýr.
// Parametreler :
//   YOK
// Geri Dönüþ Deðeri:
//   YOK
//-----------------------------------------------------------------------
void initPaging()
{
	int i;

	//---------------------------------------------------------------
	// Sistemdeki sayfa tablolarýnýn fiziksel adresleri atanýyor. 
	// 12 MB'lýk bellek adreslenebilecek...
	KernelPageTable_Low_0  =(unsigned long *)0x00010000;
	KernelPageTable_Low_1  =(unsigned long *)0x00011000;
	KernelPageTable_High   =(unsigned long *)0x00012000;
	//---------------------------------------------------------------

	//---------------------------------------------------------------
	// Kernel Page Directory boþaltýlýyor
	// Sayfa tablolarý bire bir fiziksel adresler ile dolduruluyor
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
	//Kernel frame'leri ve PT frameleri dolu olarak iþaretleniyor
	for(i=0;i<19;i++)
		FrameMap[i]=1;

	FrameMap[0xb8]=1; //video bellek bölgesi
	//---------------------------------------------------------------

	//---------------------------------------------------------------
	//Artýk 12MB'lýk bellek bölgesine eriþim için gerekli sayfalama tablolarý oluþ-
	//turuldu.

	//Sayfalama mekanizmasýný aktif hale getir.
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
// Fonksiyon Adý:
//   "allocPages"
// Açýklama:
//	Ýþletim sisteminden parametre olarak verilen kadar sayfa alýnmasý 
//  iþlemini yapar.Sayfanýn fiziksel baþlangýç adresini döndürür.
// Parametreler :
//   num_pages -> kaç adet sayfa alýnacak
//   physical_address -> dönen sayfalarýn fiziksel baþlangýç adresi
// Geri Dönüþ Deðeri:
//   YOK
//-----------------------------------------------------------------------
unsigned long allocPages(unsigned long num_pages,
						 void **physical_address)
{
	unsigned long i, j;

	//---------------------------------------------------------------
	// verilen sayfa sayýsý kadar ardýþýk sayfayý, hafýza sistemi içe-
	// risinde bul
	for(i = 0; i < (NUM_FRAMES - num_pages); i++)
	{
		//---------------------------------------------------------------
		//ardýþýk sayfalarýn da boþ olup olmadýðýný kontrol et
		for(j = 0; j < num_pages; j++)
		{
			//eðer sayfa dolu ise, artýk aramaya o sayfadan itibaren 
			//baþla
			if(FrameMap[i + j]==1)
			{
				i += j;
				break;
			}
		}
		//---------------------------------------------------------------

		//---------------------------------------------------------------
		// eðer istenen sayfa kadar ardýþýk sayfa var ise
		if(j == num_pages)
		{
			//sayfalarý dolu olarak iþaretle
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

	Println("<allocPages>: Hafýza doldu!!!");
	return STATUS_OUT_OF_MEMORY;
}

//-----------------------------------------------------------------------
// Fonksiyon Adý:
//   "mapPages"
// Açýklama:
//	verilen sayfa tablosuna, verilen fiziksel adres kadar sayfa ekle
// Parametreler :
//   page_table -> girdilerin ekleneceði sayfa tablosu
//   num_pages -> kaç adet girdi var
//	 attributes -> girdi özellikleri
//   offset -> sanal adrese eklenecek ofset
//   physical_address -> eklenecek fiziksel adres girdisi
//   virtual_address -> eklenen girdilere göre dönen sanal adres
// Geri Dönüþ Deðeri:
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
	// o sayfa tablosu içinde sayfa sayýsý kadar,ardýþýk boþ girdi bul
	for(i = 0; i < (1024 - num_pages); i++)
	{
		for(j = 0; j < num_pages; j++)
		{
			//eðer dolu ise döngüden çýk
			if(page_table[i + j])
			{
				i += j;
				break;
			}
		}

		if(j == num_pages)
		{
			//---------------------------------------------------------------
			//eðer bulunduysa, sayfa tablosuna kopyala 
			for(j = 0; j < num_pages; j++)
			{
				page_table[i + j] = (((unsigned long)physical_address) + (j<<12)) | attributes;
			}
			//---------------------------------------------------------------

			//---------------------------------------------------------------
			// hesaplanan sanal adres, artýk o fiziksel adresin, o sayfa tab-
			// losuna karþýlýk gelen sanal adrestir.
			*virtual_address = (void *) (offset + (i<<12));
			//---------------------------------------------------------------

			return STATUS_SUCCESS;
		}
	}
	//---------------------------------------------------------------

	return STATUS_OUT_OF_VIRTUAL_MEM;

}

//-----------------------------------------------------------------------
// Fonksiyon Adý:
//   "allocateUserPages"
// Açýklama:
//	 sistemden boþ sayfa alýnýr ve verilen sayfa tablosuna girdi olarak
//   eklenir.
// Parametreler :
//   num_pages -> kaç adet girdi eklenecek
//   page_table -> girdilerin ekleneceði sayfa tablosu
// Geri Dönüþ Deðeri:
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
// Fonksiyon Adý:
//   "allocateKernelPages"
// Açýklama:
//	sistemden 
// Parametreler :
//   num_pages -> kaç adet girdi eklenecek
// Geri Dönüþ Deðeri:
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
// Fonksiyon Adý:
//   "freePages"
// Açýklama:
//	Ýþletim sisteminden daha önce alinmiþ bir sayfayý, tekrar sisteme
//	geri vermek için kullanýlýr. Bu iþlemde sürecin sayfa tablolarýnda,
//  kernel sayfa tablolarýnda ve sistemde frame takibi yapan listede
//  deðiþiklikler meydana gelebilir.
// Parametreler :
//   page_table ->hangi sayfa dizin tablosundaki o adrese ait girdi si-
//				  linecek
//   num_pages -> ardýþýk kaç sayfa silinecek
//   address   -> sisteme verilecek adres baþlangýcý 
// Geri Dönüþ Deðeri:
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
	// hangi sayfa tablosundan o elemanýn çýkartýlacaðý ve o elemana
	// karþýlýk gelen fiziksel adres bulunuyor.

	//eðer kernel adres sahasý içinden bir adres sisteme verilecek ise
	if((unsigned long)address >= 0x800000)
	{
		index=(virt_to_phys((unsigned long)address) >>PTE_SHIFT) & PTE_MASK;
		table=(unsigned long *)phys_to_virt((unsigned long)KernelPageTable_High);
	}
	//eðer kullanýcý adres sahasýndan bir sayfa çýkartýlacak ise 
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
	// o sanal adrese karþýlýk gelen fiziksel adresi, sayfa tablosunu
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
	//frame takibini yapan listeden de o sayfayý çýkart.
	j=(unsigned long)physical >> 12;

	for(i=0;i<num_pages;i++)
	{
		// o sayfa artik boþ..
		FrameMap[j+i]=0;
	}
	//---------------------------------------------------------------
}
//-----------------------------------------------------------------------
// Fonksiyon Adý:
//   "getPageDirectoryEntry"
// Açýklama:
//	Verilen sayfa dizin tablosundan,verilen adrese iliþkin sayfa tablo-
//	sunun fiziksel adresini döndürür.
// Parametreler :
//   page_directory   -> sayfa dizin tablosu
//   virtual_address  -> sanal adres
// Geri Dönüþ Deðeri:
//   YOK
//-----------------------------------------------------------------------
unsigned long getPageDirectoryEntry(unsigned long *page_directory,
							        unsigned long virtual_address)
{	
	unsigned long page_table_address;
	
	//---------------------------------------------------------------
	//sayfa dizin tablosunadaki, o adrese karþýlýk gelen elemaný döndür
	virtual_address=(virtual_address>>PDE_SHIFT) & PDE_MASK;
	page_table_address = page_directory[virtual_address] & PDE_PT_BASE;
	//---------------------------------------------------------------

	//---------------------------------------------------------------
	return page_table_address;
	//---------------------------------------------------------------
}

//-----------------------------------------------------------------------
// Fonksiyon Adý:
//   "Get_Page_Table_Entry"
// Açýklama:
//	Verilen sayfa dizin tablosundan,verilen adrese iliþkin sayfa tablo-
//	sundaki fiziksel adres deðerini döndürür.
// Parametreler :
//   page_directory   -> sayfa dizin tablosu
//   virtual_address  -> sanal adres
// Geri Dönüþ Deðeri:
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
	// sayfa dizin tablosu girdisindeki deðeri döndür.
	return (page_table[virtual_address] & PTE_P_BASE);
	//---------------------------------------------------------------
}

