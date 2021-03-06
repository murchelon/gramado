/*
 * File: mk\pc\process.h
 *
 * Descri��o:
 *     Header principal para as rotinas de gerenciamento de processos.
 *
 * Classes de processos (tipo de n�vel):
 * ====================================
 *    PB - Processos inclu�dos no Kernel Base.
 *    P0 - Processos em ring0.
 *    P1 - Processos em ring1.
 *    P2 - Processos em ring2.
 *    P3 - Processos em ring3, User Mode.
 *
 *
 * @todo: Taskman e outros programas gerenciadores de tarefas precisam de informa��es
 * sobre os processos. A barra de tarefas tamb�m precisa.
 * @todo: Contagem de processos.
 * @todo: Contar a quantidade de objetos alocados para um processo.
 *        contar a quantidade de objetos por classe de objetos.
 *        Saber a classe dos objetos alocados � importante, pois isso
 *        oferece informa��es sobre o comportamento do processo. 
 * @todo: Fazer contagem de faltas de p�gina.
 * @todo: suporte a workset.(quantidade de mem�ria numa faixa de tempo.)
 * +em qual cpi o processo est�.?
 * + com qual cpu o processo tem afinidade?.
 * + o processo est� confinado ou n�o.?
 * +prioridade b�sica e atual?
 * medir quantidade de opera��es de leitura e sa�da.
 * +name, pathname e cmd(command line).
 *
 * Hist�rico:
 *     Vers�o: 1.0, 2015 - Esse arquivo foi criado por Fred Nora.
 *     Vers�o: 1.0, 2016 - Aprimoramento geral das rotinas b�sicas.
 *     ...
 */


#define gramado_pid_t pid_t


//Limite para cria��o de processos. 
#define PROCESS_COUNT_MAX 1024   //256 
 
//
//#define FIRST_PROCESS processList[0]
//#define LAST_PROCESS  processList[PROCESS_COUNT_MAX-1] 

 
//
// Endere�os virtuais usados pelos processos.
//


// Image. 
// (Base da imagem de um processo de usu�rio).
//
// "Todo processo ter� seu pr�prio diret�rio de p�ginas
//  e ser� carregado no endere�o virtual 0x400000, logicamente
//  cada processo ser� carregado em um endere�o f�sico diferente."
// 
#define UPROCESS_IMAGE_BASE 0x400000 

//Process Limit. (User process) 
//(O limite � o in�cio do kernel).
#define UPROCESS_PROCESS_LIMIT 0xC0000000    



// Heap.
// Base default do heap do processo.
// "Endere�o virtual de onde come�a o heap de um processo,
//  evidentemente, cada processo ter� seu heap em um 
//  endere�o f�sico diferente".
//
#define UPROCESS_DEFAULT_HEAP_BASE  0x80000000 
//Tamanho default do heap do processo.  
#define UPROCESS_DEFAULT_HEAP_SIZE  0x2000     

//Stack.
//Deslocamento default do in�cio da pilha em rela��o ao in�cio do kernel. #bugbug
#define UPROCESS_DEFAULT_STACK_OFFSET 0x2000   
//Base default da pilha do processo.
#define UPROCESS_DEFAULT_STACK_BASE ( UPROCESS_PROCESS_LIMIT - UPROCESS_DEFAULT_STACK_OFFSET )  
//Tamanho da pilha do processo.   
#define UPROCESS_DEFAULT_STACK_SIZE 0x2000    
 
 

 
//
// Bases para a contagem de processos, do sistema e de usu�rios.
// 
 
//Primeiro �ndice na contagem das processos do sistema.
#define SYSTEM_BASE_PID 0
//Primeiro �ndice na contagem das processos dos usu�rios.  
#define USER_BASE_PID 100    
 
 
/*
 * Constantes para n�veis de prioridade.
 */
 
//Defini��es principais. 
#define PRIORITY_LOW4      1  //4
#define PRIORITY_LOW3      2  //3
#define PRIORITY_LOW2      3  //2
#define PRIORITY_LOW1      4  //1 
#define PRIORITY_NORMAL    5  //*0 (Normal).
#define PRIORITY_HIGH1     6  //1
#define PRIORITY_HIGH2     7  //2
#define PRIORITY_HIGH3     8  //3
#define PRIORITY_HIGH4     9  //4
//Defini��es secund�rias.
#define PRIORITY_LOW        PRIORITY_LOW1
#define PRIORITY_SUPERLOW   PRIORITY_LOW4
#define PRIORITY_MIN        PRIORITY_SUPERLOW
#define PRIORITY_HIGH       PRIORITY_HIGH1 
#define PRIORITY_SUPERHIGH  PRIORITY_HIGH4
#define PRIORITY_MAX        PRIORITY_SUPERHIGH    
//Defini��o especial.
#define PRIORITY_REALTIME  10    


#define TIMESLICE_MULTIPLIER 2
//#define TIMESLICE_MULTIPLIER 1
//...

/*
 * Constantes para n�veis de quantum.
 * Tempo de processamento atribuido pelo scheduler � cada processo.
 * dado em quantidades de ticks ...
 * para saber quanto tempo tem que multiplicar por 10ms ... que � 
 * o tempo de cada interrup��o ... e somar o tempo perdido com taskswitch ...
 *  100ms + 5ms(que o timer fica esperando o kernel habilitar as interrup��es).
 */
#define QUANTUM_BASE   (PRIORITY_NORMAL*TIMESLICE_MULTIPLIER)
#define QUANTUM_LIMIT  (PRIORITY_REALTIME*TIMESLICE_MULTIPLIER)


//Limite de tempo esperando.
#define READY_LIMIT   (PRIORITY_REALTIME*TIMESLICE_MULTIPLIER)
#define WAITING_LIMIT (PRIORITY_REALTIME*TIMESLICE_MULTIPLIER)
#define BLOCKED_LIMIT (PRIORITY_REALTIME*TIMESLICE_MULTIPLIER)
 
//Lista de status na cria��o de um processo.     
#define ERRO_SLOT_OCUPADO  0xfffff    //Slot ocupado.      
#define ERRO_DEFAULT       0xffffe
//...


//Process runtime stack.
//??? Deve ser o limite m�ximo para a pilha de um processo.
#define MAXSTACK 128    //dwords.
 
//N�mero total de slots para cria��o de processo.
//@todo: deletar, definido em threads. ??? 
#define NUMERO_TOTAL_DE_SLOTS 256   
 
#define PROCESS_MAGIC 1234
 
/*
 * Globais.
 *
 * @todo: Talvez o certo � n�o ter globais aqui, e
 *        as estruturas e vari�veis relativas ao gerenciamento
 *        de processos fique dentro do arquivo process.c.
 *        Encapsulamento.?? 
 */ 
 
 

/*
 * process_state_t
 *     Status de um processo.
 *     @todo: Pode-se usar mais status.
 *            como estado de transi��o.
 *
 *  Os status de um processo s�o diferentes do status de uma thread.
 *
 */ 
typedef enum {
	PROCESS_CREATED,
	PROCESS_INITIALIZED,
	PROCESS_RUNNING,
	PROCESS_BLOCKED,
	PROCESS_TERMINATED,
    PROCESS_IN_MEMORY,        //O processo est� carregado na mem�ria.
	PROCESS_OUT_OF_MEMORY,    //O processo n�o est� carregado na mem�ria.
	//...
}process_state_t;


/* 
 appmode_t
 
 Importante:
 
 APPMODE_TERMINAL = O kernel cria uma estrutura de terminal 
 com uma janela associada a essa estrutura, essa janela ser� a 
 janela de terminal para o aplicativo.
 APPMODE_WINDOW = O kernel n�o cria estrutura de terminal para 
 esse processo e o processo criar� janelas.
 */
typedef enum {
    APPMODE_NULL,      // Isso se aplica ao processo kernel e ao processo idle por exemplo.	
    APPMODE_TERMINAL,  // O kernel cria uma janela de terminal para o aplicativo.
	APPMODE_WINDOW,    // O kernel n�o cria janela de terminal para o aplicativo
}appmode_t;

/*
 * Estruturas para processos.
 */

  
/*
 * process_d: 
 *
 *    PCB - Process Control Block.
 *
 *    Estruturas para processos.
 *    Estrutura onde se guarda informa��es sobre os processos.
 *    Cada estrutura ter� informa��es necess�rias para
 *    gerenciar um processo espec�fico.
 *    S�o os SLOTS. 
 *
 *    Obs:
 *        Devem ter informa��es sobre o carregamento,
 *        se est�o na memoria ou n�o, 
 *        se est�o em processo de transi��o. etc.
 *        *IMPOTANTE: Uso de cotas de recursos.
 *
 *    @todo: 
 *        Deve-se come�ar listando os recursos que o processo usa.
 *        Ex: Lista de arquivos, 
 *            lista de recursos que pretende usar.
 *            permiss�es, acessos, 
 *            (isso tem a ver com o usu�rio, dono do processo).
 *
 *     Obs: Fica por �ltimo o que for referenciado com menor frequ�ncia.
 *          Onde ficar�o os diret�rios e as p�ginas dos processos.(mm)
 *          O kernel precisa aloca memoria para os PDs PTs.
 *          O kernel precisa alocar mem�ria para Heap e Stack dos processos.
 *          //...
 */
struct process_d 
{
	object_type_t objectType;
	object_class_t objectClass;
	
	//object control
	struct object_d *object;
	
	//callback ,d.
	
    /*
	 * Identificadores.
	 */
	 
	//PID, (Process Identification), 
	//c, N�mero que identifica unicamente um processo.	
	int pid;
	//PPID, (Parent Process Identification),
    //b, N�mero de identifica��o do processo pai.	
	int ppid;                
	//UID, (User Identification),
	//a, N�mero de identifica��o do usu�rio que criou o processo.
	int uid;
	//GID, (Group Identification),
	//g, N�mero do grupo do dono do processo.
	int gid;
	
	unsigned long Streams[NUMBER_OF_FILES];
	
	//State.
	process_state_t state; // flag,
	

    //plano de execu��o.
	int plane;

    //unsigned long error; // error.	

	unsigned long used;     //@todo: Poderia ser short.
    unsigned long magic;    //@todo: Poderia ser short.
	
	
	//
	// @todo:
	// +name     (Nome=EXEMPLO.BIN)
	// +pathname (Caminho=/root/boot/EXEMPLO.BIN)
	// +cmd      (linha de comando ex:EXEM ) 
	//
	
	unsigned long name_address;  //@todo: n�o usar isso.
	char *name;             //Nome do processo. 
	//char *pathname;       //@todo: Incluir.
	char *cmd;              //Nome curto que serve de comando.



	
	//Se o processo � ou n�o um processo de terminal.
	//que aparece no terminal.
	int terminal;
	
    //Importante:
	//isso substituir� a flag 'terminal'
    //APPMODE_TERMINAL = O kernel cria uma estrutura de terminal 
    //com uma janela associada a essa estrutura, essa janela ser� a 
    //janela de terminal para o aplicativo.
    //APPMODE_WINDOW = O kernel n�o cria estrutura de terminal para 
    //esse processo e o processo criar� janelas.
	appmode_t appMode;
	
	//
	//    ****  Banco de dados ****
	//
	
	//
	//Obs: Um processo � um cliente de banco de dados.
	//     Um processo tem contas conjuntas e pessoais.
    //     um processo poder� compartilhar esses objetos.	
    //

	//
	// BANCOS
	//
	
	//Acesso ao banco de dados do kernel. (n�o � uma lista).
	struct bank_d *kdb;
	
	//Lista de acessos � bancos de contas conjuntas.
	struct bank_d *gdbListHead;
	
	//Lista de acessos � bancos de contas pessoais.
	struct bank_d *ldbListHead;
	
	
	//
	// CONTAS
	//
	
	//Lista de contas conjuntas que o processo tem.
	struct aspace_d *aspaceSharedListHead;
	
	//Lista de contas pessoais que o processo tem.
	struct aspace_d *aspacePersonalListHead;


	//Lista de contas conjuntas que o processo tem.
	struct dspace_d *dspaceSharedListHead;
	
	//Lista de contas pessoais que o processo tem.
	struct dspace_d *dspacePersonalListHead;
	
	//Testing...
	//Process Page Table. (PPT)
	//Quais s�o as p�ginas que o processo est� usando e onde elas est�o.
	//na tabela tem que ter: O n�mero da p�gina. onde ela est�, se no disco ou se na mem�ria
	//qual � o n�mero do pageframe da p�gina e qual o status de modifica��o
	// Bom, acho que pra isso, o processo deve trabalhar em um n�mero limitado de pagetables
	// pois o n�mero do frame deve ser o �ndice da pte dentro da page table.
	// Uma pagetable tem 1024 entradas, garantindo um n�mero de 1024 frames, o que d� 4MB.
	// Mas se um processo tiver mais que 4MB? Ent�o o diret�rio de p�ginas do processo
	// ter� que ter mais de uma page table.
	// Obs: O kernel deve manter uma lista de frames que podem ser usados.
	//      na hora de criar uma pagetable pra um page directory de um processo
	// � necess�rio pegar na lista de frames 1024 frames livres ou quanto for necess�rio.
	//
	
	// **** IMPORTANTE ****
	// Uma lista de framepool. (Lista de parti��es de mem�ria f�sica).
	// Cada framepool � composto de 1024 frames.
	// Obs: *IMPORTANTE: Quando um processo � criado, pelo menos um framepool 
	// deve ser atribu�do a ele, mesmo antes de mapear os frames desse 
	// framepool em alguma pagetable do page directory do processo.
	//
	struct frame_pool_d *framepoolListHead;
	
	
	//Quantidade de mem�ria f�sica usada pelo processo que n�o pode ser compartilhada
	//com outros processos. (em KB).
	unsigned long private_memory_size;
	
	//Quantidade de mem�ria f�sica usada pelo processo que pode ser compartilhada
	//com outros processos. (em KB).
	unsigned long shared_memory_size;
	
	//Quantidade de mem�ria usada por um processo em determinado espa�o de tempo.
	// workset = (private + shared);
	unsigned long workingset_size;
	unsigned long workingset_peak_size;	
	

	//Qualquer pagefault deve ser registrada na estrutura do processo corrente.
	unsigned long pagefaultCount;
	//...
	
	
    //double Cycles;                   //ticks running .. 
    //unsigned long ContextSwitches;   //quantas vezes no total o dispacher atuou sobre ele.
    //unsigned long ContextSwitchesDelta;  //Quantas trocas de context sofreu durante um determinado tempo de an�lise.
	//esse deve ser o mesmo tempo de an�lise usado para calcular o working set.
	
	
	//
	//files.
	//
	//struct FILE Files; lista de arquivos usados por um processo.	

	//file descriptors.
	//struct file_t *files; //poteiro para array de estruturas de arquivos.
	
	//inicial thread.
	//struvt thread_d *thread; 
	
	//
	// MEMORY !!!  Image + Heap + Stack 
	//
	
	//struct image_t *processImageMemory;  // process image.(binario)
	//struct heap_t  *processHeapMemory;   //process heap
	//struct stack_t *processStackMemory;  //process stack.
	
    	
	//
	// ORDEM: O que segue � referenciado durante o processo de task switch.
	//

	/*
	 * Page directory information:
	 * ==========================
	 *
	 *     Todo processo deve ter seu pr�prio diret�rio.
	 *
	 *     As threads usam o diret�rio do processo ao qual pertencem.
	 *     O endere�o do diret�rio � carregado no CR3.
	 *
	 */
	
	
	unsigned long DirectoryVA;                  
	unsigned long DirectoryPA;
	
	
    struct page_directory_d *page_directory;  //ponteiro para a estrutura do diret�rio do processo.

	//
	// Teste: Blocos de memoria usados pelo processo.
	//
	
	//struct mmblock_d mmBlocks[32];    //estruturas 
	//unsigned long mmblockList[32];    //ponteiros para estruturas.
	
	//Muitas informa��es sobre a mem�ria usada pro um processo.
	//struct process_memory_info_d *processMemoryInfo;
	
	
	//Image support.
	
	unsigned long Image;          //Base da imagem do processo.
	unsigned long ImageSize;      //Tamanho da imagem do processo.


	// ## Heap ##     

	//#importante 
	unsigned long Heap;            //Endere�o do in�cio do Heap do processo.
	unsigned long HeapEnd;
	unsigned long HeapSize;        //Tamanho do heap.
	
	//?? Isso serve para manipula��o do heap do processo.
	unsigned long HeapPointer;     //Ponteiro do heap do processo para o pr�xima aloca��o.
	unsigned long HeapLastValid;   //�ltimo ponteiro v�lido.
	unsigned long HeapLastSize;    //�ltimo tamanho alocado..	
	//struct heap_d *processHeap;  //@todo: Usar essa estrutura.

	
	//  ## Stack ##
		
	unsigned long Stack;          //Endere�o do in�cio da Stack do processo.
	unsigned long StackEnd;
	unsigned long StackSize;      //Tamanho da pilha.	
	unsigned long StackOffset;    //Deslocamento da pilha em rela��o ao in�cio do kernel.	
	//struct stack_d *processStack;  //@todo: Criar essa estrutura.

	
	//IOPL of the task. (ring).
	unsigned long iopl;      

	//PPL - (Process Permition Level).(gdef.h)
    //Determina as camadas de software que um processo ter� acesso irrestrito.	
	process_permition_level_t ppl;

	//
	// Os procedimentos s�o pontos de entrada npos processos via iret.
	// o kernel utiliza eles para se comunicar com os processos.
	// Podemos ter v�rios pontos de entrada em um processo.
	// Configurados de diversas maneiras. Esses procedimentos ipc
	// s�o configurados pelo pr�prio processo na hora de sua inicializa��o.
	// Se o processo n�o configur�-los, o valor atribu�do ser� NULL e eles
	// ficar�o inv�lidos. Mas um programa n�o ser� fechado se perdermos
	// a referencia � um de seus procedimentos do tipo ipc. esses procedimentos
	// poder�o ser configurados pelo programa a qualquer hora, em tempo de execu��o.
	// J� um procedimento de janela n�o pode ser fechado. Perdendo o procedimento de
	//  janela o programa fica comprometido, perdendo completamente o controle
	// que ele tinha sobre si pr�prio, ficando por conta do sistema operacional
	// todo o controle sobre o programa agora sem procedimento de janela.
	//
	
	
	//
	// Procedimentos IPC;    
	//

	//O endere�o do procedimento de ipc de um processo.
	unsigned long ipc_procedure;
	unsigned long ipc_procedureEx;	
	//@todo: Criar esses e outros pontos de entrada nos processos.
	//unsigned long ipc_procedureFast;             //S� um argumento.
	//unsigned long ipc_procedureInitializeDriver; //S� um argumento.	
	//...
	
	//
	// Os procedimentos de janelas atual somente sobre a janela
	// que receber ou pegar a mensagem via chamada de procedimento.
	// Normalmente a janela ativa do programa tem o procedimento de
    // janela que comanda tudo dentro do programa; Se fecharmos
    // a janela ativa, perdemos a estrutura de janela que nos
    // d� acesso ao procedimento de janela que controla o programa
    // ent�o o programa deve ser fechado.
	// Um procedimento de janela n�o pode ser fechado. Perdendo o procedimento de
	//  janela o programa fica comprometido, perdendo completamente o controle
	// que ele tinha sobre si pr�prio, ficando por conta do sistema operacional
	// todo o controle sobre o programa agora sem procedimento de janela.	
	//
	
	//
	// Procedimentos de janela.
	//
	
	//Endere�o do procedimento de janela da processo. 
	unsigned long procedure;  
	
	//Argumentos para o procedimento de janela.
	struct window_d *window;    //arg1.
	int msg;                    //arg2.
	unsigned long long1;        //arg3.
	unsigned long long2;        //arg4.

		
	//struct process_d *callerq;	   //head of list of procs wishing to send.
    //struct process_d *sendlink;    //link to next proc wishing to send.
    //void *message_bufffer;		   //pointer to message buffer.
    //int getfrom_pid;		       //from whom does process want to receive.
    //int sendto_pid;	               //pra quem.
	
	//
	// @todo: Criar uma fila de mensagens.
	//
	

	
	//@todo: mailBox, sockets ... (IPC).
	
	
    //
	// @todo: Criar fila de mensagens do processo.
	//        Pois cada processo tem sua fila de mensagens.
	//        O procedimento de janela do processo pega mensagens aqui.
    //
	
	//unsigned char process_message_queue[8];
	

	/*
	 * Priority.
     * ========
     * Um processo tem uma prioridade b�sica est�tica e tamb�m uma prioridade 
	 * atual din�mica, que pode ser incrementada ou decrementada. Se afastando 
	 * ou se aproximando da prioridade b�sica. Isso acontece no OpenVMS e no NT.
	 */
	unsigned long base_priority; //b�sica. 
	unsigned long priority;      //din�mica.
	
	//Que tipo de scheduler o processo utiliza. (rr, realtime ...).
	//int scheduler_type;    
	
	/* 
	 * Temporiza��o da tarefa. 
	 */
	
	unsigned long step;               //Quantas vezes a tarefa usou o processador. 
	unsigned long quantum;            //thread quantum
	unsigned long timeout;            //Tempo em modo de espera. 
	unsigned long ticks_remaining;    //rt, quanto tempo a tarefa tem dispon�vel para ser concluida.
	
	//unsigned long alarm;            //Tempo para o pr�ximo alarme, dado em ticks.
	
	//unsigned long ThreadQuantum;    //As threads do processo iniciam com esse quantum.
	
	/*
	 * QUANTUM:
	 * =======
	 *     Quantum � a cota atrituida � cada processo pelo
	 *     scheduler. (time slice) 
	 *     Esse � o tempo limite imposto pelo scheduler para
	 *     execu��o de um processo. Ocorre ent�o uma preemp��o por tempo.
	 *      
	 *     'ProcessingTime' � atribu�do pelo processo. � o quanto
	 *     ele precisa. o quanto ele deseja.
	 */
	
	//
	//  +++++ Thread support +++++
	//
	
	int threadCount;    //N�mero de threads do processo. 
	
	/*
	 * threadList:
	 *     Lista com ponteiros de estrutura das threads do processo.
	 *     O indice dessa lista serve para enumer�las.
     *	 
	 *     @todo: Usar array de estruturas din�mico. (Alocar).
	 */ 
	unsigned long tList[32];      //@todo: deletar  
	//struct thread_d *Threads;   //@todo: usar esse.
	//struct thread_d CurrentThread;
	
	//A primeira thead de uma lista linkada.
	struct thread_d *threadListHead;
	//struct thread_d *threadReadyListHead;
	//...	
	

    //#importante:
	//thread de input
	
    struct thread_d *InputThread;	
	
	//@todo: estat�stica de tempo de uso de cpi.
	
	//@todo: Tipo cpu-bound, i/o bound.
	
	/*
	 * window:
	 *    ID da janela m�e da tarefa.
	 *    Na estrutura de janela ter� v�rias informa��es importantes. como:
	 * procedimento de janela, menus, procedimentos de menus ... etc.
	 *   @todo: criar vari�vel.
	 *
	 */
	//int window_id;
	
	/*
	 * preempted:
	 *     flag ~ Sinaliza que uma tarefa pode ou n�o sofrer preemp��o.
     *	   Uma tarefa de menor prioridade pode deixar o estado running 
	 * para assumir o estado ready em favor de uma tarefa de maior prioridade
	 * que assumir� o estado running.
	 *
	 */
	unsigned long preempted;
	
	//saved ~ Sinaliza que a tarefa teve o seu contexto salvo.
	unsigned long saved;
	
	//??
	unsigned long PreviousMode;	
	
	
	/*
	 * event: 
	 *    Tipo de evento que fazem a tarefa entrar em modo de espera. 
	 */	
    //event_type_t event;
		
	
	/*
	 * Windows. (Janelas)
	 */
	
	//user session, window station e desktop. 
	//Obs: A estrutura de user session contem muitas informa��es.
	//@todo: usar processWindowStation processDesktop;
	//struct usession_d *processUserSession;
	struct wstation_d *window_station;    //Window Station do processo.  
	struct desktop_d *desktop;           //Desktop do processo.        
	

	//
	// ORDEM: O que segue � referenciado com pouca frequ�ncia.
	//
	
	//lista de arquivos ??
	//fluxo padr�o. stdio, stdout, stderr
	//unsigned long iob[8];
    
	//ponteiros para as streams do fluxo padr�o.
	unsigned long standard_streams[3];
	
	struct _iobuf *root;	// 4 root directory
	struct _iobuf *pwd;	    // 5 (print working directory) 
	//...
		
		
	//#todo: esse tamanho deve ser igual ao encontrado no m�dulo /fs.	
	char pwd_string[32];	
	
	// @todo:
	// Outros:
	//tempo de cpu.
	//prioridade b�sica. //Obs: prioridade poderia ser uma estrutura.
	//contagem de threads usadas no momento.
	//path name - Endere�o completo do arquivo ex:/root/users/fred/text.bin
	//comando. - Comando que abre o programa.
	//Contagem de objetos sendo usado. (estatisticas ajudam melhorar o sistema.)
	//i/o read count - Contagem de leituras de disco feitas pelo processo.
	//i/o write count - Contagem de grava��es de disco feitas pelo processo.
	//generic i/o - Outras opera��es de i/o que n�o s�o leitura e escrita em disco.
	//todal de bytes lidos do disco pelo processo.
	//total de bytes gravados no disco pelo processo.
	//generic bytes i/o. contagem de bytes para opera��es gen�ricas de i/o, excluindo disco.
	

	//wait4pid: 
    //O processo pode estar esperando um processo filho fechar. 
	//@todo: usar 'int'.
	unsigned long wait4pid; 
	
	//Motivo do processo fechar.
	int exit_code;
	
	// Lista de processos filhos que est�o no estado zumbi.
	// List of terminated childs
	struct process_d *zombieChildListHead;           
	
    //?? mensagens pendentes.	
	//struct thread_d *sendersList; //Lista encadeada de processos querendo enviar mensagem
	//struct thread_d *nextSender;  //pr�ximo processo a enviar mensagem.
	
    //
    //Next.
    //	
	
    //PID do pr�ximo processo. 
    //(@todo: usar next_process). 	
	int next_task;  //@todo deletar isso.
    //int next_process;
    //struct process_d *NextInitialized;    //Pr�ximo pronto pra rodar.		
	//struct process_d *Parent;
	//struct process_d *Prev;
	
	
	//Signal
	unsigned long signal;
	unsigned long signalMask;
	
	struct process_d *Next;
};

//Os quatro principais processos.
struct process_d *KernelProcess;     //PID=0.   Ok.
struct process_d *InitProcess;       //PID=100. ?
struct process_d *ShellProcess;      //PID=101. ?
struct process_d *TaskManProcess;    //PID=103. ?

//Outros.
struct process_d *Process;           //Current.
struct process_d *idle_proc;           //Iddle. //@todo: deletar
struct process_d *cur_process;         //Current.

//Lista encadeada de processos.
struct process_d *process_Conductor2;
struct process_d *process_Conductor;
struct process_d *process_rootConductor;
struct process_d *CurrentProcess;    //Current.

//Gen�rico.
//process_descriptor_t *PCB;
//...


/*
 *    processList:
 *        Tabela de processos.
 *
 *    **** LONG-TERM SCHEDULER FOR PROCESSES ****
 *
 *     Essa � a thread job list, ou job queue.
 *     Armazena todos os processos do sistema.
 *     os que est�o residentes na memoria ram e as que n�o est�o.
*/  
unsigned long processList[PROCESS_COUNT_MAX];


//
// @todo: Aqui pode haver outras tabelas de processos:
//

//unsigned long running_processList[8];
//unsigned long running_processList[8];
//unsigned long running_processList[8];
//unsigned long running_processList[8];
//...




/*
 * Lista de processos:
 *   Em forma de fila.
 *
 */
typedef struct proc_list_d proc_list_t;
struct proc_list_d
{
	unsigned long len;
	struct process_d *head;
	struct process_d *tail;
	
}; 
struct proc_list_d *system_procs;      //Processos do sistema.
struct proc_list_d *periodic_procs;    //Processos peri�dicos.
struct proc_list_d *rr_procs;          //Processos do tipo round robin.
struct proc_list_d *waiting_procs;     //Processos que est�o esperando.
//...  




/*
 * process_info_d:
 *    Informa��es b�sicas sobre um processo. 
 *    Quick access.
 */
struct process_info_d 
{
	int processId;
	struct process_d *process; 	
    
	// Thread principal.
	int threadId;
	struct thread_d *thread;

    //... 	
};


/* 
 * Linux style.
#define invalidate() \
 asm("movl %%eax,%%cr3"::"a" (0))
*/

//
// Prot�tipos de fun��o.
//


//clona um processo sem thread.
int do_fork_process();

//cria uma estrutura do tipo processo, mas n�o inicializada.
struct process_d *processObject();

//pegar um slot vazio na lista de processos.
//isso permite clonar um processo,
int getNewPID();


// Signal.
int 
processSendSignal( struct process_d *p, 
                   unsigned long signal );


//
// Page directory support.
//
 
unsigned long GetPageDirValue();

unsigned long GetProcessDirectory( struct process_d *process );

void 
SetProcessDirectory( struct process_d *process, 
                     unsigned long Address );


//
// Process support.
//

unsigned long GetProcessHeapStart( int pid );

unsigned long GetProcessPageDirectoryAddress( int pid );


int processTesting(int pid);

void init_processes();

void show_currentprocess_info ();
void show_process_information ();


//copiar um processo.
//isso ser� usado por fork.
int processCopyProcess( int p1, int p2 );

struct process_d *create_process( struct wstation_d *window_station,
                                  struct desktop_d  *desktop,
                                  struct window_d *window,
                                  unsigned long init_eip, 
                                  unsigned long priority, 
								  int ppid, 
								  char *name,
								  unsigned long iopl,
                                  unsigned long directory_address );
									
//Finaliza��es.
void CloseAllProcesses();									
void dead_task_collector();    
//@todo: mudar para dead_process_collector().
//void dead_process_collector();

/*
 * exit_process:
 *     exit process..
 *
 *     Torna o estado PROCESS_TERMINATED.
 *     mas n�o destr�i a estrutura DO PROCESSO.
 *     Outra rotina destruir� as informa��es.
 *     liberara a mem�ria.     
 */
void exit_process(int pid, int code);

void set_caller_process_id(int pid);
int get_caller_process_id();

int init_process_manager();

									
//
// Fim.
//

