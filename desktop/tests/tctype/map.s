
Allocating common symbols
Common symbol       size              file

prompt              0x400             main.o
idleStatus          0x10              main.o
ClientAreaInfo      0x10              main.o
BufferInfo          0x10              main.o
driverInitialized   0x10              main.o
stderr              0x10              main.o
prompt_pos          0x10              main.o
current_mmblock     0x10              stdlib.o
mm_prev_pointer     0x10              stdlib.o
messagebox_button2  0x10              api.o
g_using_gui         0x10              main.o
stdin               0x10              main.o
heap_end            0x10              stdlib.o
rect                0x10              main.o
g_available_heap    0x10              stdlib.o
heap_start          0x10              stdlib.o
heapCount           0x10              stdlib.o
HEAP_START          0x10              stdlib.o
prompt_max          0x10              main.o
CurrentWindow       0x10              main.o
g_heap_pointer      0x10              stdlib.o
heapList            0x400             stdlib.o
prompt_err          0x400             main.o
randseed            0x10              stdlib.o
current_semaphore   0x10              main.o
g_columns           0x10              main.o
HEAP_SIZE           0x10              stdlib.o
stdout              0x10              main.o
Streams             0x80              main.o
last_size           0x10              stdlib.o
last_valid          0x10              stdlib.o
g_rows              0x10              main.o
prompt_out          0x400             main.o
mmblockList         0x400             stdlib.o
dialogbox_button1   0x10              api.o
libcHeap            0x10              stdlib.o
g_cursor_y          0x10              main.o
messagebox_button1  0x10              api.o
Heap                0x10              stdlib.o
dialogbox_button2   0x10              api.o
CursorInfo          0x10              main.o
mmblockCount        0x10              stdlib.o
prompt_status       0x10              main.o
g_char_attrib       0x10              main.o
ApplicationInfo     0x10              main.o
idleError           0x10              main.o
g_cursor_x          0x10              main.o
HEAP_END            0x10              stdlib.o

Memory Configuration

Name             Origin             Length             Attributes
*default*        0x00000000         0xffffffff

Linker script and memory map

                0x00400000                __image_base__ = 0x400000
                0x00000000                __dll__ = 0x0
                0x00400000                ___ImageBase = 0x400000
                0x00001000                __section_alignment__ = 0x1000
                0x00000200                __file_alignment__ = 0x200
                0x00000004                __major_os_version__ = 0x4
                0x00000000                __minor_os_version__ = 0x0
                0x00000001                __major_image_version__ = 0x1
                0x00000000                __minor_image_version__ = 0x0
                0x00000004                __major_subsystem_version__ = 0x4
                0x00000000                __minor_subsystem_version__ = 0x0
                0x00000003                __subsystem__ = 0x3
                0x00200000                __size_of_stack_reserve__ = 0x200000
                0x00001000                __size_of_stack_commit__ = 0x1000
                0x00100000                __size_of_heap_reserve__ = 0x100000
                0x00001000                __size_of_heap_commit__ = 0x1000
                0x00000000                __loader_flags__ = 0x0

.text           0x00401000     0x7000
                0x00401000                code = .
                0x00401000                _code = .
                0x00401000                __code = .
 *(.head_x86)
 .head_x86      0x00401000       0x35 head.o
                0x00401000                idle_entry_point
 *(.text)
 *fill*         0x00401035        0x3 00
 .text          0x00401038      0x780 main.o
                0x00401083                idleInit
                0x0040150b                TEST_PRINTF3
                0x004010a1                idleServices
                0x0040103d                driverInitialize
                0x0040110b                testtest_main
                0x004011a9                TEST_PRINTF
                0x00401038                idleLoop
                0x00401450                TEST_PRINTF2
                0x00401060                driverUninitialize
 .text          0x004017b8     0x2a00 stdio.o
                0x004021c5                printf_i2hex
                0x004023f7                putchar
                0x0040186d                stdio_fntos
                0x004023b3                sprintf
                0x00402c58                feof
                0x00402233                printf2
                0x00402d7e                stdioSetCursor
                0x00402b6d                ungetc
                0x00404174                rewind
                0x00401bd6                puts
                0x00402d36                fputc
                0x00402dd4                stdioGetCursorY
                0x00402e00                scanf
                0x00402a83                fputs
                0x00402c96                ferror
                0x004025d9                input
                0x00401b14                scroll
                0x00402af8                gets
                0x00402423                outbyte
                0x00402bd4                fileno
                0x0040297c                fflush
                0x00402a0e                fprintf
                0x00404132                stderr_printf
                0x00402741                stdio_system_call
                0x004019d2                fopen
                0x00402da8                stdioGetCursorX
                0x00402769                getchar
                0x004020d8                printf_atoi
                0x004017b8                stdio_atoi
                0x004030b0                kvprintf
                0x00404103                stdout_printf
                0x00402795                stdioInitialize
                0x00402bf7                fgetc
                0x0040196c                fclose
                0x00402cbc                fseek
                0x00404161                perror
                0x004025a6                _outbyte
                0x00402baa                ftell
                0x0040403f                printf
                0x004020bd                printf3
                0x00404077                vfprintf
 .text          0x004041b8      0xc60 stdlib.o
                0x004044ae                FreeHeap
                0x004041e0                rtGetHeapStart
                0x004047bb                free
                0x004041b8                stdlib_system_call
                0x004046e4                rand
                0x0040465b                stdlibInitMM
                0x004044b8                heapInit
                0x00404c55                __findenv
                0x004041f4                rtGetHeapPointer
                0x004041fe                rtGetAvailableHeap
                0x00404701                srand
                0x00404d55                atoi
                0x004046bb                libcInitRT
                0x00404208                heapSetLibcHeap
                0x00404739                stdlib_die
                0x00404d20                getenv
                0x0040477a                malloc
                0x0040449b                AllocateHeapEx
                0x004042af                AllocateHeap
                0x00404bcc                stdlib_strncmp
                0x0040470e                xmalloc
                0x00404c2a                exit
                0x004041ea                rtGetHeapEnd
                0x004047c0                system
 .text          0x00404e18        0x0 ctype.o
 .text          0x00404e18      0x430 string.o
                0x00405004                strcspn
                0x00404f67                strcat
                0x00404e18                strcmp
                0x00404fbe                bzero
                0x00405144                strtok_r
                0x00404efa                memcpy
                0x00404ed1                memoryZeroMemory
                0x00404f97                bcopy
                0x00404f37                strcpy
                0x004050a4                strspn
                0x00405219                strtok
                0x00404e73                strncmp
                0x00404fd9                strlen
 .text          0x00405248     0x2600 api.o
                0x00406ab8                APIShowCurrentProcessInfo
                0x004072eb                api_getchar
                0x004057ea                system11
                0x0040588f                refresh_buffer
                0x0040699a                APISetFocus
                0x0040584d                system14
                0x00405b76                apiInitBackground
                0x0040688a                APICreateWindow
                0x0040709a                apiBeginPaint
                0x00405b7b                MessageBox
                0x00406b36                APIreplace_window
                0x004070e1                apiGetSystemMetrics
                0x0040582c                system13
                0x00406f35                apiDown
                0x004065ec                dbProcedure
                0x00406c2f                apiExit
                0x00406fe9                enterCriticalSection
                0x00407226                gramadocore_init_execve
                0x00406e9d                apiFOpen
                0x00407819                apiShowWindow
                0x004056a0                system1
                0x00405724                system5
                0x00406d15                api_refresh_screen
                0x00407266                apiDialog
                0x00406da1                apiGetCursorY
                0x00405b1f                carrega_bitmap_16x16
                0x00407042                exitCriticalSection
                0x00405248                system_call
                0x00406f8f                apiUp
                0x00405270                apiSystem
                0x004071fa                api_get_window_with_text_input
                0x0040776f                apiGetWSScreenWindow
                0x0040586e                system15
                0x00406a4a                APISetActiveWindow
                0x00406df9                apiSetClientAreaRect
                0x00407728                apiDrawText
                0x00406b0d                APIredraw_window
                0x00406e4b                apiCreateThread
                0x00405abb                SetNextWindowProcedure
                0x004077c7                apiCreateTimer
                0x00406d75                apiGetCursorX
                0x00405a8b                chama_procedimento
                0x00406679                call_kernel
                0x00406916                APIRegisterWindow
                0x0040779b                apiGetWSMainWindow
                0x00406c8b                api_strncmp
                0x00406b88                APIminimize_window
                0x004056c1                system2
                0x004056e2                system3
                0x004060fa                DialogBox
                0x00407133                api_get_current_keyboard_responder
                0x00406e22                apiCreateProcess
                0x004069dc                APIGetFocus
                0x00405766                system7
                0x00405745                system6
                0x004070d7                apiDefDialog
                0x0040606d                mbProcedure
                0x00406958                APICloseWindow
                0x004076e7                apiSendMessage
                0x0040706e                initializeCriticalSection
                0x004057c9                system10
                0x00405aeb                set_cursor
                0x00406b5f                APImaximize_window
                0x0040725c                execve
                0x00406ce9                refresh_screen
                0x00407317                apiDisplayBMP
                0x004067f9                call_gui
                0x004057a8                system9
                0x00407230                fork
                0x0040710a                api_set_current_keyboard_responder
                0x00406ae4                APIresize_window
                0x004070a4                apiEndPaint
                0x00405b1a                put_char
                0x00405a26                print_string
                0x004077f0                apiGetSysTimeInfo
                0x00406dcd                apiGetClientAreaRect
                0x00405a5c                edit_box
                0x00406c5a                kill
                0x00406d1f                apiReboot
                0x00405b48                apiShutDown
                0x0040715f                api_set_current_mouse_responder
                0x00406bda                APIget_foregroung_window
                0x00405703                system4
                0x00406ed7                apiSaveFile
                0x00406d4b                apiSetCursor
                0x0040580b                system12
                0x00405a2b                vsync
                0x00405787                system8
                0x004071b4                api_set_window_with_text_input
                0x00406a8c                APIGetActiveWindow
                0x00406bb1                APIupdate_window
                0x00407188                api_get_current_mouse_responder
                0x00406c5f                dead_thread_collector
                0x00406e74                apiStartThread
                0x00406a08                APIKillFocus
                0x00406c06                APIset_foregroung_window
                0x004070ae                apiPutChar
                0x00408000                . = ALIGN (0x1000)
 *fill*         0x00407848      0x7b8 00

.rdata          0x00408000      0xf10
 .rdata         0x00408000      0x3a0 api.o
 .rdata         0x004083a0      0x100 ctype.o
                0x004083a0                _ctype
 .rdata         0x004084a0      0x480 stdlib.o
 .rdata         0x00408920      0x380 stdio.o
                0x00408b00                hex2ascii_data
 .rdata         0x00408ca0      0x270 main.o

.data           0x00408f10     0x10f0
                0x00408f10                data = .
                0x00408f10                _data = .
                0x00408f10                __data = .
 *(.data)
 .data          0x00408f10        0x0 main.o
 .data          0x00408f10        0x0 stdio.o
 .data          0x00408f10        0x0 stdlib.o
 .data          0x00408f10        0x0 ctype.o
 .data          0x00408f10        0x0 string.o
 .data          0x00408f10      0x440 api.o
                0x0040a000                . = ALIGN (0x1000)
 *fill*         0x00409350      0xcb0 00

.bss            0x0040a000     0xa720
                0x0040a000                bss = .
                0x0040a000                _bss = .
                0x0040a000                __bss = .
 *(.bss)
 .bss           0x0040a000        0x0 main.o
 .bss           0x0040a000       0x10 stdio.o
 .bss           0x0040a010     0x8010 stdlib.o
                0x0040a010                environ
 .bss           0x00412020        0x0 ctype.o
 .bss           0x00412020       0x10 string.o
 .bss           0x00412030       0x40 api.o
                0x00413000                . = ALIGN (0x1000)
 *fill*         0x00412070      0xf90 00
 COMMON         0x00413000      0xde0 main.o
                0x00413000                prompt
                0x00413400                idleStatus
                0x00413410                ClientAreaInfo
                0x00413420                BufferInfo
                0x00413430                driverInitialized
                0x00413440                stderr
                0x00413450                prompt_pos
                0x00413460                g_using_gui
                0x00413470                stdin
                0x00413480                rect
                0x00413490                prompt_max
                0x004134a0                CurrentWindow
                0x004134b0                prompt_err
                0x004138b0                current_semaphore
                0x004138c0                g_columns
                0x004138d0                stdout
                0x004138e0                Streams
                0x00413960                g_rows
                0x00413970                prompt_out
                0x00413d70                g_cursor_y
                0x00413d80                CursorInfo
                0x00413d90                prompt_status
                0x00413da0                g_char_attrib
                0x00413db0                ApplicationInfo
                0x00413dc0                idleError
                0x00413dd0                g_cursor_x
 COMMON         0x00413de0      0x900 stdlib.o
                0x00413de0                current_mmblock
                0x00413df0                mm_prev_pointer
                0x00413e00                heap_end
                0x00413e10                g_available_heap
                0x00413e20                heap_start
                0x00413e30                heapCount
                0x00413e40                HEAP_START
                0x00413e50                g_heap_pointer
                0x00413e60                heapList
                0x00414260                randseed
                0x00414270                HEAP_SIZE
                0x00414280                last_size
                0x00414290                last_valid
                0x004142a0                mmblockList
                0x004146a0                libcHeap
                0x004146b0                Heap
                0x004146c0                mmblockCount
                0x004146d0                HEAP_END
 COMMON         0x004146e0       0x40 api.o
                0x004146e0                messagebox_button2
                0x004146f0                dialogbox_button1
                0x00414700                messagebox_button1
                0x00414710                dialogbox_button2
                0x00414720                end = .
                0x00414720                _end = .
                0x00414720                __end = .
LOAD head.o
LOAD main.o
LOAD stdio.o
LOAD stdlib.o
LOAD ctype.o
LOAD string.o
LOAD api.o
OUTPUT(TCTYPE.BIN pe-i386)

.comment        0x00000000       0x20
 .comment       0x00000000       0x1f head.o
