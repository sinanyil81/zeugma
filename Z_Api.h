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
//  Sistemdeki API prototiplerini içerir
/////////////////////////////////////////////////////////////////////////////
#ifndef Z_API
#define Z_API

void API_Cls();
void API_Set_Cursor(int koord_x,int koord_y);
void API_Print(char *Karakter_Dizisi);
void API_Println(char *Karakter_Dizisi);
void API_Exec(unsigned long code_segment_base);
void API_Exit();

#endif