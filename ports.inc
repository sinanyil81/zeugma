;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
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
; PORTS.Inc dosyas� sisteme ait portlar�n de�erlerini tutan sistem dosyas�d�r. Bu portlar
; yard�m� ile , sistem a��l���nda gerekli olan ilklemeler yap�l�r.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

%ifndef PORTS
%define PORTS
;-------------------------------------------------------------------------------
;PIC programlanmas� i�in gerekli olan port adresleri
;Bu port numaralar� kullan�larak donan�m kesmeleri ta��nacakt�r. Ayr�ca sistemin kesme-
;lere tam olarak yan�t vermesi i�in, PIC programlamas�n�n yap�lmas� ve gereken paramet-
;relerin verilmesi �artt�r.BIOS bunu a��l��ta yapsa da , uyumluluk a��s�ndan tekrar ya-
;p�lmas� daha uygundur.
%define PIC_MASTER_PORT_0 20h    ;ICW1'in g�nderilece�i port numaras� (PIC1)
%define PIC_MASTER_PORT_1 21h    ;ICW2,ICW3 ve ICW4'�n g�nderilece�i port numaras� (PIC1)
%define PIC_SLAVE_PORT_0 0A0h    ;PIC2 i�in ilk port
%define PIC_SLAVE_PORT_1 0A1h    ;PIC2 i�in 2. port
;-------------------------------------------------------------------------------
;PIT programlanmas� i�in gerekli portlar
%define PIT_1_COUNTER_0  40h
%define PIT_1_COUNTER_1  41h
%define PIT_1_COUNTER_2  42h
%define PIT_1_CONTROL_REGISTER 43h
%define PIT_2_COUNTER_0  48h
%define PIT_2_COUNTER_1  49h
%define PIT_2_COUNTER_2  4Ah
%define PIT_2_CONTROL_REGISTER 4Bh
;-------------------------------------------------------------------------------
;8042 Klavye denetleyicisinin programlanabilmesi i�in gerekli olan portlar
;Bu port numaralar� A20 adres baca��n�n aktif hale getirilmesi i�in kullan�lacakt�r.
%define KEYBOARD_DATA_REGISTER  60h
%define KEYBOARD_COMMAND_REGISTER  64h  ;komut yazma
%define KEYBOARD_STATUS_REGISTER  64h   ;durum bilgisi alma

%endif




