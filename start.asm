;ORG 0h
%include "GDT.inc"        ; GDT tan�mlay�c�lar�na ait offset bilgileri tutan dosya
%include "Descriptor.inc"
;--------------------------------------------------------------------------------------------
;
;     zzzzzzzzzz
;	    zzz	eeee  u  u gggg mmmmm  aaa
;	  zzz   eeee  u  u g  g m m m a  a	T�RK�E ��LET�M S�STEM�
;	zzz	e     u  u gggg m m m a  a	       ver 0.1
;     zzz	eeee  uuuu    g m m m  aaaa
;    zzzzzzzzzzzzzzzzzzzzzzzzzgzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz
;                             g
;                          gggg
;
; Art�k korumal� moda ge�mi�, i�letim sistemi �ekirde�ini haf�zaya y�klemi� durumday�z. �u
; an kernel kodu �al��makta.
; Saatler s�ren hata ay�klama i�leminden sonra en sonunda �ekirde�e ve korumal� moda atlaya-
; bildik. Art�k i�letim sistemi kodu C ile yaz�lmaya ba�lanabilir.
;
; |  Bu a�amada ortaya ��kan haf�za haritas� �u �ekildedir:
; |  0 - ffffh -> Kernel'e ayr�lm��t�r...
; |  10000h - A0000h -> Bo� bellek
; |  A0000h - C0000h -> Video       ---> Hi�bir s�rece atanamaz.Kernelin kontrol� alt�nda...
; V  C0000h - fffffh -> Rom system  ---> Rom kodunun bulundu�u bellek b�lgesi...
;--------------------------------------------------------------------------------------------
[section .text]
[BITS 32]

EXTERN       Zeugma_Main                  ;Main.c i�erisinde tan�mlanm��t�r
GLOBAL       Global_Descriptor_Table      ;GDT tablosu
GLOBAL       Interrupt_Descriptor_Table   ;IDT tablosu
GLOBAL       Kernel_Stack                 ;Kernel y���t b�lgesi(kernel i�lemleri i�in)
GLOBAL       Page_Directory_Table         ;Sistemdeki ana Sayfa Dizin Tablosu
;--------------------------------------------------------------------------------------------
;A�a��daki kodlar t�pk� Linux'te oldu�u gibi, sayfa tablolar� ile kaplanacakt�r.A�a��daki
;kodlar i�letildikten sonra, sayfalama i�lemi i�in gerekli olan tablolardan Sayfa Dizin
;Tablosu (PageDirectory) , bu kodlar�n �zerine yaz�lacakt�r. Sayfa Dizin Tablosu 4MB'l�k
;bellek b�lgelerini se�mek i�in, Sayfa Tablolar� 4MB'l�k bellek b�lgeleri i�inden 4KB'l�k
; bellek b�lgeleri se�mek i�in kullan�l�r.
;
;Sayfa Dizin Tablosu i�in buradaki b�lge uygun g�r�lm��t�r.C koduna ge�ildikten sonra bu
;tabloya 2 adet girdi eklenecektir. Bu sayede 8MB'l�k bellek b�lgesi adreslenecektir.
;Bu sistemin ilklenmesi ve ilk i�lemler i�in yeterlidir.
;
align 4096
Page_Directory_Table:
             ;--------------------------------------------------------------------
             ;Ba�lang��ta segment yazma�lar�n� uygun de�erlerle y�kle...
             mov AX,10h            ;4GB data
             mov DS,AX
             mov ES,AX
             mov FS,AX
             mov GS,AX
             ;--------------------------------------------------------------------
             ;T�m sistem ya�ant�s� boyunca kullan�lacak GDT tablosunu olu�tur. IDTR ve
             ;GDTR yazma�lar�na uygun de�erleri y�kle
             Call GDT_Olustur
             lgdt [GDT_Limit]
             lidt [IDT_Limit]
             ;--------------------------------------------------------------------
             ;T�m tablolar y�klendikten sonra art�k di�er segment yazma�lar�na uygun
             ;de�erleri yeniden y�kle
             mov AX,sel_KernelDS
             mov DS,AX
             mov ES,AX
             mov FS,AX
             mov GS,AX
             mov SS,AX
             ;--------------------------------------------------------------------
             ;A20 kap�s�n�n aktif hale getirilip getirilmedi�ini kontrol et...
             ;Bu i�lemde 0h fiziksel adresindeki veri 1 artt�r�l�p 100000h fiziksel
             ;adresine yaz�l�yor. Sonra 0h fiziksel adresindeki veri tekrar okunup
             ;kontrol ediliyor. E�er o an okunan de�er ile 1 artt�r�lm�� de�er ayn� ise
             ;A20 kap�s� aktif hale getirilememi�tir...
             mov EAX,[0h];        ;0h fiziksel adresindeki bilgiyi al
             inc EAX              ;ve o bilgiyi 1 artt�r
             mov [100000h],EAX    ;artt�rd���m�z de�eri 100000h fiziksel adresine yaz
             mov EBX,[0h]
             cmp EAX,EBX          ;e�er 2 bilgi ayn� ise A20 kap�s� aktif de�ildir...
             je $                 ;kullan�c� reset tu�una bas�ncaya kadar bekler:)
             ;--------------------------------------------------------------------
             ;Y���t kernel stack'i g�stersin...
             mov EAX,Kernel_Stack
             add EAX,4096                      ;y���t�m�z Kernel y���t�n� g�steriyor.
             mov ESP,EAX
             ;--------------------------------------------------------------------
             ;Sayfalama mekanizmas�n� aktiflemeden �nce, Sayfa Dizin Yazma��n� ,
             ;Sayfa Dizinini g�sterecek �ekilde ayarla
             mov EAX,Page_Directory_Table
             mov CR3,EAX           ;cr3=Page Directory
             ;--------------------------------------------------------------------
             ;��letim sistemini ilkleyecek ve s�re�leri ba�latacak olan fonksiyonu �a��r
             xor EAX,EAX           ;EAX=0
             push EAX              ;Kernel_Main'in parametreleri
             push EAX
             push EAX
             call Zeugma_Main
             ;--------------------------------------------------------------------
             ;Art�k C dilini kullanarak yazm�� oldu�umuz fonksiyonlara atlad�k.
             ;Art�k alt sat�ra kesinlikle atlayamay�z.(En az�ndan �yle umuyoruz :))


;-----------------------------------------------------------------------
; Fonksiyon Ad�:
;   "GDT_Olustur"
; A��klama:
;   Fonksiyon, korumal� modda haf�zaya eri�mek i�in kullan�lacak olan selekt�rlerin
;   g�sterdi�i descriptor'lar�n Global Descriptor Table 'a doldurulmas�n� sa�lar.
; Parametreler :
;   YOK
; Geri D�n�� De�eri:
;   YOK
;-----------------------------------------------------------------------
GDT_Olustur:
            ;--------------------------------------------------------------------
            ;null tan�mlay�c�s� dolduruluyor.
            mov EDI,sel_Null
            xor EBX,EBX
            xor ECX,ECX
            xor DX,DX
            call Descriptor_Doldur
            ;--------------------------------------------------------------------
            ;KernelCS tan�mlay�c�s� dolduruluyor. Bu tan�mlay�c�, kernelin t�m bellek
            ;b�lgesine eri�imini sa�lar.
            ;4GB, Read,Write,
            mov EDI,sel_KernelCS
            mov DL,PRESENT + CODE_EXEC
            mov DH,PAGE_GRANULARITY + SEGMENT_32_BIT + AVAILABLE
            xor EBX,EBX
            mov ECX,0fffffh
            call Descriptor_Doldur
            ;--------------------------------------------------------------------
            ;KernelDS tan�mlay�c�s� dolduruluyor. Bu tan�mlay�c�, kernelin t�m bellek
            ;b�lgesine eri�imini sa�lar.
            ;4GB, Read,Write,
            mov EDI,sel_KernelDS
            mov DL,PRESENT + DATA_READWRITE
            mov DH,PAGE_GRANULARITY + SEGMENT_32_BIT + AVAILABLE
            xor EBX,EBX
            mov ECX,0fffffh
            call Descriptor_Doldur
            ;--------------------------------------------------------------------
            ;UserCS tan�mlay�c�s� dolduruluyor. Bu tan�mlay�c�,kullan�c�n�n t�m lineer
            ;bellek b�lgesine eri�imini sa�lar.
            ;4GB, Read,Write,
            mov EDI,sel_UserCS
            mov DL,PRESENT + CODE_EXEC + DPL3
            mov DH,PAGE_GRANULARITY + SEGMENT_32_BIT + AVAILABLE
            xor EBX,EBX
            mov ECX,0fffffh
            call Descriptor_Doldur
            ;--------------------------------------------------------------------
            ;UserDS tan�mlay�c�s� dolduruluyor. Bu tan�mlay�c�,kullan�c�n�n t�m lineer
            ;bellek b�lgesine eri�imini sa�lar.
            ;4GB, Read,Write,
            mov EDI,sel_UserDS
            mov DL,PRESENT + DATA_READWRITE  + DPL3
            mov DH,PAGE_GRANULARITY + SEGMENT_32_BIT + AVAILABLE
            xor EBX,EBX
            mov ECX,0fffffh
            call Descriptor_Doldur

            ret

;-----------------------------------------------------------------------
; Fonksiyon Ad�:
;   "Descriptor_Doldur"
; A��klama:
;   Fonksiyon, korumal� modda gerekli olan descriptorlar�n doldurulmas� i�lemini yapar.
; Parametreler :
;   EDI -> descriptor baslangicini gosterir
;   EBX -> Base
;   ECX -> limit
;   DL -> Eri�im haklar�
;   DH -> Granularity ve b�y�kl�k ile ilgili bilgiler
; Geri D�n�� De�eri:
;   YOK
;-----------------------------------------------------------------------
Descriptor_Doldur:
              add EDI,Global_Descriptor_Table   ;GDT'deki konum hesaplan�yor
              cld
              ;--------------------------------------------------------------------
              ;limit 15-0 '� yaz
              mov AX,CX
              stosw
              shr ECX,16    ;CL -> limit(16-19)
              ;--------------------------------------------------------------------
              ;base 15-0 '� ve 23-16'y� yaz
              mov AX,BX
              stosw          ; BX -> base (0-15)
              shr EBX,16
              mov AL,BL
              stosb          ; BL -> base (16-23)
              ;--------------------------------------------------------------------
              ;eri�im haklar�n� yaz
              mov AL,DL
              stosb
              ;--------------------------------------------------------------------
              ;Limit 16-19 ve Granularity bilgilerini yaz
              or DH,CL
              mov AL,DH
              stosb
              ;--------------------------------------------------------------------
              ;Base 31-24 yaz�l�yor
              mov AL,BH   ;BH -> base (24-31)
              stosb

              ret
;-----------------------------------------------------------------------------------
;NASM'�n align makrosu sayesinde, a�a��daki kod par�alar� di�er bir sayfada yer ala-
;cakt�r. Bu i�lem sayesinde Sayfa Dizin Tablosu 1 sayfal�k yer kaplayacak �ekilde 
;ayarlanm�� olacakt�r.
align         4096
;-----------------------------------------------------------------------------------
;KERNEL Veri Alan�
;Sistem Global Descriptor tablosu
GDT_Limit                              DW 256*8-1
GDT_Base                               DD Global_Descriptor_Table
Global_Descriptor_Table Times 256*8    DB 0
;Sistem Interrupt Descriptor tablosu
IDT_Limit                              DW 256*8-1
IDT_Base                               DD Interrupt_Descriptor_Table
Interrupt_Descriptor_Table Times 256*8 DB 0
;Y���t sadece Kernel ilkleme i�lemleri i�in kullan�l�yor.Bu nedenle 1 sayfal�k bellek
;yeterlidir. (4096 byte)
Kernel_Stack resb 4096
;-----------------------------------------------------------------------------------








