:SetUp
@echo off  

:Monting
echo -{ ...
echo -{ Creating application LONGEST1.BIN ...

    cd c:\sdk\bin
	
:Compiling	
echo -{ ...
echo -{ Compiling ...
	gcc  -c  c:\gramado\desktop\tests\longest1\main.c -I c:\gramado\include\libc -o main.o 

    copy c:\gramado\lib\ctype.o   c:\sdk\bin\ctype.o
    copy c:\gramado\lib\stdio.o   c:\sdk\bin\stdio.o
    copy c:\gramado\lib\stdlib.o  c:\sdk\bin\stdlib.o
	copy c:\gramado\lib\string.o  c:\sdk\bin\string.o
    copy c:\gramado\lib\time.o	  c:\sdk\bin\time.o	
 
:Objects	
set myObjects=main.o ^
stdio.o ^
string.o ^
ctype.o  





:Linking  
echo -{ ...
echo -{ Linking objects ... 
   ld -T c:\gramado\desktop\tests\longest1\link.ld -o LONGEST1.BIN %myObjects% -Map c:\gramado\desktop\tests\longest1\map.s
   
   rem Não deletar os objetos.
 
:Moving   

   move LONGEST1.BIN c:\gramado\bin\LONGEST1.BIN
   
:End   
echo End!
	pause