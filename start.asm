;ORG 0h
%include "GDT.inc"        ; GDT tanýmlayýcýlarýna ait offset bilgileri tutan dosya
%include "Descriptor.inc"
;--------------------------------------------------------------------------------------------
;
;     zzzzzzzzzz
;	    zzz	eeee  u  u gggg mmmmm  aaa
;	  zzz   eeee  u  u g  g m m m a  a	TÜRKÇE ÝÞLETÝM SÝSTEMÝ
;	zzz	e     u  u gggg m m m a  a	       ver 0.1
;     zzz	eeee  uuuu    g m m m  aaaa
;    zzzzzzzzzzzzzzzzzzzzzzzzzgzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz
;                             g
;                          gggg
;
; Artýk korumalý moda geçmiþ, iþletim sistemi çekirdeðini hafýzaya yüklemiþ durumdayýz. Þu
; an kernel kodu çalýþmakta.
; Saatler süren hata ayýklama iþleminden sonra en sonunda çekirdeðe ve korumalý moda atlaya-
; bildik. Artýk iþletim sistemi kodu C ile yazýlmaya baþlanabilir.
;
; |  Bu aþamada ortaya çýkan hafýza haritasý þu þekildedir:
; |  0 - ffffh -> Kernel'e ayrýlmýþtýr...
; |  10000h - A0000h -> Boþ bellek
; |  A0000h - C0000h -> Video       ---> Hiçbir sürece atanamaz.Kernelin kontrolü altýnda...
; V  C0000h - fffffh -> Rom system  ---> Rom kodunun bulunduðu bellek bölgesi...
;--------------------------------------------------------------------------------------------
[section .text]
[BITS 32]

EXTERN       Zeugma_Main                  ;Main.c içerisinde tanýmlanmýþtýr
GLOBAL       Global_Descriptor_Table      ;GDT tablosu
GLOBAL       Interrupt_Descriptor_Table   ;IDT tablosu
GLOBAL       Kernel_Stack                 ;Kernel yýðýt bölgesi(kernel iþlemleri için)
GLOBAL       Page_Directory_Table         ;Sistemdeki ana Sayfa Dizin Tablosu
;--------------------------------------------------------------------------------------------
;Aþaðýdaki kodlar týpký Linux'te olduðu gibi, sayfa tablolarý ile kaplanacaktýr.Aþaðýdaki
;kodlar iþletildikten sonra, sayfalama iþlemi için gerekli olan tablolardan Sayfa Dizin
;Tablosu (PageDirectory) , bu kodlarýn üzerine yazýlacaktýr. Sayfa Dizin Tablosu 4MB'lýk
;bellek bölgelerini seçmek için, Sayfa Tablolarý 4MB'lýk bellek bölgeleri içinden 4KB'lýk
; bellek bölgeleri seçmek için kullanýlýr.
;
;Sayfa Dizin Tablosu için buradaki bölge uygun görülmüþtür.C koduna geçildikten sonra bu
;tabloya 2 adet girdi eklenecektir. Bu sayede 8MB'lýk bellek bölgesi adreslenecektir.
;Bu sistemin ilklenmesi ve ilk iþlemler için yeterlidir.
;
align 4096
Page_Directory_Table:
             ;--------------------------------------------------------------------
             ;Baþlangýçta segment yazmaçlarýný uygun deðerlerle yükle...
             mov AX,10h            ;4GB data
             mov DS,AX
             mov ES,AX
             mov FS,AX
             mov GS,AX
             ;--------------------------------------------------------------------
             ;Tüm sistem yaþantýsý boyunca kullanýlacak GDT tablosunu oluþtur. IDTR ve
             ;GDTR yazmaçlarýna uygun deðerleri yükle
             Call GDT_Olustur
             lgdt [GDT_Limit]
             lidt [IDT_Limit]
             ;--------------------------------------------------------------------
             ;Tüm tablolar yüklendikten sonra artýk diðer segment yazmaçlarýna uygun
             ;deðerleri yeniden yükle
             mov AX,sel_KernelDS
             mov DS,AX
             mov ES,AX
             mov FS,AX
             mov GS,AX
             mov SS,AX
             ;--------------------------------------------------------------------
             ;A20 kapýsýnýn aktif hale getirilip getirilmediðini kontrol et...
             ;Bu iþlemde 0h fiziksel adresindeki veri 1 arttýrýlýp 100000h fiziksel
             ;adresine yazýlýyor. Sonra 0h fiziksel adresindeki veri tekrar okunup
             ;kontrol ediliyor. Eðer o an okunan deðer ile 1 arttýrýlmýþ deðer ayný ise
             ;A20 kapýsý aktif hale getirilememiþtir...
             mov EAX,[0h];        ;0h fiziksel adresindeki bilgiyi al
             inc EAX              ;ve o bilgiyi 1 arttýr
             mov [100000h],EAX    ;arttýrdýðýmýz deðeri 100000h fiziksel adresine yaz
             mov EBX,[0h]
             cmp EAX,EBX          ;eðer 2 bilgi ayný ise A20 kapýsý aktif deðildir...
             je $                 ;kullanýcý reset tuþuna basýncaya kadar bekler:)
             ;--------------------------------------------------------------------
             ;Yýðýt kernel stack'i göstersin...
             mov EAX,Kernel_Stack
             add EAX,4096                      ;yýðýtýmýz Kernel yýðýtýný gösteriyor.
             mov ESP,EAX
             ;--------------------------------------------------------------------
             ;Sayfalama mekanizmasýný aktiflemeden önce, Sayfa Dizin Yazmaçýný ,
             ;Sayfa Dizinini gösterecek þekilde ayarla
             mov EAX,Page_Directory_Table
             mov CR3,EAX           ;cr3=Page Directory
             ;--------------------------------------------------------------------
             ;Ýþletim sistemini ilkleyecek ve süreçleri baþlatacak olan fonksiyonu çaðýr
             xor EAX,EAX           ;EAX=0
             push EAX              ;Kernel_Main'in parametreleri
             push EAX
             push EAX
             call Zeugma_Main
             ;--------------------------------------------------------------------
             ;Artýk C dilini kullanarak yazmýþ olduðumuz fonksiyonlara atladýk.
             ;Artýk alt satýra kesinlikle atlayamayýz.(En azýndan öyle umuyoruz :))


;-----------------------------------------------------------------------
; Fonksiyon Adý:
;   "GDT_Olustur"
; Açýklama:
;   Fonksiyon, korumalý modda hafýzaya eriþmek için kullanýlacak olan selektörlerin
;   gösterdiði descriptor'larýn Global Descriptor Table 'a doldurulmasýný saðlar.
; Parametreler :
;   YOK
; Geri Dönüþ Deðeri:
;   YOK
;-----------------------------------------------------------------------
GDT_Olustur:
            ;--------------------------------------------------------------------
            ;null tanýmlayýcýsý dolduruluyor.
            mov EDI,sel_Null
            xor EBX,EBX
            xor ECX,ECX
            xor DX,DX
            call Descriptor_Doldur
            ;--------------------------------------------------------------------
            ;KernelCS tanýmlayýcýsý dolduruluyor. Bu tanýmlayýcý, kernelin tüm bellek
            ;bölgesine eriþimini saðlar.
            ;4GB, Read,Write,
            mov EDI,sel_KernelCS
            mov DL,PRESENT + CODE_EXEC
            mov DH,PAGE_GRANULARITY + SEGMENT_32_BIT + AVAILABLE
            xor EBX,EBX
            mov ECX,0fffffh
            call Descriptor_Doldur
            ;--------------------------------------------------------------------
            ;KernelDS tanýmlayýcýsý dolduruluyor. Bu tanýmlayýcý, kernelin tüm bellek
            ;bölgesine eriþimini saðlar.
            ;4GB, Read,Write,
            mov EDI,sel_KernelDS
            mov DL,PRESENT + DATA_READWRITE
            mov DH,PAGE_GRANULARITY + SEGMENT_32_BIT + AVAILABLE
            xor EBX,EBX
            mov ECX,0fffffh
            call Descriptor_Doldur
            ;--------------------------------------------------------------------
            ;UserCS tanýmlayýcýsý dolduruluyor. Bu tanýmlayýcý,kullanýcýnýn tüm lineer
            ;bellek bölgesine eriþimini saðlar.
            ;4GB, Read,Write,
            mov EDI,sel_UserCS
            mov DL,PRESENT + CODE_EXEC + DPL3
            mov DH,PAGE_GRANULARITY + SEGMENT_32_BIT + AVAILABLE
            xor EBX,EBX
            mov ECX,0fffffh
            call Descriptor_Doldur
            ;--------------------------------------------------------------------
            ;UserDS tanýmlayýcýsý dolduruluyor. Bu tanýmlayýcý,kullanýcýnýn tüm lineer
            ;bellek bölgesine eriþimini saðlar.
            ;4GB, Read,Write,
            mov EDI,sel_UserDS
            mov DL,PRESENT + DATA_READWRITE  + DPL3
            mov DH,PAGE_GRANULARITY + SEGMENT_32_BIT + AVAILABLE
            xor EBX,EBX
            mov ECX,0fffffh
            call Descriptor_Doldur

            ret

;-----------------------------------------------------------------------
; Fonksiyon Adý:
;   "Descriptor_Doldur"
; Açýklama:
;   Fonksiyon, korumalý modda gerekli olan descriptorlarýn doldurulmasý iþlemini yapar.
; Parametreler :
;   EDI -> descriptor baslangicini gosterir
;   EBX -> Base
;   ECX -> limit
;   DL -> Eriþim haklarý
;   DH -> Granularity ve büyüklük ile ilgili bilgiler
; Geri Dönüþ Deðeri:
;   YOK
;-----------------------------------------------------------------------
Descriptor_Doldur:
              add EDI,Global_Descriptor_Table   ;GDT'deki konum hesaplanýyor
              cld
              ;--------------------------------------------------------------------
              ;limit 15-0 'ý yaz
              mov AX,CX
              stosw
              shr ECX,16    ;CL -> limit(16-19)
              ;--------------------------------------------------------------------
              ;base 15-0 'ý ve 23-16'yý yaz
              mov AX,BX
              stosw          ; BX -> base (0-15)
              shr EBX,16
              mov AL,BL
              stosb          ; BL -> base (16-23)
              ;--------------------------------------------------------------------
              ;eriþim haklarýný yaz
              mov AL,DL
              stosb
              ;--------------------------------------------------------------------
              ;Limit 16-19 ve Granularity bilgilerini yaz
              or DH,CL
              mov AL,DH
              stosb
              ;--------------------------------------------------------------------
              ;Base 31-24 yazýlýyor
              mov AL,BH   ;BH -> base (24-31)
              stosb

              ret
;-----------------------------------------------------------------------------------
;NASM'ýn align makrosu sayesinde, aþaðýdaki kod parçalarý diðer bir sayfada yer ala-
;caktýr. Bu iþlem sayesinde Sayfa Dizin Tablosu 1 sayfalýk yer kaplayacak þekilde 
;ayarlanmýþ olacaktýr.
align         4096
;-----------------------------------------------------------------------------------
;KERNEL Veri Alaný
;Sistem Global Descriptor tablosu
GDT_Limit                              DW 256*8-1
GDT_Base                               DD Global_Descriptor_Table
Global_Descriptor_Table Times 256*8    DB 0
;Sistem Interrupt Descriptor tablosu
IDT_Limit                              DW 256*8-1
IDT_Base                               DD Interrupt_Descriptor_Table
Interrupt_Descriptor_Table Times 256*8 DB 0
;Yýðýt sadece Kernel ilkleme iþlemleri için kullanýlýyor.Bu nedenle 1 sayfalýk bellek
;yeterlidir. (4096 byte)
Kernel_Stack resb 4096
;-----------------------------------------------------------------------------------








