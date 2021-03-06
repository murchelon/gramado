:SetUp
@echo off

    rem
	rem Objetivo:
    rem Esse código cria um VHD com MBR e código de boot no começo do disco,
	rem e também os elementos básicos do sistema de arquivos fat16, como
	rem vbr, fat, root ...
    rem
	rem nasm 2.05.01
	rem imdisk 2.0.9
	rem
    rem SET BuildFlags=-h
    rem if "%BuildFlags%" == "-h" goto HowItWorks
    rem if "%BuildFlags%" == "-?" goto Usage	
    rem ...
	
:readNotification:	
echo -{ ...
echo -{ Warning: This script will install files on Gramado 0.4.
echo -{ Warning: You need to run this script as administrator.
	pause

    rem
	rem Ordem de montagem:
	rem stage1, stage2, msm, bl, kernel, browser, fat16 stuff and vhd footer.
	rem

echo -{ ...
echo -{ Size={31MB}
echo -{ Volume letter F:
echo -{ FAT16 File system. 
echo -{ Partition starts on sector 63.

:attachingVHD
	rem Montando um disco já formatado.
	imdisk -a -f c:\gramado\GRAMADO.VHD -s 31M -m F: -b 63b
		
:Copying
echo -{ ...

    rem Gramado Boot
	copy c:\gramado\bin\BM.BIN    F:\
    copy c:\gramado\bin\BL.BIN    F:\
	
	rem Gramado Core
	copy c:\gramado\bin\KERNEL.BIN   F:\
    copy c:\gramado\bin\INIT.BIN     F:\
    copy c:\gramado\bin\SHELL.BIN    F:\
    copy c:\gramado\bin\TASKMAN.BIN  F:\

    rem Gramado Cali

    rem Gramado LA	
	
	rem config stuff
    copy c:\gramado\arch\x86\boot\vhd\INIT.TXT     F:\
    copy c:\gramado\arch\x86\boot\vhd\USER.TXT     F:\
	copy c:\gramado\arch\x86\boot\vhd\GRAMADO.TXT  F:\
	
	REM copy c:\gramado\arch\x86\boot\vhd\FILE2KB.TXT  F:\
	
	rem BMPs
	copy c:\gramado\arch\x86\boot\vhd\images\*.BMP  F:\*.BMP
	rem copy c:\gramado\arch\x86\boot\vhd\images\BMP1.BMP      F:\
	rem copy c:\gramado\arch\x86\boot\vhd\images\MOUSE.BMP     F:\
	rem copy c:\gramado\arch\x86\boot\vhd\images\DENNIS.BMP    F:\
	rem copy c:\gramado\arch\x86\boot\vhd\images\APP.BMP       F:\
	rem copy c:\gramado\arch\x86\boot\vhd\images\FILE.BMP      F:\
	rem copy c:\gramado\arch\x86\boot\vhd\images\FOLDER.BMP    F:\
	rem copy c:\gramado\arch\x86\boot\vhd\images\TERMINAL.BMP  F:\	
	rem copy c:\gramado\arch\x86\boot\vhd\images\CURSOR.BMP    F:\
	
	
	rem
	rem ## TESTS ##
    rem 
	
	rem font
	copy c:\gramado\bin\NC2.FON F:\
	
	
	copy c:\gramado\bin\FCLASS.BIN    F:\
	copy c:\gramado\bin\GRAMCC.BIN    F:\
    copy c:\gramado\bin\JACKPOT.BIN   F:\	
	copy c:\gramado\bin\TASCII.BIN    F:\
	copy c:\gramado\bin\TEDITOR.BIN   F:\	
	copy c:\gramado\bin\TGFE.BIN      F:\

		
	rem arquivos de texto.
	copy c:\gramado\arch\x86\boot\vhd\tests\*.C    F:\*.C 
	copy c:\gramado\arch\x86\boot\vhd\tests\*.TXT  F:\*.TXT
	
	REM copy c:\gramado\arch\x86\boot\vhd\tests\TEST.C F:\
	rem copy tests\TEST2.C  F:\
	rem copy tests\REGRAS.TXT   F:\
	rem copy tests\REGRA1.TXT   F:\
	
	rem scripts
	rem copy tests\TEST1.SH1   F:\
	
	
	rem ...
	
:creatingFolders:

    REM ESSA ROTINA DEVERÁ SER FEITA PELO INSTALADOR.
	REM MAS NO CASO ESSE É O INSTALADOR POR ENQUANTO.

    rem #todo 	
	rem #importante:
	rem O host Windows usa letras maiúsculas para criar os diretórios.
    rem O usuário digita letras minúsculas, mas o Gramado procura pode letras 
	rem maiúsculas.
	rem Esse problema aparecerá em todos os sistemas operacionais que usaram 
	rem o Windows como host.
	rem 
	
    mkdir F:\DEV
    mkdir F:\SECURITY
	mkdir F:\TOOLS
	mkdir F:\USERS
	
	
	REM 
	REM  /DB
	REM 

	REM #IMPORTANTE:
	REM BANCO DE DADOS USADOS PELO SISTEMA PARA
	REM ARMAZENAS ARQUIVOS 'SISTEMICAMENTE IMPORTANTES' E 
	REM ARQUIVOS DE TESTE DE NOVOS RECURSOS.

	mkdir F:\TOOLS\DB	
	copy c:\gramado\arch\x86\boot\vhd\images\*.BMP  F:\TOOLS\DB\*.BMP
 	copy c:\gramado\arch\x86\boot\vhd\*.TXT         F:\TOOLS\DB\*.TXT
	copy c:\gramado\bin\*.BIN                       F:\TOOLS\DB\*.BIN
	copy c:\gramado\bin\*.FON                       F:\TOOLS\DB\*.FON
	rem  ...


	
	REM 
	REM  /BOOT
	REM 
	
	mkdir F:\TOOLS\BOOT
	copy c:\gramado\bin\BM.BIN    F:\TOOLS\BOOT\BM.BIN
	copy c:\gramado\bin\BL.BIN    F:\TOOLS\BOOT\BL.BIN
	
	
    mkdir F:\USERS\DEFAULT
    copy c:\gramado\arch\x86\boot\vhd\USER.TXT    F:\USERS\DEFAULT\USER.TXT	


:detachingVHD
echo -{ ...
	imdisk -D -m F:
	
:vhd	
echo -{ ...
    rem echo -{ copying vhd
	rem copy GRAMADO.VHD c:\gramado\GRAMADO.VHD	
    goto End

:HowItWorks	
    echo -{ ...
	echo -{ How it works ?
	echo -{ + nasm.exe mount the main.asm file creating the SYSTEM.VHD file.
	echo -{ + The main.asm file has a MBR, VBR, FATs, ROOTDIR, DATAAREA and a vhd footer.
	echo -{   So the SYSTEM.VHD is created as a formated disk.
    echo -{ + imdisk.exe attachs the VHD disk, mounting it on the host OS.
	echo -{ + The script moves all the .bin files to the mounted disk.
    echo -{ + imdisk.exe detachs the VHD disk.
    echo -{ + The script ends.
    echo -{ Warning: You need to run this script as administrator.	
    goto End
	
:Usage
    echo -{ ...
	echo -{ Usage:
    echo -{ Warning: This script will create a VHD and install Gramado on it.
    echo -{ Warning: You need to run this script as administrator.
	echo -{ -? flag shows this usage message.
	echo -{ @todo
:End
echo -{ ...
echo -{ End.
	pause


