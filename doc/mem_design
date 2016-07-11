#
# Droid Sans Mono - Regular - size 9 - kate
#

0000000000000000 - 00007fffffffffff (=47 bits) user space, different per mm
hole caused by [48:63] sign extension
ffff800000000000 - ffff80ffffffffff (=40 bits) guard hole
ffff880000000000 - ffffc7ffffffffff (=64 TB) direct mapping of all phys. memory
ffffc80000000000 - ffffc8ffffffffff (=40 bits) hole
ffffc90000000000 - ffffe8ffffffffff (=45 bits) vmalloc/ioremap space
ffffe90000000000 - ffffe9ffffffffff (=40 bits) hole
ffffea0000000000 - ffffeaffffffffff (=40 bits) virtual memory map (1TB)
... unused hole ...
ffffffff80000000 - ffffffffa0000000 (=512 MB)  kernel text mapping, from phys 0
ffffffffa0000000 - ffffffffff5fffff (=1525 MB) module mapping space
ffffffffff600000 - ffffffffffdfffff (=8 MB) vsyscalls
ffffffffffe00000 - ffffffffffffffff (=2 MB) unused hole


TODO
set init section to purge late
gdt 디스크립터등 c언어 align 체크해보기

전체 가상공간 256TB = ---- FFFF FFFF FFFF
유저 가상 공간 ( 128TB ) = ---- 0000 0000 0000 ~ ---- 7fff ffff ffff
커널 가상 공간 ( 128TB ) = ---- 8000 0000 0000 ~ ---- ffff ffff ffff

################################################################################
# 이미지 바이너리_디자인
################################################################################

image offset		desc			vmem offset					phys offset
[0000 0000 ~ 0000 03ff] GRUB용 커널헤더 (2섹터)	-						-
[0000 0400 ~ 000? ????]	64bit kernel		ffff 8000 0010 0200 ~ ffff 8000 801? ????	0010 0000 ~ 001? ????


################################################################################
# 커널 가상 메모리 this here
################################################################################

vm						phys mem(total 8gb)				size	desc
0000 0000 0000 0000 ~ 0000 0001 ffff ffff	0000 0000 0000 0000 ~ 0000 0001 ffff ffff	8gb	dummy for early
0000 7000 0000 0000 ~ 0000 7??? ???? ????	0000 0000 0000 0000 ~ 0000 0001 ffff ffff	8gb	shm mem start
ffff 8000 0000 0000 ~ ffff 8001 ffff ffff	0000 0000 0000 0000 ~ 0000 0001 ffff ffff	8gb	kernel code,data,bss



################################################################################
# physical memory map
# KERNELSTART = 0xffff800000000000
# phys mem size = 128M
################################################################################

PHYSC ADDR			DESC
[0000 7C00 ~ ???? ????] 	grub2 ( ver 1.98 ) : 32bit jump mode
[0001 0000 ~ 0001 00??] 	AP jumper


/////////////////////////////////////////////////////////////////////////////			

[0010 0000 ~ 001? ????] 	Kernel64.bin(text+data+bss) + padding


[.text.header]			# Early GDTR, GDT TABLE
				
[.text.header]			# Early page table
				PML4 table 4KB [ entry 0, 256 ] // 0 is just for early
				page directory pointer table 4KB [ entry 0 ~ 7 ]
				page directory table 4KB [entry 0 ~ 511 ]
				page directory table 4KB [entry 0 ~ 511 ]
				page directory table 4KB [entry 0 ~ 511 ]
				page directory table 4KB [entry 0 ~ 511 ]
				page directory table 4KB [entry 0 ~ 511 ]
				page directory table 4KB [entry 0 ~ 511 ]
				page directory table 4KB [entry 0 ~ 511 ]
				page directory table 4KB [entry 0 ~ 511 ]
				
[.text.header]			# Idle Process 스택영역
				(8kb) - core0
				(8kb) - core1
				(8kb) - core2
				(8kb) - core3


[.data]			 	# GDTR 자료구조 16바이트
				# GDT
				NULL 디스크립터 8바이트
				커널 코드 디스크립터 8바이트
				커널 데이터 디스크립터 8바이트
				유저 데이터 디스크립터 8바이트
				유저 코드 디스크립터 8바이트
				TSS 디스크립터 16바이트 * 4(코어수)
				# TSS 104바이트 * 4(코어수)
				TSS[0].IST[0]
				TSS[1].IST[0]
				TSS[2].IST[0]
				TSS[3].IST[0]

					
[.data] 			# IDTR 자료구조 16바이트
				# IDT 100개 ( IST[0] 스택 사용 - 커널 유저 통합 인터럽트스택영역)
				IDT 디스크립터 16바이트 * 100
				
[.data] 		 	# IA-32e모드 IST-0 영역
				코어당 8KB * 4(코어수)
				core0 IST[0]
				core1 IST[0]
				core2 IST[0]
				core3 IST[0]
				
				? 인터럽트시 스택변화 정리해두기
/////////////////////////////////////////////////////////////////////////////



※ 동적 메모리 영역을 3GB 이하로 설정하는 이유는 3GB 이상의 메모리 영역에 비디오 레지스터나 특수한 목적으로 사용되는 프로세서 레지스터가 존재하기 때문임.
이러한 영역에 임의의 데이터를 쓰게 되면 어떠한 결과가 생길지 예측할 수 없으므로, 사고를 미연에 방지하고자 3GB 영역까지만 한정하였음.




################################################################################
#############################                     ##############################
#############################  BLUE ver0.00   ##############################
#############################                     ##############################
################################################################################



///////////////////////////////////////////////////////////////
cr3에 들어가는 페이지 디렉터리 주소도
디렉터리의 논리주소임

//
커널 이미지가 올라간 물리메모리 영역을 논리주소 "0xc000 0000~" 영역에 매핑시킨후
페이징 활성화
그리고 "0xc000 0000" 으로 점프


// 태스크 스위칭시마다
물리페이지를 할당받아서 프로세스이미지를 올리고,
프로세스의 가상메모리 공간을 새로 만들어서
할당받은 페이지를 연결시킨다.
그리고 cr3레지스터에 PGD주소를 적는다.







################################################################################
# 기능 구성
################################################################################
# [0000 7C00 ~ 0000 7DFF] (1섹터)
전체 커널 이미지 로드

# [0001 0000 ~ 0001 01FF] (1섹터)
-AP 시작 지점-
A20 게이트 on(AP제외)
cli, lgdt
캐쉬비활성화, 보호모드 전환
IA-32e모드 커널용 페이지 테이블 생성(AP제외)
IA-32e모드 커널을 2mb어드레스로 이동(AP제외)
<페이지테이블시작주소등록>
SYSCALL,SYSRET 명령활성화, IA-32e모드로 전환
캐쉬활성화(write-though), 페이징활성화, FPU활성화

# [0020 0000 ~ 005F FFFF] (3섹터)
<TSS디스크립터 포함시켜 GDT 재설정>
lgdt, ltr
<IDT 세팅, lidt>
TCB 풀, 스케쥴러 초기화, "최초태스크설정"
동적 메모리 초기화
PIT를 초기화
키보드 초기화
PIC 컨트롤러 초기화, PIC 모든 인터럽트핀 마스크해제
sti (interrupt enable )
파일 시스템을 초기화
***
MP 설정 테이블 분석
IA32_APIC BASE MSR 전역 APIC 비트 enable
로컬 APIC를 활성화
AP 초기화, 활성화
***
PIC 비활성화, cli
로컬 APIC의 로컬 벡터 테이블을 초기화
I/O APIC 초기화, sti
시스템 콜 관련된 MSR을 초기화
"유휴 태스크 생성"
"윈도우메니져 시작"

-AP 시작 지점-
lgdt, ltr
lidt
스케쥴러 초기화, "최초태스크설정"
로컬 APIC를 활성화
로컬 APIC의 로컬 벡터 테이블을 초기화
sti
시스템 콜 관련된 MSR을 초기화
"유휴테스크 시작"


################################################################################
# 인터럽트 관리
################################################################################
@ 최초설정
IRQ0 은 모든 코어에 전달킨다
나머지는 BSP에만 전달시키도록한다

@ 인터럽트 처리루틴
@@ IRQ1~15 인터럽트발생시
1) 인터럽트를 전달받은 코어에서
인터럽트 관리 테이블에서 현재 코어와, 전달받은 IRQ번호에 해당하는 부분의 숫자를 증가시킨다

2) 증가시킨 숫자가 일정 수준 이상이 되면
I/O APIC의 리다이렉션 테이블에 해당하는 엔트리부분을
해당하는 인터럽트를 가장 적게 처리한 코어로 전달되게 바꾼다
이때 가장 온도가 높은 코어는 제외 시킨다


@@ LOCAL APIC에서 온도관련 인터럽트가 발생햇을시
1) 인터럽트를 전달받은 코어에서 해당하는 부분에 온도를 기록한다


################################################################################
# 테스크 관리
################################################################################


태스크스위칭시점: 타이머 인터럽트가 발생했을시 또는 태스크에서 스케쥴을 요구했을때,




################################################################################
# TCB : Task Control Block
################################################################################
태스크의 종합정보가 들어있는 자료구조
// 태스크(프로세스 및 스레드)의 상태를 관리하는 자료구조
// FPU 콘텍스트가 추가되었기 때문에 자료구조의 크기가 16의 배수로 정렬되어야 함
typedef struct kTaskControlBlockStruct {

// 다음 데이터의 위치와 ID	
LISTLINK stLink; // 16바이트

// 데이터를 연결하는 자료구조
// 반드시 데이터의 가장 앞부분에 위치해야 함
typedef struct kListLinkStruct
{ 
// 다음 데이터의 어드레스와 데이터를 구분하기 위한 ID
void* pvNext;

// ID 상위 32bit : 고유 번호(gs_stTCBPoolManager.iAllocatedCount)
// 0을 제외한 값을 가짐. 0이라면 비어있는 TCB
// 하위 32bit : TCB 오프셋
QWORD qwID;
} LISTLINK;

	
// 플래그
QWORD qwFlags; // 8바이트

// 태스크의 우선 순위
#define TASK_FLAGS_HIGHEST		0
#define TASK_FLAGS_HIGH		1
#define TASK_FLAGS_MEDIUM		2
#define TASK_FLAGS_LOW		3
#define TASK_FLAGS_LOWEST		4
#define TASK_FLAGS_WAIT		0xFF

// 태스크의 플래그
#define TASK_FLAGS_ENDTASK			0x8000000000000000 // 태스크 종료를 위함
#define TASK_FLAGS_SYSTEM			0x4000000000000000
#define TASK_FLAGS_PROCESS			0x2000000000000000
#define TASK_FLAGS_THREAD			0x1000000000000000
#define TASK_FLAGS_IDLE			0x0800000000000000


// 프로세스 메모리 영역의 시작과 크기
void* pvMemoryAddress; // 8바이트
QWORD qwMemorySize; // 8바이트


//==========================================================================
// 이하 스레드 정보
//==========================================================================

// 자식 스레드의 위치와 ID
LISTLINK stThreadLink; // 16바이트

// 부모 프로세스의 ID
QWORD qwParentProcessID; // 8바이트

// FPU 콘텍스트는 16의 배수로 정렬되어야 하므로, 앞으로 추가할 데이터는 현재 라인
// 아래에 추가해야 함
QWORD vqwFPUContext[512 / 8]; // 8 * 64 = 512바이트

// 자식 스레드의 리스트
LIST stChildThreadList; // 20바이트
// 리스트를 관리하는 자료구조
typedef struct kListManagerStruct
{
    // 리스트 데이터의 수
    int iItemCount;

    // 리스트의 첫 번째와 마지막 데이터의 어드레스
    void* pvHeader;
    void* pvTail;
} LIST;

// 콘텍스트
CONTEXT stContext; // 192바이트
// 콘텍스트에 관련된 자료구조
typedef struct kContextStruct {
	QWORD vqRegister[TASK_REGISTERCOUNT];
} CONTEXT;
#define TASK_REGISTERCOUNT     ( 5 + 19 )

// 스택의 어드레스와 크기
void* pvStackAddress; // 8바이트
QWORD qwStackSize; // 8바이트

// FPU 사용 여부
BOOL bFPUUsed; // 1바이트

// TCB 전체를 16바이트 배수로 맞추기 위한 패딩
char vcPadding[11]; // 11바이트

} TCB; // 816바이트




################################################################################
# 동기화 처리가 필요한 시스템 자료 구조
################################################################################
키보드 디바이스 드라이버
동적메모리 할당
스케줄러





/////////////////////////////////////////////////////////////////////////////////////////////









