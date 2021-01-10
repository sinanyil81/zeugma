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
//  ��letim sisteminin komut yorumlay�c�s�
/////////////////////////////////////////////////////////////////////////////
#include "Z_Api.h"
#include "Lib.h"


void komutBekle();
void logoCiz();
void surec1();
void surec2();
void surec3();
void surec4();

//-----------------------------------------------------------------------
// Fonksiyon Ad�:
//   "init"
// A��klama:
//   Sistemde ba�lang��ta yarat�lan ve hi�bir suretle sistemden at�lma-
//	yan surec
// Parametreler :
//   YOK
// Geri D�n�� De�eri:
//   YOK
//-----------------------------------------------------------------------
extern void Shell()
{
//	logoCiz();
	API_Println("------------------------------Zeugma Shell-------------------------------");
	API_Set_Color(5);
	API_Println("");
	API_Println("Sistem yardimi icin \"yardim\" yaziniz.");
	//---------------------------------------------------------------
	// Kullan�c�dan komut alma i�lemini yapar
	komutBekle();
	//---------------------------------------------------------------
}

//-----------------------------------------------------------------------
// Fonksiyon Ad�:
//   "logoCiz"
// A��klama:
//   Isletim sistemi logosunu �izer
// Parametreler :
//   YOK
// Geri D�n�� De�eri:
//   YOK
//-----------------------------------------------------------------------
void logoCiz()
{
	//---------------------------------------------------------------
	// i�letim sistemi logosunu �izer
	API_Set_Color(1);

	API_Println("  zzzzzzzzzz");
	API_Println("         zzz eeee  u  u gggg mmmmm  aaa");
	API_Println("       zzz   eeee  u  u g  g m m m a  a     TURKCE ISLETIM SISTEMI");
	API_Println("     zzz	    e     u  u gggg m m m a  a             ver 0.1");
	API_Println("   zzz       eeee  uuuu    g m m m  aaaa");
	API_Println("  zzzzzzzzzzzzzzzzzzzzzzzzzgzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz");
	API_Println("                           g");
	API_Println("                        gggg");
	//---------------------------------------------------------------

}
//-----------------------------------------------------------------------
// Fonksiyon Ad�:
//   "version"
// A��klama:
//   ��letim sistemi versiyon bilgisini yazar
// Parametreler :
//   YOK
// Geri D�n�� De�eri:
//   YOK
//-----------------------------------------------------------------------
void version()
{
	//---------------------------------------------------------------
	// versiyon bilgilerini yaz
	logoCiz();
	API_Println("");
	API_Set_Color(14);
	API_Println("");
	API_Println("                                Kasim Sinan YILDIRIM        ");
	API_Println("                                  Ege Universitesi          ");
	API_Println("                           Bilgisayar Muhendisligi Bolumu   ");
	API_Println("                                        2003                ");
	API_Set_Color(10);
	API_Println("                            ----------------------------    ");
	API_Println("                           | yildirim@bilmuh.ege.edu.tr |   ");
	API_Println("                            ----------------------------    ");
	API_Println("");
	API_Println("                                                            ");
	//---------------------------------------------------------------

}
//-----------------------------------------------------------------------
// Fonksiyon Ad�:
//   "help"
// A��klama:
//   Yard�m komutlarini yazar
// Parametreler :
//   YOK
// Geri D�n�� De�eri:
//   YOK
//-----------------------------------------------------------------------
void help()
{
	//---------------------------------------------------------------
	// yardim men�s�
	API_Set_Color(2);
	API_Println("Zeugma komutlari ------------------------------");
	API_Set_Color(3);
	API_Print("* yardim           ");
	API_Set_Color(5);
	API_Println("--> Komut yardimi");
	API_Set_Color(3);
	API_Print("* bilgi            ");
	API_Set_Color(5);
	API_Println("--> Isletim sistemi bilgisi");
	API_Set_Color(3);
	API_Print("* surec 1          ");
	API_Set_Color(5);
	API_Println("--> 1 numarali sureci cagirir.");
	API_Println("  =>(6 adet 10 satir 10 sutundan olusan matrisleri 150000 defa carpar)");
	API_Set_Color(3);
	API_Print("* surec 2          ");
	API_Set_Color(5);
	API_Println("--> 2 numarali sureci cagirir.");
	API_Println("  =>(bos surectir.islem yapmaz)");
	API_Set_Color(3);
	API_Print("* surec 3          ");
	API_Set_Color(5);
	API_Println("--> 3 numarali sureci cagirir.");
	API_Println("  =>(kullanici girdisi bekler ve onu ekrana yazar)");

	API_Set_Color(3);
	API_Print("* surec 4          ");
	API_Set_Color(5);
	API_Println("--> 4 numarali sureci cagirir.");
	API_Println("  =>(0 ile bolme yazilim kesmesi cagirilir)");

	API_Set_Color(3);
	API_Print("* surec bilgisi    ");
	API_Set_Color(5);
	API_Println("--> Sistemdeki surec listelerine ait bilgi verir.");
	API_Set_Color(3);
	API_Print("* kill XXXXXXXX    ");
	API_Set_Color(5);
	API_Println("--> Sistemdeki ID'si (hex olarak) verilen sureci sonlandirir.");
	API_Set_Color(2);
	API_Println("------------------------------------------------");
	//---------------------------------------------------------------
}
//-----------------------------------------------------------------------
// Fonksiyon Ad�:
//   "komutIsle"
// A��klama:
//   Kullan�c�n�n girdi�i komutu i�ler
// Parametreler :
//   YOK
// Geri D�n�� De�eri:
//   YOK
//-----------------------------------------------------------------------
void komutIsle(char *komut)
{
	int i;
	unsigned long ID;
	unsigned char digit;

	//---------------------------------------------------------------
	// kullan�c�n�n girdi�i komutlari teker teker kontrol et
	if(strcmp(komut,"",strlen(komut)))
		return;
	if(strcmp(komut,"yardim",6))
		help();
	else if(strcmp(komut,"bilgi",5))
		version();
	else if(strcmp(komut,"surec 1",7))
		API_Exec((unsigned long)surec1);
	else if(strcmp(komut,"surec 2",7))
		API_Exec((unsigned long)surec2);
	else if(strcmp(komut,"surec 3",7))
		API_Exec((unsigned long)surec3);
	else if(strcmp(komut,"surec 4",7))
		API_Exec((unsigned long)surec4);
	else if(strcmp(komut,"surec bilgisi",13))
		API_Surec_Bilgisi();
	else if(strcmp(komut,"kill",4))
	{
		ID=0;
		if(strlen(komut)==4)
			return;
		for(i=0;i<strlen(komut)-5;i++)
		{
			digit=komut[i+5];
		
			if((digit>=0x30)&&(digit<=0x39))
				digit=digit-0x30;
			else
				digit=digit - 0x61 + 10 ;

			ID=(ID<<4)+digit;
		}
		API_Kill_Process(ID);		
	}
	else
		API_Println("Bilinmeyen komut...");
	//---------------------------------------------------------------
}
//-----------------------------------------------------------------------
// Fonksiyon Ad�:
//   "komutBekle"
// A��klama:
//   Kullan�c�dan komut bekler
// Parametreler :
//   YOK
// Geri D�n�� De�eri:
//   YOK
//-----------------------------------------------------------------------
void komutBekle()
{
	char Komut_Tamponu[128];

	while(1)
	{
		API_Set_Color(3);
		API_Print("<Z-Shell>");
		//---------------------------------------------------------------
		// komutun girilmesini bekle
		API_Scanf(Komut_Tamponu);
		//---------------------------------------------------------------

		//---------------------------------------------------------------
		// komutu i�le
		komutIsle(Komut_Tamponu);
		//---------------------------------------------------------------
	}
}


//---------------------------------------------------------------
// deneme s�re�leri
void surec1()
{
	unsigned long matris1[10][10];
	unsigned long matris2[10][10];
	unsigned long matris3[10][10];
	unsigned long matris4[10][10];
	unsigned long matris5[10][10];
	unsigned long matris6[10][10];
	unsigned long sonuc[10][10];
	unsigned long toplam=0;
	unsigned long i,j,k;

	API_Set_Color(5);
	API_Print("<surec 1> Calismaya basliyorum...");
	for(k=0;k<1500;k++)
	{
		for(i=0;i<10;i++)
			for(j=0;j<10;j++)
			{
				matris1[i][j]=i;
				matris2[i][j]=i+100;
				matris3[i][j]=i+200;
				matris4[i][j]=i+300;
				matris5[i][j]=i+400;
				matris6[i][j]=i+500;
			}

		for(i=0;i<10;i++)
			for(j=0;j<10;j++)
				sonuc[i][j]=matris1[i][j]*matris2[j][i];

		for(i=0;i<10;i++)
			for(j=0;j<10;j++)
				sonuc[i][j]=matris3[i][j]*sonuc[i][j];

		for(i=0;i<10;i++)
			for(j=0;j<10;j++)
				sonuc[i][j]=matris4[i][j]*sonuc[i][j];

		for(i=0;i<10;i++)
			for(j=0;j<10;j++)
				sonuc[i][j]=matris5[i][j]*sonuc[i][j];

		for(i=0;i<10;i++)
			for(j=0;j<10;j++)
				sonuc[i][j]=matris6[i][j]*sonuc[i][j];

		for(i=0;i<10;i++)
			for(j=0;j<10;j++)
			toplam+=sonuc[i][j];
	}

	API_Set_Color(5);
	API_Print("<surec 1> Matris islemleri bitti ---> toplam : ");
	API_Print_Sayi_Hex(toplam);
	API_Exit();
}
//---------------------------------------------------------------
// bo� s�re�
void surec2()
{
	int i;

	while(1);
}
//---------------------------------------------------------------
//kullan�c�dan tu� al�p ekrana basan s�re�
void surec3()
{
	char deneme[100];

	API_Scanf(deneme);
	API_Set_Color(5);
	API_Print("<surec 3> girdiginiz string--> ");
	API_Print(deneme);

	while(1);
}
//---------------------------------------------------------------
void surec4()
{
	int i;

	i=0/0;
	while(1);
}
//---------------------------------------------------------------
