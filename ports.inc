;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
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
; PORTS.Inc dosyasý sisteme ait portlarýn deðerlerini tutan sistem dosyasýdýr. Bu portlar
; yardýmý ile , sistem açýlýþýnda gerekli olan ilklemeler yapýlýr.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

%ifndef PORTS
%define PORTS
;-------------------------------------------------------------------------------
;PIC programlanmasý için gerekli olan port adresleri
;Bu port numaralarý kullanýlarak donaným kesmeleri taþýnacaktýr. Ayrýca sistemin kesme-
;lere tam olarak yanýt vermesi için, PIC programlamasýnýn yapýlmasý ve gereken paramet-
;relerin verilmesi þarttýr.BIOS bunu açýlýþta yapsa da , uyumluluk açýsýndan tekrar ya-
;pýlmasý daha uygundur.
%define PIC_MASTER_PORT_0 20h    ;ICW1'in gönderileceði port numarasý (PIC1)
%define PIC_MASTER_PORT_1 21h    ;ICW2,ICW3 ve ICW4'ün gönderileceði port numarasý (PIC1)
%define PIC_SLAVE_PORT_0 0A0h    ;PIC2 için ilk port
%define PIC_SLAVE_PORT_1 0A1h    ;PIC2 için 2. port
;-------------------------------------------------------------------------------
;PIT programlanmasý için gerekli portlar
%define PIT_1_COUNTER_0  40h
%define PIT_1_COUNTER_1  41h
%define PIT_1_COUNTER_2  42h
%define PIT_1_CONTROL_REGISTER 43h
%define PIT_2_COUNTER_0  48h
%define PIT_2_COUNTER_1  49h
%define PIT_2_COUNTER_2  4Ah
%define PIT_2_CONTROL_REGISTER 4Bh
;-------------------------------------------------------------------------------
;8042 Klavye denetleyicisinin programlanabilmesi için gerekli olan portlar
;Bu port numaralarý A20 adres bacaðýnýn aktif hale getirilmesi için kullanýlacaktýr.
%define KEYBOARD_DATA_REGISTER  60h
%define KEYBOARD_COMMAND_REGISTER  64h  ;komut yazma
%define KEYBOARD_STATUS_REGISTER  64h   ;durum bilgisi alma

%endif




