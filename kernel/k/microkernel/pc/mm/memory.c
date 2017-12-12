/*
 * File: memory.c
 *
 * Descri��o:
 *     Arquivo principal do m�dulo /mm do /pc, Memory Manager. 
 *     MB - M�dulos inclu�dos no Kernel Base.
 *
 * Atribui��es:
 *     + Alocar Heap do Kernel.
 *     + Liberar Heap do Kernel.
 *     + Inicializar Heap e Stack do Kernel.
 *     + Gerenciar Page Directory, Page Table ...
 *     ...
 *
 * Obs:
 *     Uma regi�o � uma �rea de 4MB alocada. S�o todas as p�ginas
 * que cabem em uma tabela, 2014 p�ginas de 4KB cada. Isso tamb�m pode
 * ser chamado de pool e ser alocado todo de uma vez. At� mesmo
 * compartilhado entre processos.
 *
 * Sobre o heap de processo:
 *    Uma vari�vel global indica qual � o Heap do processo atual. 
 *    A estrutura de um processo cont�m informa��es sobre o seu Heap.
 *    ... 
 *
 * @todo: Criar: 
 * Criar uma fun��o que aloca mem�ria no heap de um processo. Deve-se usar 
 * o heap do desktop ao qual o processo pertence ou o heap do processo.
 * 
 * Obs: Poss�veis organiza��es de Heap: 
 *     + Heap Size Modifier, (HSM).
 *     + Heap Size Checker, (HSC). 
 *     + Desktop Heap Size Manager, (DHSM). 
 *       Gerencia o Heap alocado para cada desktop. Os programas usam o Heap
 * do seu pr�prio desktop. Alguns desktops podem ter Heaps pequenos.
 * @todo: Criar o arquivo heap.c para as rotinas de ger�ncia de heap.
 * Continua...
 *
 * Obs: Um processo pode criar um heap privado, que ele usar� para aloca��o 
 * din�mica.
 *      
 *
 * In this file:
 * =============
 *     +get_process_heap_pointer
 *     +SetKernelHeap
 *     +AllocateHeap
 *     +FreeHeap
 *     +AllocateHeapEx
 *     +show_memory_structs
 *     +init_heap
 *     +init_stack
 *     +init_mm
 *
 *
 * @todo: Criar: heapGetPointer, heapSePointer.
 *
 *
 * @todo: IMPORTANTE: Devemos ter um heap do kernebase, grande
 * o bastante para alocarmos os recursos gr�ficos grenciados
 * pelo m�dulo /gramado.
 *
 * @todo:
 * IMPORTANTE: Um alocador de mem�ria f�sica precisa ser criado,
 * que considere o tamanho da mem�ria f�sica dispon�vel.
 * Esse alocador deve ser usado para alocarmos uma regi�o bem grande
 * da mem�ria f�sica onde ficar�o os frames de mem�ria f�sica. Os 
 * frames livres ser�o encontrados nessa regi�o e o alocador de 
 * p�ginas, utilizar� esses frames livres para associ�-los �s
 * p�ginas alocadas aos processos.
 *
 * @todo: Vari�veis globais devem controlar o in�cio e o fim da �rea
 * destinada aos frames de mem�ria f�sica. semelhando ao que foi 
 * feito com o heap do kernel base. Faremos isso no in�cio do arquivo mm.h.
 *
 * Hist�rico:
 *     Vers�o 1.0, 2015 - Esse arquivo foi criado por Fred Nora.
 *     Vers�o 1.0, 2016 - Aprimoramento geral das rotinas b�sicas.
 *     ...
 */

 
#include <kernel.h>
 
 
//Vari�veis internas. 

//int mmStatus;
unsigned long last_valid;         //�ltimo heap pointer v�lido. 
unsigned long last_size;          //�ltimo tamanho alocado.
unsigned long mm_prev_pointer;    //Endere�o da �ntima estrutura alocada.



/*
 * memsize: 
 *     Testando manualmente o tamanho da mem�ria.
 *     ## Adaptado de SANOS - Michael Ringgaard.
 *     @todo: Corrigir o asm inline. usar padr�o gcc.
 *
 */

/* 
unsigned long memsize();  
unsigned long memsize() 
{
    volatile unsigned long *mem;
  unsigned long addr;
  unsigned long value;
  unsigned long cr0save;
  unsigned long cr0new;

  // Start at 1MB
  addr = 1024 * 1024;

  // Save a copy of CR0
  __asm { mov eax, cr0 };
  __asm { mov [cr0save], eax };

  // Invalidate the cache (write-back and invalidate the cache)
  __asm { wbinvd };

  // Plug cr0 with just PE/CD/NW (cache disable(486+), no-writeback(486+), 32bit mode(386+))
  cr0new = cr0save | 0x00000001 | 0x40000000 | 0x20000000;
  __asm { mov eax, [cr0new] };
  __asm { mov cr0, eax };

  //Probe for each megabyte
  
    while(addr < 0xFFF00000){
		
        addr += 1024 * 1024;
        mem = (unsigned long *) addr;

        value = *mem;
        *mem = 0x55AA55AA;

        if(*mem != 0x55AA55AA) break;

        *mem = 0xAA55AA55;
        if(*mem != 0xAA55AA55) break;

        *mem = value;  //corrige.
    };

  //Restore 
  
  __asm { mov eax, [cr0save] };
  __asm { mov cr0, eax };

  return addr;
}
*/

/*
 Traduz um endere�o virtual em um endere�o f�sico.
unsigned long memoryGetPhysicalAddress( unsigned long virtual_address);
unsigned long memoryGetPhysicalAddress( unsigned long virtual_address){
	//return (unsigned long) ;
}
*/

/*
unsigned long heap_set_new_handler( unsigned long address );
unsigned long heap_set_new_handler( unsigned long address )
{
    unsigned long Old;

    Old = kernel_heap_start;
    
    kernel_heap_start = address;

    return (Old);
};
*/


/*
 * get_process_heap_pointer:
 *     Pega o heap pointer do heap de um processo.
 *     Pega o endere�o do in�cio do header da pr�xima aloca��o.
 * Obs: Isso ser� usado para alocar mem�ria dentro do heap do processo.
 * Cada processo ter� seu heap, cada heap tem ponteiros.
 *
 */
unsigned long get_process_heap_pointer(int pid)
{
	unsigned long heapLimit;
	struct process_d *P;
	
	//@todo: Limite m�ximo.
	if(pid < 0){
		return (unsigned long) 0;
	};
	
	P = (void*) processList[pid];
	
	if((void*) P == NULL ){
	    return (unsigned long) 0;	
	};
	
	//Obs podemos checr 'used' e 'magic'.
	
	//
	// Limits:
	//     Cada processo tem uma �rea onde ele pode alocar mem�ria, os 
    // processos usam seu pr�prio heap ou o heap do desktop ao qual pertencem.
    // Os limites devem ser respeitados.	
	//
	
	heapLimit = (unsigned long) (P->Heap + P->HeapSize);
	
	//Se for menor que o in�cio ou maior que o limite.
	if( P->HeapPointer < P->Heap || P->HeapPointer >= heapLimit ){
		return (unsigned long) 0;
	};
	
done:
    //Retorna o heap pointer do processo. 
	return (unsigned long) P->HeapPointer;
};


/*
 * SetKernelHeap:
 *    Configura o heap do kernel.
 *    Reconfigura��o total do heap do kernel.
 *    @todo: Salvar em estrutura de processo. O kernel tem seu processo.
 */
void SetKernelHeap( unsigned long HeapStart, unsigned long HeapSize)
{
	struct heap_d *h;    //Kernel Heap struct.    
	
	
	//
	// Check limits.
	//
	
	//if(HeapStart == 0){}
	//if(HeapSize == 0){}

    //
    // Salvando em vari�veis globais.
    //
	
	//start, end, pointer, available.    
	
	kernel_heap_start = (unsigned long) HeapStart;                 
	kernel_heap_end   = (unsigned long) (HeapStart + HeapSize);
	g_heap_pointer    = (unsigned long) kernel_heap_start;            
	g_available_heap  = (unsigned long) (kernel_heap_end - kernel_heap_start); 
	
	
	
	//
	// ?? Devemos checar a validade dessa estrutura ??
	//
	
	//A estrutura fica no in�cio do heap.??!!
	h = (void*) kernel_heap_start;
	
	//Configurando a estrutura.
	h->HeapStart     = (unsigned long) kernel_heap_start;             
	h->HeapEnd       = (unsigned long) kernel_heap_end;
	h->HeapPointer   = (unsigned long) g_heap_pointer;            
	h->AvailableHeap = (unsigned long) g_available_heap; 	
	
	
	//
	// Configura a estrutura global que guarda informa��es sobre o heap do 
	// kernel. 'KernelHeap'
	//
	
	KernelHeap = (void*) h;
	
	//
	// Lista de heaps.
	//
	
	//@todo: Um 'for' pode inicializar essa lista com '0' nesse momento.
	//for(...)
	
	
	//Configura��o inicial da lista de heaps. S� temos 'um' ainda.
	heapList[0] = (unsigned long) KernelHeap;  //Configura o heap do kernel.
	heapList[1] = (unsigned long) 0;
	heapList[2] = (unsigned long) 0;
	//...
	
	//Contagem? ainda em zero.?!
	
done:	
	return;
};



/*
 * HeapAlloc:
 * @todo: Criar essa rotina.
 * Aloca mem�ria dentro de um heap determinado.
 * Esse rotina deve ser oferecida como servi�o e chamada via system call.
void* HeapAlloc(struct heap_d * heap, unsigned long size);
void* HeapAlloc(struct heap_d * heap, unsigned long size)
{
	return (void*) ...;
}
*/


/*
 * GetProcessHeap:
 @todo:
 Retorna um ponteiro para a estrutura do heap de um processo.
 Obs: Oferecer como servi�o do sistema.
void *GetProcessHeap(struct process_d *process);
void *GetProcessHeap(struct process_d *process)
{
	return NULL;
}
*/

/*
 * GetHeap:
 @todo:
 Retorna um ponteiro para o heap do processo atual.
 Obs: Oferecer como servi�o do sistema.
void *GetHeap();
void *GetHeap()
{
	//@todo: Pegar o identificador do processo atual.
	//pegar na estrutura do processo o ponteiro para seu heap.
	return NULL;
}
*/



/*
 ****************************************************************
 * AllocateHeap:
 *     Aloca mem�ria no heap do kernel.
 *
 * *IMPORTANTE: 
 *     Aloca BLOCOS de mem�ria dentro do heap do processo Kernel.
 *
 * Obs: 
 *     ?? A estrutura usada aqui � salva onde, ou n�o � salva ??
 *
 * @todo: 
 *     Ao fim dessa rotina, os valores da estrutura devem ser armazenas no 
 * header, l� onde foi alocado espa�o para o header, assim tem-se informa��es 
 * sobre o header alocado. ??
 *
 *  A estrutura header do heap, � uma estrutura e deve ficar antes da �rea 
 * desejada. Partes={header,client,footer}.
 *
 * @todo: 
 *     Essa rotina poderia se chamar memoryKernelProcessHeapAlloc(unsigned long size).
 *
 * 2015 - Created.
 * sep 2016 - Revision.
 * ...
 */
unsigned long AllocateHeap(unsigned long size)
{
	struct mmblock_d *Current;	
	//struct mmblock_d *Prev;	


    //
    // @todo: Aplicar filtro.
    // Aqui podemos checar se o quantidade de heap dispon�vel 
	// est� coerente com o tamanho do heap. Se essa quantidade 
	// for muito grande, maior que o heap total, ent�o temos um 
	// problema.
    //	
	
	//
	// Se n�o h� espa�o dispon�vel no heap, n�o h� muito o que fazer.
	// Uma op��o seria tentar almentar o heap, se isso for poss�vel.
	//
	
	// Available heap.
	if(g_available_heap == 0)
	{
	    //
		// @todo: Tentar crescer o heap para atender o size requisitado.
		//
		
		//try_grow_heap() ...
		
		//
		// @todo: Aqui poderia parar o sistema e mostrar essa mensagem.
		//
		
	    printf("AllocateHeap fail: g_available_heap={0}\n");
        refresh_screen();
        //while(1){} 
		
        return (unsigned long) 0;
		//while(1){};
	};
	
	
	
	//
    // Size limits. (Min, max).
    //
 	
	//Se o tamanho desejado for igual a zero.
	//@todo: Aqui podemos converter o size para uo tamanho m�nimo.
	// n�o h� problema nisso.
    if(size == 0){
		//size = 1;
	    printf("AllocateHeap error: size={0}\n");
		refresh_screen();
		return (unsigned long) g_heap_pointer;
	};
	
	//Se o tamanho desejado � maior ou igual ao espa�o dispon�vel.
	if(size >= g_available_heap)
	{
	    //
		// @todo: Tentar crescer o heap para atender o size requisitado.
		//

		//try_grow_heap() ...

		printf("AllocateHeap error: size >= g_available_heap\n");
		refresh_screen();
		return (unsigned long) 0;
	};
    
	//Salvando o tamanho desejado.
	last_size = (unsigned long) size;
	
	//
	// Contador de blocos.
	//
	
try_again:	

    mmblockCount++;  
    
	//Obs: Temos um limite para a quantidade de �ndices na lista de blocos.
	if( mmblockCount >= MMBLOCK_COUNT_MAX ){
        printf("AllocateHeap Error: mmblockCount limits!");
		refresh_screen();
		while(1){};
    };
	
    //Identificadores.	
	
	//
	// A vari�vel 'Header', no header do bloco, � o in�cio da estrutura que o 
	// define. 'b->Header'. Ou seja o endere�o da varir�l marca o in�cio da 
	// estrutura.
	//

	//
	// Pointer Limits:
	// (N�o vamos querer um heap pointer fora dos limites do heap do kernel)
	// Se o 'g_heap_pointer' atual esta fora dos limites do heap, ent�o 
	// devemos usar o �ltimo v�lido que provavelmente est� nos limites.
	// ?? #bugbug: Mas se o �ltimo v�lido est� sendo usado por uma aloca��o 
	// anterior. ?? temos flags que indiquem isso ??
	//
	// IMPORTANTE: O HEAP POINTER TAMB�M � O IN�CIO DE UMA ESTRUTURA. 
	// NESSA ESTRUTURA PODEMOS SABER SE O HEAP ESTA EM USO OU N�O.
	// ISSO SE APLICA � TENTATIVA DE REUTILIZAR O �LTIMO HEAP POINTER V�LIDO.
	//
	
	//Se estiver fora dos limites.
	if( g_heap_pointer < KERNEL_HEAP_START || 
	    g_heap_pointer >= KERNEL_HEAP_END )
	{
		//#bugbug: ?? Como saberemos, se o �ltimo v�lido,
		// n�o est� em uso por uma aloca��o anterior. ??
		
	    //Checa os limites o �ltimo last heap pointer v�lido.
	    if( last_valid < KERNEL_HEAP_START || 
		    last_valid >= KERNEL_HEAP_END )
		{
            printf("AllocateHeap Error: last valid heap pointer limits");
		    refresh_screen();
		    while(1){}		
	    };
		
		//
		// @todo: Checar a disponibilidade desse �ltimo v�lido.
		// Ele � v�lido, mas n�o sabemos se est� dispon�vel.
		
		//Havendo um last heap pointer v�lido.
		//?? isso n�o faz sentido.
		g_heap_pointer = (unsigned long) last_valid + last_size;
		goto try_again;
	};
	

    //
	// Agora temos um 'g_heap_pointer' v�lido, salvaremos ele.
	// 'last_valid' N�O � global. Fica nesse arquivo.
	//
	
	last_valid = (unsigned long) g_heap_pointer;
	
	//
	// Criando um bloco, que � uma estrutura mmblock_d.
	// Estrutura mmblock_d interna.
	// Configurando a estrutura para o bloco atual.
	//
	// Obs: A estutura dever� ficar l� no espa�o reservado para o header. 
	// (antes da area alocada).
	// Current = (void*) g_heap_pointer;
    //		
	
	//O endere�o do ponteiro da estrutura ser� o pointer do heap.
	Current = (void*) g_heap_pointer;    
	
	if( (void*) Current != NULL )
	{
		//Obs: Perceba que 'Current' e 'Current->Header' devem ser iguais. 
		
	    Current->Header = (unsigned long) g_heap_pointer;  //Endere�o onde come�a o header.
	    Current->headerSize = MMBLOCK_HEADER_SIZE;         //Tamanho do header. (*TAMANHO DA STRUCT).  
        Current->Id = mmblockCount;                        //Id do mmblock. (�ndice na lista)
	    Current->Used = 1;                //Flag, 'sendo Usado' ou 'livre'.
	    Current->Magic = 1234;            //Magic number. Ver se n�o est� corrompido.
        Current->Free = 0;                //0=not free 1=FREE (*SUPER IMPORTANTE)
	    //Continua...		
	
	    //
	    // Mensuradores. (tamanhos) (@todo:)
	    //
	
	    //
	    // @todo:
	    // Tamanho da �rea reservada para o cliente.
	    // userareaSize = (request size + unused bytes)
	    // Zera unused bytes, j� que n�o foi calculado.
	    //	

        //
	    // User Area base:
	    // *Onde come�a a �rea solicitada. 
		// *F�cil. Isso fica logo depois do header.
	    //
		
		//Obseve que 'Current->headerSize' � igual a 'MMBLOCK_HEADER_SIZE'
		// E que 'Current->headerSize' � o in�cio da estrutura.
	
	    Current->userArea = (unsigned long) Current->Header + Current->headerSize;    

	    //
	    // Footer:
        // O footer come�a no 'endere�o do in�cio da �rea de cliente' + 'o tamanho dela'.
	    // O footer � o fim dessa aloca��o e in�cio da pr�xima.
	    //
	
	    // #bugbug: Penso que aqui dever�amos considerar 'userareaSize' como 
		// tamanho da �rea de cliente, esse tamanho equivale ao tamanho solicitado,
		// mais o tanto de bytes n�o usados.
		// Obs: Por enquanto o tamanho da �rea de cliente tem apenas o tamanho
		// do espa�o solicitado.
	    Current->Footer = (unsigned long) Current->userArea + size;
	
	    //
	    // Heap pointer. 
	    //     Atualiza o endere�o onde vai ser a pr�xima aloca��o.
	    //
	
	    //if ( Current->Footer < KERNEL_HEAP_START){
	    //    Current->Used = 0;                //Flag, 'sendo Usado' ou 'livre'.
	    //    Current->Magic = 0;            //Magic number. Ver se n�o est� corrompido.	    
	    //	goto try_again;	
	    //}
	
        
		//
		// Obs: O limite da contagem de blocos foi checado acima.
		//
		
	    //
	    // Coloca o ponteiro na lista de blocos.
	    //
	
	    mmblockList[mmblockCount] = (unsigned long) Current;
	
	    //
		// Salva o ponteiro do bloco usado como 'pr�vio'.
	    // Obs: 'mm_prev_pointer' n�o � global, fica nesse arquivo.
	    //
		
		mm_prev_pointer  = (unsigned long) g_heap_pointer; 
	
	    //
		// *****************************************************
		//                **** SUPER IMPORTANTE ****
		// *****************************************************
		//
	    // Atualiza o ponteiro. Deve ser onde termina o �ltimo bloco 
		// configurado.
	    // Isso significa que o pr�ximo ponteiro onde come�aremos a pr�xima 
		// estrutura fica exatamente onde come�a o footer dessa estrutura.
		// Obs: O footer est� aqui somente para isso. Para ajudar
		// a localizamarmos o in�cio da pr�xima estrutura.
		//
		
		g_heap_pointer = (unsigned long) Current->Footer;	

	
	    //
	    // Available heap:
	    // Calcula o valor de heap dispon�vel para as pr�ximas aloca��es.
	    // O heap dispon�vel ser� o que t�nhamos dispon�vel menos o que 
		// gastamos agora.
		// O que gastamos agora foi o tamanho do header mais o tamanho da �rea 
		// de cliente.
		//
	
	    g_available_heap = (unsigned long) g_available_heap - (Current->Footer - Current->Header);		
	
	    //
	    // Retorna o ponteiro para o in�cio da �rea alocada.
		// Obs: Esse � o valor que ser� usado pela fun��o malloc.
		//
		// *Imporatate:
		//     O que acontece se um aplicativo utilizar al�m da �rea alocada ??
		// O aplicativo invadir� a �rea do footer, onde est� a estrutura do 
		// pr�ximo bloco. Inutilizando as informa��es sobre aquele bloco.
		// *Aviso: Cuidado com isso. @todo: Como corrigir.?? o que fzer??
		//
		
		return (unsigned long) Current->userArea;
		//Nothing.
		
	}else{
		
	    //Se o ponteiro da estrutura de mmblock for inv�lido.
		
		printf("AllocateHeap fail: struct.\n");
		//@todo: Deveria retornar.
		//goto fail;
		return (unsigned long) 0;
	};
	

	//
	// @todo: 
	// Checar novamente aqui o heap dispon�vel. Se esgotou, tentar crescer.
	//
	

    /*
	 * *IMPORTANTE
	 *
	 * @todo:
	 * Colocar o conte�do da estrutura no lugar destinado para o header.
	 * O header conter� informa��es sobre o heap.
	 *
	 */
	
	
	
	/* errado #bugbug.
	Prev = (void*) mm_prev_pointer;
	
	if( (void*) Prev != NULL)
	{
	    if( Prev->Used == 1 && 
		    Prev->Magic == 1234 && 
			Prev->Free == 0)
		{
		    Current->Prev = (void*) Prev;
            Prev->Next = (void*) Current;			
		};		
	};
	*/
	
	
//
// Fail.
//	
fail:
    //Se falhamos, retorna 0. Que equivaler� � NULL.
    return (unsigned long) 0;	
};



/*
 * FreeHeap: #bugbug
 * @todo: Implementar essa fun��o.
 * Objetivo: Liberar o bloco de mem�ria, configurando a sua estrutura.
 * Libera mem�ria.
 * O argumento � a diferen�a entra o ponteiro antigo e o novo ponteiro 
 * desejado. 
 * Ponteiros do in�cio da �rea do cliente.
 * ??
 */
unsigned long FreeHeap(unsigned long size){  		
	return (unsigned long) g_heap_pointer;    //#cancelada!
};


/*
 * AllocateHeapEx:
 *     Aloca heap.
 *     @todo: Fun��o imcompleta.
 *     Poderia chamar a fun��o AllocateHeap.
 */
void *AllocateHeapEx(unsigned long size){
	return (void *) AllocateHeap(size);  //suspensa.
};


/*
 * show_memory_structs:
 *     *IMPORTANTE.
 *     Mostra as informa��es das estruturas de mem�ria. 
 * @todo: 
 *     Mostrar a mem�ria usada pelos processos.
 *     Mostrar o quanto de mem�ria o processo usa.
 *     *Importante: Esse tipo de rotina mere�e muita aten��o
 * principalmente na fase inicial de constru��o do sistema.
 * Apresentar as informa��es em uma janela bem grande e 
 * chamar atrav�s do procedimento de janela do sistema � 
 * uma op��o bem interessante.
 * Por enquanto apenas escrevemos provavelmente na janela como 
 * foco de entrada e o procedimento de janela do sistema efetua
 * um refresh screen
 * 
 */
void show_memory_structs()
{
	int i = 0;
    struct mmblock_d *B;	
	
	// Title.
	printf("Memory Block Information:\n\n");
	//printf("=========================\n");
	
	//Mostra os heap da lista.		
	while(i < MMBLOCK_COUNT_MAX) 
	{
        B = (void*) mmblockList[i];
		++i;
		
		if( (void*) B != NULL )
		{
			//Validade.
		    if( B->Used == 1 && B->Magic == 1234){
		        printf("Id={%d} Header={%x} userA={%x} Footer{%x}\n",B->Id
		                                                            ,B->Header
				                                                    ,B->userArea
				                                                    ,B->Footer );
			};
			//Nothing.
		};
		//Nothing.
    };
	
	
	//
	// Aqui podemos aprentar informa��es sobre o heap.
	// como tamanho, espa�o dispon�vel, ponteiro, � que processo ele 
	// pertence.
	// mas estamos lidando a estrtutura de mmblock_d, que � especial e meio 
	// engessada.
	//
	
	//More?!
	
//
// Done.
//	
done:
    printf("Done\n");
	return;
};


/*
 * init_heap:
 *     Iniciar a ger�ncia de Heap do kernel. 
 *     @todo: Usar heapInit() ou heapHeap(). memoryInitializeHeapManager().
 *
 * Essa rotina controi a m�o o heap usado pelo processo kernel.
 *     +Ela � chamada apenas uma vez.
 *     +Ela deve ser chamada entes de quelquer outra opera��o 
 * envolvendo o heap do processo kernel.
 * 
 * @todo: Rotinas de automa��o da cria��o de heaps para processos.
 */
//int memoryInitializeHeapManager() 
int init_heap()
{
	//Internas.
	int i = 0;

    //Globals.	
	kernel_heap_start  = (unsigned long) KERNEL_HEAP_START;  
    kernel_heap_end    = (unsigned long) KERNEL_HEAP_END;  
	g_heap_pointer     = (unsigned long) kernel_heap_start;    //Heap Pointer.	
    g_available_heap   = (unsigned long) (kernel_heap_end - kernel_heap_start);    	// Available heap.
	heapCount = 0;      // Contador.
	
	//�ltimo heap pointer v�lido. (*IMPORTANTE)
	last_valid = (unsigned long) g_heap_pointer;
	last_size = 0;
	
	
	//Check Heap Pointer.
	if(g_heap_pointer == 0){
	    printf("init_heap fail: Heap pointer!\n");
		goto fail;
	};
	
	//Check Heap Pointer overflow.
	if(g_heap_pointer > kernel_heap_end){
        printf("init_heap fail: Heap Pointer Overflow!\n");
		goto fail;
    };	
	
    //Heap Start.
	if(kernel_heap_start == 0){
	    printf("init_heap fail: HeapStart={%x}\n" ,kernel_heap_start);
	    goto fail;
	};
	
	//Heap End.
	if(kernel_heap_end == 0){
	    printf("init_heap fail: HeapEnd={%x}\n" ,kernel_heap_end);
	    goto fail;
	};
	
	//Check available heap.
	if(g_available_heap == 0)
	{
	    //@todo: Tentar crescer o heap.
		
		printf("init_heap fail: Available heap!\n");
		goto fail;
	};
	
	// Heap list ~ Inicializa a lista de heaps.
	while(i < HEAP_COUNT_MAX){
        heapList[i] = (unsigned long) 0;
		++i;
    };
	
	
	//KernelHeap = (void*) x??;
	
	//More?!
	
// Done.
done:
    printf("Done.\n");	
	return (int) 0;
// Fail. Falha ao iniciar o heap do kernel.
fail:
    printf("init_heap: Fail!\n");
	
	/*
	printf("* Debug: %x %x %x %x \n", kernel_heap_start, 
	                                  kernel_heap_end,
									  kernel_stack_start,
									  kernel_stack_end);	
	refresh_screen();	 
    while(1){}		
	*/
    
	return (int) 1;
};


/*
 * init_stack:
 *     Iniciar a ger�ncia de Stack do kernel. 
 *     @todo Usar stackInit(). 
 *
 */
int init_stack()
{
    //Globals.
	//#bugbug
	kernel_stack_end   = (unsigned long) KERNEL_STACK_END; 
	kernel_stack_start = (unsigned long) KERNEL_STACK_START; 
	
    //End.
	if(kernel_stack_end == 0){
	    printf("init_stack fail: StackEnd={%x}\n" ,kernel_stack_end);
	    goto fail;
	};
	
	//Start.
	if(kernel_stack_start == 0){
	    printf("init_stack fail: StackStart={%x}\n" ,kernel_stack_start);
	    goto fail;
	};
	//Nothing.
done:
    return (int) 0;
fail:
    return (int) 1;
};


/*
 * memoryShowMemoryInfo:
 *     Show memory info.
 */
void memoryShowMemoryInfo()
{
	printf("Memory info:\n");
	printf("============\n");
	
	//sizes
	printf("BaseMemory     = (%d KB)\n", memorysizeBaseMemory);
	printf("OtherMemory    = (%d KB)\n", memorysizeOtherMemory);
	printf("ExtendedMemory = (%d KB)\n", memorysizeExtendedMemory);
	printf("TotalMemory    = (%d KB)\n", memorysizeTotal);
	
	//system zone
	printf("systemzoneStart  = 0x%x\n", systemzoneStart);
	printf("systemzoneEnd    = 0x%x\n", systemzoneEnd);
	printf("systemzoneSize   = 0x%x\n", systemzoneSize);

	//window zone.
	printf("windowzoneStart  = 0x%x\n", windowzoneStart);
	printf("windowzoneEnd    = 0x%x\n", windowzoneEnd);
	printf("windowzoneSize   = 0x%x\n", windowzoneSize);
	
	return;
}


/*
 * init_mm:
 *   Inicializa o memory manager.
 *    @todo: Usar mmInit().
 */
int init_mm()
{	
    int Status = 0;
	int i = 0;	
	
	//
	// @todo: 
	// Inicializar algumas vari�veis globais.
	// Chamar os construtores para inicializar o b�sico.
	//
	
	//
	// @todo: 
	// Clear BSS.
	// Criar mmClearBSS()
	//
	//
	

	
	
	
	//
	// Chamando uma rotina que cria e inicializa o heap do kernel manualmente.
	//
	
	//Heap.
	Status = (int) init_heap();
	if(Status != 0){
	    printf("init_mm fail: Heap.\n");
	    return (int) 1;
	};	
	
	//Stack.
	Status = (int) init_stack();
	if(Status != 0){
	    printf("init_mm fail: Stack.\n");
	    return (int) 1;
	};		
	
	//Lista de blocos de mem�ria dentro do heap do kernel.
	while(i < MMBLOCK_COUNT_MAX){
        mmblockList[i] = (unsigned long) 0;
		++i;
    };
	
	//Primeiro Bloco.
    //current_mmblock = (void *) NULL;
	
	
	//
	// MEMORY SIZES
	//
	
	
	//
	// Get memory sizes via RTC.
	//
	
	//Get extended Memory. (KB)
	memorysizeBaseMemory     = (unsigned long)  rtcGetBaseMemory(); //@todo 
	memorysizeOtherMemory    = (unsigned long) (1024 - memorysizeBaseMemory);
	memorysizeExtendedMemory = (unsigned long) rtcGetExtendedMemory();  //KB
    memorysizeTotal          = (unsigned long) (memorysizeBaseMemory + memorysizeOtherMemory + memorysizeExtendedMemory);
	
	
	
	//
	// MEMORY ZONES
	//
	
	
	//
	// Agora que temos o tamanho da mem�ria podemos definir as zonas 
	// de mem�ria de acordo com o tamanho da mem�ria.
	// inicializando as estruturas principais de gerenciamento de mem�ria.
	//
	
	
    //Se a mem�ria total for igual ou maior que 256MB.	
	//Estamos medindo em KB.
	//Isso seria o normal.
	if( memorysizeTotal >= (256*1024) )
	{
        //system zone
	    systemzoneStart = 0;
        systemzoneEnd   = 0x0FFFFFFF;
        systemzoneSize  = (systemzoneEnd - systemzoneStart);
	
	    //window zone
	    windowzoneStart = 0x10000000;
        windowzoneEnd   = systemzoneSize;    //?? Devemos levar em considera��o o calculo do tamanho da mem�ria
        windowzoneSize  = (windowzoneEnd - windowzoneEnd);
	}
    

    //Se a mem�ria total for igual ou maior que 128MB.	
	//Estamos medindo em KB.
	//Isso seria o normal.
	if( memorysizeTotal >= (128*1024) )
	{
        //system zone
	    systemzoneStart = 0;
        systemzoneEnd   = 0x07FFFFFF;
        systemzoneSize  = (systemzoneEnd - systemzoneStart);
	
	    //window zone
	    //windowzoneStart = 0x10000000;
        //windowzoneEnd   = systemzoneSize;    //?? Devemos levar em considera��o o calculo do tamanho da mem�ria
        //windowzoneSize  = (windowzoneEnd - windowzoneEnd);
	}
	
    //Se a mem�ria total for igual ou maior que 64MB.	
	//Estamos medindo em KB.
	//Isso seria o normal.
	if( memorysizeTotal >= (64*1024) )
	{
        //system zone
	    systemzoneStart = 0;
        systemzoneEnd   = 0x03FFFFFF;
        systemzoneSize  = (systemzoneEnd - systemzoneStart);
	
	    //window zone
	    //windowzoneStart = 0x10000000;
        //windowzoneEnd   = systemzoneSize;    //?? Devemos levar em considera��o o calculo do tamanho da mem�ria
        //windowzoneSize  = (windowzoneEnd - windowzoneEnd);
	}
	

    //Se a mem�ria total for igual ou maior que 32MB.	
	//Estamos medindo em KB.
	//Isso � o requisito m�nimo de mem�ria..
	if( memorysizeTotal >= (32*1024) )
	{
        //system zone
	    systemzoneStart = 0;
        systemzoneEnd   = 0x01FFFFFF;
        systemzoneSize  = (systemzoneEnd - systemzoneStart);
	
	    //window zone
	    //windowzoneStart = 0x10000000;
        //windowzoneEnd   = systemzoneSize;    //?? Devemos levar em considera��o o calculo do tamanho da mem�ria
        //windowzoneSize  = (windowzoneEnd - windowzoneEnd);
	};

	//32MB Isso � o requisito m�nimo de mem�ria..
	if( memorysizeTotal < (32*1024) ){
		printf("init_mm: Error Memory Size!\n");
	    printf("init_mm: TotalMemory={ %d KB}\n");
        refresh_screen();
		die();
	};
	
	//
	//Aviso!
	// Essas estruturas precisam ser inicializadas antes de inicializarmos 
	// a estrutura de user session. Depois, quando configurarmos a estrutura 
	// de user session devemos salvar o ponteiro aqui na estrutura 'windowzone'.
	//
	
	//Criando estruturas principais para o gerenciamento de mem�ria.
	//@todo: Conferir validade
	zones       = (void*) malloc( sizeof(struct mm_zones_d ) );
	systemzone  = (void*) malloc( sizeof(struct system_zone_d ) );
	windowzone	= (void*) malloc( sizeof(struct window_zone_d ) );
	
	//
	// Continua...
	//
	
done:	
    return (int) Status;	
}; 

//
// Segue rotinas de GC.
//

//limpa a camada /gramado
int gcGRAMADO(){
	return (int) 0;
}

/*
 gcEXECUTIVE:
    //limpa a camada /executive.
    Esse � o Garbage Collector, o trabalho dele � checar nas listas 
 de ponteiros de estrutura se encontra, estruturas sinalizadas para serem 
  liberadas. Quando encontra uma estrutura sinalizada, libera os recurso da 
estrutura.
  Obs: As estruturas poder�o ser deletadas ou n�o.
  Obs: A �rea do cliente na estrutura mmblock dever� se preenchida com zero
       quando a estrutura estiver sinalizada.
       O garbage collector � um servi�o do kernel, e poder� ser 
	   chamado por interrup��o. Pois utilit�rios de ger�ncia de mem�ria 
	   usar�o recursos de ger�ncia de mem�ria oferecidos pelo kernel.
  Obs: a fun��o gc() poder� ser chamada de tempod em tempos do mesmo modo 
       que o dead thread collector.
	   
  Obs: 
        Importante:
		O GC deve efetuar apenas uma opera��o de libera��o, mesmo que 
        haja muita coisa pra fazer, pois n�o queremos que ele use 
		muito tempo de processamento prejudicando os outros processos.
		Pois bem, ele efetua apenas uma opera��o de limpeza e sai sem erro.
		se n�o encontrar nada pra fazer, tamb�m sai sem erro.
		
		@todo: Cria os 'for's para as outras listas.
		 @todo: Criar as rotinas de limpeza para as outras listas.
		 
	//@todo 
	
	//+Checar as etruturas de mmblock e liberar 
	// as estruturas marcadas como Free=1.
	//para isso deve haver um array mmblockList[].
	
	//+Checar as estruturas de janela e liberar as 
	//estiverem marcadas como used=216 e magic=4321.

	*Importante:
	Essa rotina deve limpar aloca��es de mem�ria e n�o estruturas de 
	objetos de outros tipos.
	+O m�dulo de ger�ncia de recursos gr�ficos que limpe suas listas.
	+O m�dulo de ger�ncia de processos que limpe suas listas.
    +O m�dulo de ger�ncia de threads que limpe suas listas.
	
	
	
 */
int gcEXECUTIVE()
{
	int i;
	struct mmblock_d *b;  //memory block.
	struct heap_d *h;     //heap.
	//...
	
	//Obs: 
	// Importante: Cada lista cont�m um tipo de estrutura.
	// Importante: Limparemos somente as listas que pertencem ao m�dulo /ram
	
	//mmblockList[]
	for( i=0; i<MMBLOCK_COUNT_MAX; i++)
	{
	    b = (void*) mmblockList[i];
		
		//ponteiro v�lido.
	    if( (void*) b != NULL )
		{
			//sinalizado para o GC.
			if( b->Used == 216 && b->Magic == 4321 ){
				goto clear_mmblock;
			}
		}
	};
	
	
	//heapList[]
	for( i=0; i<HEAP_COUNT_MAX; i++)
	{
	    h = (void*) heapList[i];
		
		//ponteiro v�lido.
	    if( (void*) h != NULL )
		{
			//sinalizado para o GC.
			if( h->Used == 216 && h->Magic == 4321 ){
				goto clear_heap;
			}
		}
	};
	
	//
	// @todo: Cria os 'for's para as outras listas.
	//

	
    goto done;
	
	//
	// Segue opera��es de limpeza em estruturas de tipos diferentes.
	// Devemos limpar e sairmos em seguida.
	//
	
	//Nothing
	
clear_mmblock:	
    if( (void*) b != NULL )
    {
		//Checar se a �rea alocada est� dentro dos limites.
	    if( (b->userArea + b->userareaSize) != b->Footer ){
			printf("gc fail: User Area Size");
			goto fail;
		}else{
	        //preenche com zeros.
			bzero( (char*) b->userArea , (int) b->userareaSize);
		};
        //Nothing.		
	};
	goto done;
	//Nothing.
	
clear_heap:
    if( (void*) h != NULL )
	{
		// ?? O que fazer aqui ??
		
		//Limparemos mas n�o deletaremos.
		//h->Used  = 1;
		//h->Magic = 1234;
		
		//lista encadeada de blocos que formavam o heap.
		//podemos apenas sinalizar todos os mmblocks dessa lista e depois o GC acaba com eles.
		//para isso precisamos de um 'for'.
		//h->mmblockListHead = NULL;
        		
		
	};
	goto done;
	//Nothing.

	//
	// @todo: Criar as rotinas de limpeza para as outras listas.
	//

fail:
    refresh_screen();
	//while(1){}
	return (int) 1;
done:	
	return (int) 0;	
};

//limpa a camada /microkernel
int gcMICROKERNEL(){
	return (int) 0;
}

//limpa a camada /hal
int gcHAL(){
	return (int) 0;
}

/*
 ***********************************************************
    gc:
	    Garbage Collector.
		Call all Garbage Collections rotines.
		Clear all layers.
		/GRAMADO
		/EXECUTIVE
		/MICROKERNEL
		/HAL
 */
int gc()
{
    int Status;
	
//clearGramadoLayer:
    Status = (int) gcGRAMADO();
    if(Status == 1){
		printf("gc: clearGramadoLayer:\n");
		goto fail;
	}
	
//clearExecutiveLayer:
    Status = (int) gcEXECUTIVE();
    if(Status == 1){
		printf("gc: clearExecutiveLayer:\n");
		goto fail;
	}
	
//clearMicrokernelLayer:
    Status = (int) gcMICROKERNEL();
    if(Status == 1){
		printf("gc: clearMicrokernelLayer:\n");
		goto fail;
	}
	
//clearHalLayer:
    Status = (int) gcHAL();
    if(Status == 1){
		printf("gc: clearHalLayer:\n");
		goto fail;
	}
	
	goto done;
	
fail:
    printf("gc: FAIL.\n");	
	refresh_screen();
	//while(1){}
	return (int) 1;
	
done:
    return (int) 0;	
};



/*
int mmInit()
{
    //...	
}
*/

//
//fim.
//