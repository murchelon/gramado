;;
;; Gramado MBR - Esse é o MBR que é montado no VHD na hora de sua criação.
;; Será o primeiro setor do disco.
;; Essa rotina carrega o BM.BIN na memória e passa o comando para ele. Além de
;; passar argumentos.
;; (c) Copyright 2017 - Fred Nora.
;;
;; É um VHD de 32MB. A primeira partição é a partição do sistema e está formatada
;; em fat16 com 512 bytes por cluster.
;;
;; Obs: Não mudar os endereços de segmento e offset usados para não arrumar problemas.
;;

END_OF_CLUSTER EQU 0xFFFF
;;END_OF_CLUSTER EQU 0xFFF8

;datasector      EQU (BUFFER_NAME)
;cluster         EQU (BUFFER_NAME+2)
;SAVE3           EQU (BUFFER_NAME+4)
;CylinderNumbers EQU (BUFFER_VOLUME_ID)
rootdirFirstSector EQU (BUFFER_VOLUME_ID)
rootdirSize        EQU (BUFFER_VOLUME_ID+2)

;; 16bit. Esse é o MBR do VHD.
[bits 16]
    jmp START
	;jmp short START ;@todo:
    ;nop


;BPB Personalizado para o VHD de 32MB usado pelo Gramado.
BUFFER_NAME           db "MSDOS5.0"    ;GRAMADO.
BPB:
BytesPerSector        dw 0x0200
SectorsPerCluster     db 1
ReservedSectors       dw 62 
TotalFATs             db 0x02
MaxRootEntries        dw 0x0200
TotalSectorsSmall     dw 0x0000
MediaDescriptor       db 0xF8
SectorsPerFAT         dw 246    
SectorsPerTrack       dw 0x11    ;; 17 SPT.    
NumHeads              dw 4       ;; 4 HEADS.
HiddenSectors         dd 0
TotalSectorsLarge     dd 0
DriveNumber           db 0x80
Flags                 db 0x00
Signature             db 0x29
BUFFER_VOLUME_ID      dd 0x980E63F5
VolumeLabel           db "GRAMADO VHD"
SystemID              db "FAT16   "


;;
;;
;;

	
;;
;;=================================
;;	

DAPSizeOfPacket db 10h
DAPReserved		db 00h
DAPTransfer		dw 0001h
DAPBuffer		dd 00000000h
DAPStart		dq 0000000000000000h

datasector 	dw 0x0000    ;Início da área de dados.
cluster     dw 0x0000


;absoluteSector db 0x00
;absoluteHead   db 0x00
;absoluteTrack  db 0x00

;CylinderNumbers:  dd 0  ;;dword	

;;
;; Mensagens.
;;

ImageName   db "BM      BIN", 0x0D, 0x0A, 0x00
;msgProgress db "*", 0x00               ;; Progresso.
;msgCRLF     db 0x0D, 0x0A, 0x00        ;; Espaçamento. 
;msgFAT		db "F", 0x0D, 0x0A, 0x00   ;; Loading FAT.
;msgImg		db "I", 0x0D, 0x0A, 0x00   ;; Loading Image.
;msgFail		db "r",0x00                ;; read failure.
msgFailure  db "R", 0x00               ;; ROOT failure.
;...

;Real Start.
START:
    ;nop   
;Step1: 	
    ; Code located at 0x7C00, adjust segment registers to 0x07C0:0.
    cli
	mov  ax, 0x07C0
    mov  ds, ax
    mov  es, ax
    ; Create stack.   0:6000h
    mov  ax, 0x0000
    mov  ss, ax
    mov  sp, 0x6000
    sti

Step2:
    mov byte [DriveNumber], byte dl 
	
	;cmp dl, byte 0x80
	;jne FAILURE
	
	;;Clear the Screen.
Step3:   
	mov ax, 02h
    int 010h
	
;Step4:	
	;@todo: Certificar que int 13h é suportada.	
	
;Step5:	
    ;@todo: Reset driver.	
	
	
	;;
	;; ** As informações sobre disco serão pegadas no BM.BIN **
	;;
	;;
	;;
	;;
	
;Step6:	
    ;; O STEP6 FOI RETIRADO E AGORA ESTÁ NO PARA O BM.BIN
	;; DESSE MODO NÃO PRECISAMOS MAIS PASSAR ARGUMENTOS PARA O BM.BIN 
	;; PASSAREMOS SOMENTE O 'DRIVE NUMBER'
	
	
;Step7:
    ;; Carregamentos...
	
	;;
	;; Carregar o ROOT.
	;; @todo: Cuidadosamente rever os cálculos feitos aqui para permitir que
	;; que carreguemos o sistema usando discos de vários tamanhos. Por 
	;; enquanto estamos predeterminando as diretrizes de carregamento. 
	;;


LOAD_ROOT:

    ;@todo: encontrar a área de dados e o root dir.
    ; Calcula o início da área de dados.
    ; Compute size of root directory and store in "cx".
    
	xor  cx, cx
    xor  dx, dx
	
	;;somando algumas coisas na ordem que elas aparecem.
	add  dx, WORD [ReservedSectors] ;;62
	add  dx, WORD [HiddenSectors]   ;;0 
    add  dx, WORD 5	                ;(MBR)+(VBR)+(RESEREVADOS DEPOIS DO VBR) 
	;MOV DX, 67                     ;SIMULANDO
	;;SOMA=67
	
    mov ax, 32                     ; 32 byte. Tamanho de uma entrada de diretório.
	mul WORD [MaxRootEntries]      ; 512. Tamanho total do diretório dado em bytes. (32*512) bytes. (BPB)
	div WORD [BytesPerSector]      ; ((32*512)/512) O número total de bytes no diretório, dividido pela quantidade de bytes por setor. (BPB)
	mov cx, ax                     ; Coloca o resultado em cx. (Quantidade de setores no diretório raiz.)
	;xchg  ax, cx                    
	;MOV CX, 32                         ;SIMULANDO 
	;;SOMA=32

	; Compute location(LBA) of root directory and store in "ax".

	XOR AX, AX
	mov  al, BYTE [TotalFATs]                ; 2. Number of FATs. (BPB)
    mul  WORD [SectorsPerFAT]                ; 246. Sectors used by FATs. (BPB)              
	;MOV AX, 492                             ;SIMULANDO
	;;SOMA=492
	
	ADD AX, DX   ; adiciona a soma anterior.  
	
	;;Nessa momento ax tem o setor inicial  do diretório raiz @todo: salvar
	;mov word [rootdirFirstSector], ax
	;;Nessa hora cx contem o tamanho do diretório raiz.
	;mov word [rootdirSize], cx
	
	ADD AX, CX   ; Adiciona a quantidade de setores no diretório raiz.             
    ;;SOMA=(492+67+32)=591  
	
	;;inicio da área de dados
	mov  WORD [datasector], ax               ;; Resultado do cálculo do início da área de dados.
    
	;;@todo: Vamos tentar usar o cãculo acima.
	;; Determinando o valor.
	;;mov  WORD [datasector], 591              ;SIMULANDO

	;;Obs: A variável 'datasector' precisa ser inicializada aqui, pois é usada mais à frente na rotina de conversão.

	;;
	;; Obtivemos com o cálculo:
	;; cx           = Tamanho do diretório raiz, dado em número de setores.
	;; [datasector] = Início da área de dados.
	;;

	;;
    ;; * Carregar o diretório raiz em es:bx 0x07C0:0200.
    ;;

	;;
	;; Obs: Me parece seguro permitirmos que carregue o diretório raiz inteiro,
	;;      Porém desnecessário ainda.
	;;

.loadRoot:
	mov  ax, 559;word [rootdirFirstSector]  ;;559  ;; Determinando o início do diretório raiz.
	mov  cx, 1 ;word [rootdirSize]         ;;1  ;; Determinando o quantos setores carregar apesar de termos calculado o seu tamanho.
    mov  bx, 0x0200                     ;; Determinando o offset do buffer do diretório raiz. 
    call  ReadSectors

.msg:
	;Uma mensagem de espaçamento.
    ;pusha
    ;mov  si, msgCRLF
    ;call  DisplayMessage
    ;popa


    ;;
	;; Procurando o arquivo BM.BIN no diretório raiz.    
	;;
	

.searchFile:	
	; Browse root directory for binary image
    mov  cx, WORD [MaxRootEntries]           ; Load loop counter.
    mov  di, 0x0200                          ; Determinando o offset do início do diretório.
.LOOP:
    push  cx
    mov  cx, 0x000B                          ; Eleven character name.
    mov  si, ImageName                       ; Image name to find.
	pusha
	call  DisplayMessage
	popa
	push  di
	rep  cmpsb                               ; Test for entry match.
    pop  di
    je  LOAD_FAT                             ;; * Se o arquivo foi encontrado.
    pop  cx
    add  di, 0x0020                          ; Queue next directory entry.
    loop  .LOOP
    jmp  FAILURE

	;;
	;; * Carregar o a FAT em es:bx 0x17C0:0200.
	;;

LOAD_FAT:
    ;nop
	
.msg:	
    ;pusha
    ;mov  si, msgFAT
    ;call  DisplayMessage
    ;popa

    ; Save starting cluster of boot image.
	; Salvamos o número do cluster inicial da Imagem, que foi obtido
	; na entrada do diretório.
    mov  dx, WORD [di + 0x001A]
    mov  WORD [cluster], dx                  ; File's first cluster.
	
	
    ; Compute size of FAT and store in "cx".
    mov  cx, WORD [SectorsPerFAT]                ;246 setores por FAT.
	
	
    ; Compute location of FAT and store in "ax".
	; Calculado qual é a LBA inicial da FAT e salvando em 'ax'.
    mov  ax, WORD [HiddenSectors]         	 ; 0 setores escondidos.
	add  ax, WORD [ReservedSectors]          ; 3 setores reservados.

	;;
	;; O que obtivemos com o cálculo:
	;; cx = Qauntidade de setores por fat.
	;; ax = LBA inicial da FAT.
	;;
	
	
    ;;
    ;; * Agora carregaremos a FAT em es:bx, 0x17C0:0200.
    ;;
	
	;Configurando o segmento 'es'. 
	push  ax             ; Salvando LBA inicial da FAT.
	mov  ax, 0x17C0
    mov  es, ax   
	pop  ax              ; recuperando LBA inicial da FAT.
	
	
    ;;
    ;; Efetuando o carregamento da fat no buffer es:bx, 0x17C0:0200.
    ;;	
	
.loadFAT:
	; Read FAT into memory (17C0:0200).
	; Obs: Confiar no cáculo do início da LBA é perigoso, apesar de necessário.
	; Por enquanto vamos determiná-lo.
	; Obs: É possível confiarmos no cálculo do tamanho da fat, mesmo assim, vamos
	; por enquanto, determiná-lo.
	; Obs: Não estamos carregando a FAT inteira. Isso pode ser arriscado, mas 
	; por enquanto, como temos poucos arquivos, vamos carrega apenas metade da FAT.
	mov  ax, 67                                  ;* Determinando a LBA inicial da FAT, apezar de termos calculado.
	mov  cx, (246/2)                             ;  Metade da fat.
	mov  bx, 0x0200                              ;* Determinando o offset da FAT.
    call  ReadSectors	

    ;;
    ;; Carregar o arquivo BM.BIN na memória em es:bx, 0:8000h.
    ;;
	
	
.msgSpace:    
	;Mensagem de espaçamento.
	;mov  si, msgCRLF
    ;call  DisplayMessage
	
	
.msgImage:	
	;Mensagem avisando que vamos carregar.
	;mov  si, msgImg
    ;call  DisplayMessage
    
	
.setupBuffers:	
    ; Configurando o buffer para o arquivo. (es:bx), 0:8000h.
	mov  ax, 0
    mov  es, ax                                  ; Segmento.
    mov  bx, 0x8000                              ; Offset.
    
	; Salvando o offset do buffer do arquivo.
	push  bx
	
	;;
	;; Nesse momento a FAT já está na memória, precisamos indicar 
	;; em gs:bx o endereço do buffer onde carregamos a fat.
	;;
	
	;;Indicando o segmento onde a FAT está carregada.
	mov  ax, 0x17C0							 ; FAT Segment	
    mov  gs, ax
	
	;;
	;; * Carregar o arquivo na memória em es:bx, 0:8000h.
	;;
	
LOAD_IMAGE:

    ;; Em 'bx' está o offset do buffer do arquivo.
    
    ; Recuperando o offset do buffer do arquivo.
    pop  bx                                  
	
	;Vamos converter Cluster em LBA.
	mov  ax, WORD [cluster]              	 ; Cluster inicial do arquivo, obtido na entrada no diretório.
    call  ClusterLBA                         ; Convert cluster to LBA.
    
.loadImage:	
	
	;; Carregamos apenas um cluster de cada vez.
	;; No nosso caso, um cluster só tem um setor.
	xor  cx, cx
    mov  cl, BYTE [SectorsPerCluster]        ; 1. Sectors to read.
    call  ReadSectors
    
.saveThis:	
	;Vamos savar o offset do próximo setor a ser carregado.
	;; Onde deve ficar o próximo setor. ??
	push bx
    
    ;;
    ;;  Agora temos que saber qual será o próximo cluster.
    ;;	
	
    ; Vamos calcular o deslocamento dentro da fat para obtermos o 
	; o local onde encontraremos o valor do próximo cluster.
	; Salvaremos em 'dx'.
	; lembrando que a fat está em gs:bx.
    mov  ax, WORD [cluster]              	 ; identify current cluster
    add  ax, ax								 ; 16 bit(2 byte) FAT entry
    mov  bx, 0x0200                          ; location of FAT in memory
	add  bx, ax                              ; index into FAT    
	mov  dx, WORD [gs:bx]                    ; read two bytes from FAT
	
	;; Em 'dx', está o valor do próximo cluster.
	
.DONE:
    ;Salvamos.
	mov  WORD [cluster], dx              	 ; store new cluster
	 
	;Testamos para ver se é o último cluster. 
.testEOF:    
	cmp  dx, END_OF_CLUSTER  ;0xFFFF                          ; test for end of file
    jne  LOAD_IMAGE
	
	;; Se esse foi o último cluster então prosseguiremos.
	
DONE:
    ;nop
;Step8:
    ;
    ; Preparando os parametros para entrar no BM.BIN. 
	; Parameters: 
	; ==========
	; bx = Magic number.
	; ax = Number of heads.    (*IMPORTANTE) 
	; dl = Drive number.       (*IMPORTANTE)
    ; cl = Sectors per track.  (*IMPORTANTE)
    ; di = cylinder numbers    (*IMPORTANTE) *Exige a rotina de pegar parâmetros do disco.
	; si = offet do ponteiro para o BPB.
    ;....
		
	;Magic.
	;mov bx, word 0xF0ED               ;MAGIC_NUMBER	

    ;Disk parameters.
	;mov ax, word [NumHeads]           ;Heads.
    mov dl, byte [DriveNumber]	      ;*Disk Number.	
	;mov cl, byte [SectorsPerTrack]    ;Sectors Per Track.
	;mov ch, byte 0                   ;Nothing. 
	;mov di, word [CylinderNumbers]    ;Cylinders.
	;BPB.
	;mov si, word BPB                  ;BPB OFFSET. ;ds:si = 0:si 
     
	 ;;
	 ;; @Não há mais espaço no mbr, mas precisamos enviar argumentos....
	 ;; sem alterar o mecanismo de carregamento , temos que captar informações sobre o disco
	 ;;e passar par o BM.BIN na forma de argumentos... mas nada impede que o BM.BIN. faça isso tambem.
	 ;;
	

;Step9:
    ;;Passando o comando para o BM.BIN em 0:8000h.	
.GO:	 
	PUSH  WORD 0 
    PUSH  WORD 0x8000 
    RETF
	





;*************************************************************************
; PROCEDURE ReadSectors
; reads "cx" sectors from disk starting at "ax" into memory location "es:bx"
;*************************************************************************
ReadSectors:
    mov WORD [DAPBuffer]   ,bx
	mov WORD [DAPBuffer+2] ,es
	mov WORD [DAPStart]    ,ax
.MAIN:
    mov  di, 0x0005                             ; Tentativas.
.SECTORLOOP:
    push  ax
    push  bx
    push  cx
	 
    push si
    mov ah, 0x42
    mov dl, 0x80
    mov si, DAPSizeOfPacket
    int 0x13
    pop si
	 
    jnc  .SUCCESS                            ; test for read error
    xor  ax, ax                              ; BIOS reset disk
    int  0x13                                ; invoke BIOS
    
	dec  di                                  ; decrement error counter
    
	pop  cx
    pop  bx
    pop  ax
	
	jnz  .SECTORLOOP                         ; attempt to read again
	int  0x18
	
.SUCCESS:
    ;*Importante: Mensagem de progresso.
    ;mov  si, msgProgress
    ;call  DisplayMessage
    
	pop  cx
    pop  bx
    pop  ax
    
	add  bx, WORD [BytesPerSector]           ; queue next buffer
	cmp	 bx, 0x0000	
	jne	 .NextSector
	
	;Trocando de segmento.
	push  ax
	mov	 ax, es
	add	 ax, 0x1000
    mov  es, ax
	pop  ax
	
.NextSector:
    inc  ax                                     ; queue next sector
	mov WORD [DAPBuffer], bx
	mov WORD [DAPStart], ax
    loop  .MAIN                               ; read next sector
    ret
 
 
;*************************************************************************
; PROCEDURE ClusterLBA
; convert FAT cluster into LBA addressing scheme
; LBA = (cluster - 2) * sectors per cluster
;*************************************************************************
ClusterLBA:
    sub  ax, 0x0002                          ; zero base cluster number
    xor  cx, cx
    mov  cl, BYTE [SectorsPerCluster]        ; convert byte to word
    mul  cx
    add  ax, WORD [datasector]               ; base data sector
	ret
	
	
;*************************************************************************
; PROCEDURE DisplayMessage
; display ASCIIZ string at "ds:si" via BIOS
;*************************************************************************
DisplayMessage:
    lodsb                                    ; Load next character.
    or  al, al                               ; Test for NUL character.
    jz  .DONE
    mov  ah, 0x0E                            ; BIOS teletype.                           
    mov  bx, 0x0007                          ; Página e atributo.  
	int  0x10                                ; Invoke BIOS.
    jmp  DisplayMessage
.DONE:
    ret

	;;*Fail.
FAILURE:
    ;int 0x18  ;; Para economizar espaço.
    mov  si, msgFailure
    call  DisplayMessage
    mov  ah, 0x00
    int  0x16                                ; await keypress
    int  0x19                                ; warm boot computer
	


;Colocando a partition table no lugar certo.(0x1BE).
    TIMES 446-($-$$) DB 0  ;0x1BE
	
;partition 0. 
P0:
.flag:                db  0x80            ;0 ativa.
.hcs_inicial:         db  1, 1, 0      
.os_type:             db  4               ;6;1;0x04 ;4 = FAT16 < 32MB ;0x0E FAT16, (32MB to 2GB), (LBA Support).
.hcs_final:           db  0, 0, 0
.lba_inicial:         dd  0x3F            ; Setor inicial da partição. (63, vbr)
.tamanho_da_particao: dd  (31*1024*1024)  ; quantidade de setores na partição.
;partitions 1, 2 and 3.
P1: dd 0,0,0,0
P2: dd 0,0,0,0
P3: dd 0,0,0,0  

MBR_SIG:    
	TIMES 510-($-$$) DB 0
    DW 0xAA55
;***********************************************************

