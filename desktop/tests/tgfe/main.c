// main.c 
// Arquivo principal do aplicativo teditor.bin
// O aplicativo é usado para testes do sistema operacional 
// Gramado 0.3.
//

#include <types.h>

#include "heap.h"
#include "api.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "status.h"
#include "addrbar.h"


//#define TEDITOR_VERBOSE 1

#define GRID_HORIZONTAL 1000
#define GRID_VERTICAL 2000

//static int running = 1;
int running = 1;


    //
	// ## Janelas de teste ##
	//
	
    struct window_d *gWindow;  //grid 
	struct window_d *mWindow;  //menu

//static char *dest_argv[] = { "-sujo0","-sujo1","-sujo2",NULL };
//static unsigned char *dest_envp[] = { "-sujo", NULL };
//static unsigned char dest_msg[512];

void editorClearScreen(); 

int tgfeProcedure ( struct window_d *window, 
                    int msg, 
					unsigned long long1, 
					unsigned long long2 );

/*
 ************************************************************
 * mainGetMessage:
 *     Função principla chamada pelo crt0.asm.
 *     Testando o recebimento de mensagens enviadas pelo shell.
 *
 */
int mainGetMessage (){
	
	//char buff[512];

	// #importante:
	// Recebendo mensagens via memória compartilhada.
	// Obs: Esse não é o melhor endereço para se usar,
	// mas isso é um teste por enquanto.
	
	// Origem.
    // Provavelmente está dentro do backbuffer na parte não visível.	 
	unsigned char* shared_memory = (unsigned char*) (0xC0800000 -0x100);	
	
#ifdef TEDITOR_VERBOSE	
	printf("\n");
	printf("mainGetMessage:\n");
	printf("Initializing teditor.bin ...\n\n");
#endif	
	
#ifdef TEDITOR_VERBOSE		
	printf("\n");
	printf(".\n");
	printf("..\n");
	printf("# MESSAGE={%s} #\n", shared_memory );
	printf("..\n");
	printf(".\n");
	printf("\n");
#endif
	
#ifdef TEDITOR_VERBOSE		
    //Inicializando o editor propriamente dito.	
	printf("Calling main() ... \n"); 
#endif	
	
	//mainTextEditor( 1, (char*)NULL );
	
	if( mainTextEditor( 1, (char*) shared_memory ) == 1 )
	{
#ifdef TEDITOR_VERBOSE				
		printf("mainGetMessage: mainTextEditor returned 1.\n");
#endif 
	}else{
#ifdef TEDITOR_VERBOSE				
		printf("mainGetMessage: mainTextEditor returned 0.\n");
#endif
	};
	
    printf("*HANG\n");
hang:
    asm("pause");
    goto hang;
};


/*
 ********************************************
 * mainTextEditor:
 *     O editor de textos.
 * In this fuction:
 *     Initializes crt.
 *     Initializes stdio.
 *
 */
int mainTextEditor ( int argc, char *argv[] ){
	
	int ch;
	FILE *fp;
    int char_count = 0;	
	
	struct window_d *hWindow;
	
	
#ifdef TEDITOR_VERBOSE			
	printf("\n");
	printf("Initializing File explorer:\n");
	printf("mainTextEditor: # argv={%s} # \n", &argv[0] );
#endif	
	
	//
	// ## vamos repetir o que dá certo ...
	//
	
	//vamos passar mais coisas via registrador.
	
	//ok
	//foi passado ao crt0 via registrador
	//printf("argc={%d}\n", argc ); 
	
	//foi passado ao crt0 via memória compartilhada.
	//printf("argvAddress={%x}\n", &argv[0] ); //endereço.
	
	
	//unsigned char* buf = (unsigned char*) (0x401000 - 0x100) ;
	//printf("argvString={%s}\n" ,  &argv[0] );
	//printf("argvString={%s}\n" , &buf[0] );
	
	//printf("argv={%s}\n", &argv[2] );
	
	
    //stdlib
	//inicializando o suporte a alocação dinâmica de memória.
	libcInitRT();

	//stdio
	//inicializando o suporte ao fluxo padrão.
    stdioInitialize();	
	

    //
	// ## app window ##
	//
	
	apiBeginPaint(); 
	hWindow = (void *) APICreateWindow ( WT_OVERLAPPED, 1, 1, "File Explorer",
	                    10, 10, 700, 250,    
                        0, 0, COLOR_WHITESMOKE, 0x303030 );	  

	if ( (void *) hWindow == NULL )
	{	
		printf("TGFE.BIN: hWindow fail");
		apiEndPaint();
		goto fail;
	}
    APIRegisterWindow (hWindow);
    APISetActiveWindow (hWindow);	
    
	//#importante: Focus.
	APISetFocus (hWindow);	
	
	refresh_screen ();	
	apiEndPaint();
	
	

	
	
	//printf("Nothing for now! \n");
    //goto done;

	//apiBeginPaint();    
	//editorClearScreen(); 
	//topbarInitializeTopBar();
	//statusInitializeStatusBar();
	//update_statuts_bar("# Status bar string 1","# Status bar string 2");
	//apiEndPaint();
	
	//
	//  ## Testing file support. ##
	//
	
//file:

#ifdef TEDITOR_VERBOSE		
	//printf("\n");
	//printf("\n");
   // printf("Loading file ...\n");
#endif	
	
	//#atenção
	//30KB ?? não é muito ??
	void *b = (void *) malloc (1024*30); 	 
    
	if ( (void *) b == NULL )
	{
		printf("allocation fail\n");
		goto done;
		//while(1){}
	}

	//printf("Loading icon...\n");
	
    // @todo: 
	// Usar alguma rotina da API específica para carregar arquivo.
	// na verdade tem que fazer essas rotinas na API.
	
	system_call ( SYSTEMCALL_READ_FILE, (unsigned long) "FOLDER  BMP", 
		(unsigned long) b, (unsigned long) b );	
	
	//
    // ## testes ##
    //
 

	
	//
	// Grid.
	//
	
	apiBeginPaint(); 
	gWindow = (void *) APICreateWindow ( WT_SIMPLE, 1, 1, "GRID-WINDOW",
	                    200, 450, 320, 50,    
                        hWindow, 0, 0x303030, 0x303030 );	  

	if ( (void *) gWindow == NULL )
	{	
		printf("TGFE.BIN: gWindow fail");
		apiEndPaint();
		goto fail;
	}
    APIRegisterWindow (gWindow);
	refresh_screen();	
    apiEndPaint();
	
	//
	// Menu.
	//
	
	apiBeginPaint(); 
	mWindow = (void *) APICreateWindow ( WT_SIMPLE, 1, 1, "MENU-WINDOW",
	                    50, 450, 100, 80,    
                        hWindow, 0, 0x303030, 0x303030 );	  

	if ( (void *) mWindow == NULL )
	{	
		printf("TGFE.BIN: mWindow fail");
		apiEndPaint();
		goto fail;
	}
    APIRegisterWindow (mWindow);
	refresh_screen ();	
    apiEndPaint();
	
	
	 
	//#isso funcionou ... 
	// mas estão se sobrepondo.
	//fazer apenas um deles.
    //#debug 
	//grid.
    //printf("Creating  grid \n");	 
	
	//#obs: Acho que isso cria grid.
	int s = (int) system_call ( 148, (unsigned long) gWindow, 4, 
	                (unsigned long) GRID_HORIZONTAL );
	 
	if (s == 1)
    {
		printf("148 fail.\n");
	}	

    //menu.
	//criando menu de teste.
	//#obs: Acho que isso cria menu.
    system_call ( 149, (unsigned long) mWindow, (unsigned long) mWindow, 
	    (unsigned long) mWindow );	
	 
	//#debug 
    //printf("## test done ##\n");	 
	
	
	/*
	int i;
	for( i=0; i<8; i++ )
	{
		// #isso é um teste.
		// Criando janelas para os ícones, mas deveria 
		// criar grid ou menu.
		// #bugbug: Não temos acesso aos elementos da estrutura 
		// da janela, pois estão em ring0.
	    apiBeginPaint(); 
	    hWindow = (void*) APICreateWindow( WT_SIMPLE, 1, 1,"ICON-WINDOW",
	                    20, 1+20+(i*24), 
						800-40, 24,    
                        0, 0, COLOR_BLUE, COLOR_BLUE );	  

	    if((void*) hWindow == NULL)
	    {	
		    printf("WINDOW-FAIL");
		    apiEndPaint();
		    goto fail;
	    }
        apiEndPaint();		
		
		
		
	    //Usando a API para exibir o bmp carregado. 
	    //ACHO QUE ISSO SOMENTE PINTA NO BACKBUFFER
	    apiDisplayBMP( (char *) b, 40, 1+20+(i*24) ); 
    };
	
	*/
	
	//MOSTRA
	
	refresh_screen();
    //goto done;
	
/*
    //fp = fopen("test1.txt","rb");	
	//fp = fopen( (char*) &argv[0],"rb");	
	
	//folder icon.
	fp = fopen ("folder.bmp", "rb" );
	
	if ( fp == NULL )
    {
        printf("fopen fail\n");
        goto fail;		
    
	} else {
		
		//
		// mostrar o icone.
		//
		
#ifdef TEDITOR_VERBOSE			
        //printf(".\n");		
       // printf("..\n");		
       // printf("...\n");
#endif 
		// Exibe o arquivo.
       // printf("%s",fp->_base);	

#ifdef TEDITOR_VERBOSE	        
		//printf("...\n");
       // printf("..\n");		
       // printf(".\n");		
#endif


#ifdef TEDITOR_VERBOSE	
		//printf("\n");
		//printf("Typing a text ...\n");
#endif


		
		//saiu.
        printf(".\n");		
        printf(".\n");		
        printf(".\n");

	};
	
*/	
	
    unsigned long message_buffer[5];	

Mainloop:		
	while (running)
	{
		enterCriticalSection(); 
		system_call ( 111,
		    (unsigned long)&message_buffer[0],
			(unsigned long)&message_buffer[0],
			(unsigned long)&message_buffer[0] );
		exitCriticalSection(); 
			
		if (	message_buffer[1] != 0 )
		{
	        tgfeProcedure ( (struct window_d *) message_buffer[0], 
		        (int) message_buffer[1], 
		        (unsigned long) message_buffer[2], 
		        (unsigned long) message_buffer[3] );
			
			message_buffer[0] = 0;
            message_buffer[1] = 0;
            message_buffer[3] = 0;
            message_buffer[4] = 0;	
        };				
	};	
	
	
	
fail:	
    printf("fail.\n");
done:
    running = 0;
   // printf("#debug *hang\n");
   // printf("done.\n");
	while(1)
	{
		asm("pause");
		//exit(0);
	};
    // Never reach this.	
	return (int) 0;
};


/*
 *  
 *     Limpar a tela 
 */

void editorClearScreen()
{
	int lin, col;    

	// @todo:
	//system( "cls" ); // calls the cls command.
	
	//cursor.
	apiSetCursor( 0, 0 );
	
	//
	// Tamanho da tela. 80x25
	//
	
	//linhas.
	for( lin=0; lin < ((600/8)-1); lin++)
	{
		col = 0;
		
		apiSetCursor(col,lin);
		
		//colunas.
		for( col=0; col < ((800/8)-1); col++)
		{
			printf("%c",' ');
	    }
	};
	
	apiSetCursor(0,2);
};


int tgfeProcedure ( struct window_d *window, 
                    int msg, 
					unsigned long long1, 
					unsigned long long2 )
{
	switch (msg)
	{
		
		case MSG_SYSKEYDOWN:
		    switch (long1)
			{
		        
				case VK_F1:
						
					break;
					
				case VK_F2:
 
					break;
					
				case VK_F3:
 
					break;
					
				//...
				
                //full screen
                //colocar em full screen somente a área de cliente. 				
		        case VK_F11:
				    
					break;
					
				//...

			};
			break;		
		
		// MSG_MOUSEKEYDOWN	
		case 30:
		    switch (long1)
			{
				//botão 1.
				case 1:
				    if ( window == gWindow )
					{
						printf("grid window\n");
					}
				    if ( window == mWindow )
					{
						printf("menu window\n");
					}

					break;
			};
			break;
		
		default:
		    break;
	};
	
    return (int) 0;	
};

