/*
 * main.c - TASCII.BIN
 *
 * Aplicativo de teste para o sistema operacional Gramado.
 * Exibe os acarteres da tabela ascii.
 * @todo: Pode haver alguma seleção, baseada em argumentos.
 * @todo: usar apenas a libC.
 *
 * 2018 - Fred Nora.
 */
 
 
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define STANDARD_ASCII_MAX 128

/*
 * # main #
 * C function to demonstrate the working of arithmetic operators 
 */

void _main (){
	
    unsigned char count;
	unsigned char standard_ascii_max = STANDARD_ASCII_MAX;
	
	printf("\n");
	printf("ASCII.BIN:\n");
	printf("Show ascii table ...\n");
	printf("\n");
	
    for( count=0; count<standard_ascii_max; count++ )
    {
		printf(" %d - %c",count,count);
        if( count % 4 == 0 ){
            printf("\n");
		}
    };	
	
	printf("done.\n");
	
	//while(1){
	//	asm("pause");
	//}
    
    //return 0;
    exit(0);
};

