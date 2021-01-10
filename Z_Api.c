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
//  Sistemdeki API çaðýrýmlarýný içerir
/////////////////////////////////////////////////////////////////////////////

#define API_CLS_INDEX			 0
#define API_SET_CURSOR_INDEX	 1
#define API_PRINT_INDEX			 2
#define API_PRINTLN_INDEX		 3
#define API_EXEC_INDEX		     4
#define API_EXIT_INDEX			 5
#define API_SET_COLOR_INDEX		 6
#define API_SET_BG_COLOR_INDEX	 7
#define API_SCANF_INDEX			 8
#define API_SUREC_BILGISI_INDEX  9
#define API_KILL_PROCESS_INDEX	 10
#define API_PRINT_SAYI_HEX_INDEX 11

extern unsigned long Zeugma_Api_Table[];

//-----------------------------------------------------------------------
// Fonksiyon Adý:
//   "SystemCall"
// Açýklama:
//   Sistemden gerekli çaðýrýmýn yapýlmasý iþleminden sorumludur.
// Parametreler :
//   API_Index - >Çaðýrýlacak API'nin indexi
//	 param1    - >1. parametre
//	 param2    - >2. parametre
//	 param3    - >3. parametre
//	 param4    - >4. parametre
// Geri Dönüþ Deðeri:
//   YOK
//-----------------------------------------------------------------------
long SystemCall(int API_Index,	//çaðýrýlan fonksiyon indexi
				long param1,	//1. parametre
				long param2,	//2. parametre
				long param3,	//3. parametre
				long param4)	//4. parametre
{
	long ret;
	
	
	//ilgili API'yi parametreleri göndererek çaðýr
	__asm__ __volatile__ ("int $0x47 "
						  :"=a"(ret)
						  :"a"(API_Index),
						   "b"(param1),
						   "c"(param2),
						   "d"(param3),
						   "S"(param4));
	return ret;
}


//----------------------*********API Bölgesi*********--------------------------//
//---------------------------------------------------------------
//Cls
void API_Cls()
{
	SystemCall(API_CLS_INDEX,0,0,0,0);
}
//---------------------------------------------------------------

//---------------------------------------------------------------
//Set Cursor
void API_Set_Cursor(int koord_x,int koord_y)
{
	SystemCall(API_SET_CURSOR_INDEX,koord_x,koord_y,0,0);
}
//---------------------------------------------------------------

//---------------------------------------------------------------
//Print
void API_Print(char *Karakter_Dizisi)
{
	SystemCall(API_PRINT_INDEX,(long)Karakter_Dizisi,0,0,0);
}
//---------------------------------------------------------------

//---------------------------------------------------------------
//Println
void API_Println(char *Karakter_Dizisi)
{
	SystemCall(API_PRINTLN_INDEX,(long)Karakter_Dizisi,0,0,0);
}
//---------------------------------------------------------------

//---------------------------------------------------------------
//Exec
void API_Exec(unsigned long code_segment_base)
{
	SystemCall(API_EXEC_INDEX,code_segment_base,0,0,0);
}
//---------------------------------------------------------------

//---------------------------------------------------------------
//Exit
void API_Exit()
{
	SystemCall(API_EXIT_INDEX,0,0,0,0);
}
//---------------------------------------------------------------

//---------------------------------------------------------------
//Set_Color
void API_Set_Color(char color)
{
	SystemCall(API_SET_COLOR_INDEX,color,0,0,0);	
}
//---------------------------------------------------------------

//---------------------------------------------------------------
//Set_Background_Color
void API_Set_Background_Color(char color)
{
	SystemCall(API_SET_BG_COLOR_INDEX,color,0,0,0);
}
//---------------------------------------------------------------

//---------------------------------------------------------------
// Scanf
void API_Scanf(char *komut)
{
	SystemCall(API_SCANF_INDEX,(unsigned long)komut,0,0,0);
}
//---------------------------------------------------------------

//---------------------------------------------------------------
//surecBilgisi
void API_Surec_Bilgisi()
{
	SystemCall(API_SUREC_BILGISI_INDEX,0,0,0,0);
}
//---------------------------------------------------------------

//---------------------------------------------------------------
//killProcess
unsigned long API_Kill_Process(unsigned long taskID)
{
	SystemCall(API_KILL_PROCESS_INDEX,taskID,0,0,0);
}
//---------------------------------------------------------------

//---------------------------------------------------------------
//Print_Sayi_Hex
void  API_Print_Sayi_Hex(unsigned long sayi)
{
	SystemCall(API_PRINT_SAYI_HEX_INDEX,sayi,0,0,0);
}
//---------------------------------------------------------------