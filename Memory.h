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
#ifndef MEMORY
#define MEMORY

//---------------------------------------------------------------
#define NUM_FRAMES 3072
#define HIGH_MEM	 0xC00000   //User   8 MB
#define LOW_MEM		 0x400000	//Kernel 1 MB
#define VIRT_OFFSET  0x800000   //virtual olarak kernel 8.MB'tan ba�l�yor
//---------------------------------------------------------------

//---------------------------------------------------------------
// sanal - fiziksel adres d�n���mleri i�in gerekli makrolar 
#define phys_to_virt(addr) (addr + VIRT_OFFSET)
#define virt_to_phys(addr) (addr - VIRT_OFFSET)
//---------------------------------------------------------------

//----------------------------------------------------------------------------
//Haf�za y�netimi sistemi i�in gerekli makro ve sabitler.
#define PAGE_SIZE	4096	//sayfa b�y�kl���
#define PDE_SHIFT	22		//sanal adresten PD'yi almak i�in
#define PDE_MASK	0x3FF	//PD'yi almak i�in gerekli maske
#define PTE_SHIFT	12		//sanal adresten PT'li almak i�in 
#define PTE_MASK    0x3FF	//PT'yi almak i�in gerekli maske
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
//�ntel i�lemcilerinde sayfalama mekanizmas�n� kullanabilmek i�in gere
//ken sabitler

//----------------------------------------------------------------------------
// Sayfa Dizin Tablolarini doldurmak ve maskeleme i�in kullan�lan sabitler
#define PDE_PRESENT			0x00000001
#define PDE_WRITE			0x00000002
#define PDE_USER			0x00000004
#define PDE_WRITE_THROUGH	0x00000008
#define PDE_CACHE_DISABLED	0x00000010
#define PDE_ACCESSED		0x00000020
#define PDE_GLOBAL			0x00000100
#define PDE_PT_BASE			0xFFFFF000
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Sayfa Tablolarini doldurmak ve maskeleme i�in kullan�lan sabitler
#define PTE_PRESENT			0x00000001
#define PTE_WRITE			0x00000002
#define PTE_USER			0x00000004
#define PTE_WRITE_THROUGH	0x00000008
#define PTE_CACHE_DISABLED	0x00000010
#define PTE_ACCESSED		0x00000020
#define PTE_DIRTY			0x00000040
#define PTE_GLOBAL			0x00000100
#define PTE_P_BASE			0xFFFFF000
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
//Haf�za sistemi i�erisinde bulunan fonksiyonlar�n kulland��� sabitler
#define STATUS_SUCCESS 0
#define STATUS_OUT_OF_MEMORY 1
#define STATUS_OUT_OF_VIRTUAL_MEM 2
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// s�re�lerin sayfa ve sayfa dizin tablolar�n�n tutuldu�u yap�
struct address_space
{
	unsigned long pdir[1024];			//sayfa dizin tablosu
	unsigned long user_ptable_0[1024];	//kullan�c� sayfa tablosu
	unsigned long user_ptable_1[1024];	//kullan�c� sayfa tablosu
};
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// fonksiyon prototipleri
void initPaging();
unsigned long allocPages(unsigned long num_pages,
						 void **physical_address);
unsigned long mapPages(unsigned long *page_table,
					   unsigned long num_pages,
					   unsigned long attributes,
					   unsigned long offset,
					   void *physical_address,
					   void **virtual_address);
void *allocUserPages(unsigned long num_pages,
						unsigned long *page_table);
void *allocKernelPages(unsigned long num_pages);
void freePages(struct address_space *addr_spc,
			   unsigned long num_pages,
			   void *address);
unsigned long getPageDirectoryEntry(unsigned long *page_directory,
								    unsigned long virtual_address);
unsigned long getPageTableEntry(unsigned long *page_directory,
							    unsigned long virtual_address);
//----------------------------------------------------------------------------

#endif