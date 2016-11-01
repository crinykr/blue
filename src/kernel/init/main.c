// - to do list
// * _do_fork 구현 (in linux 4.8.1, in ml setting, fork()->sys_fork->_do_fork())

// - History
// 16.8.2
// OS 전문가가 되어야지
// 16.10.9
// 많은 고민과 함께 현재 hobby 가 되어줄 blue를 다시 개발하기 시작
// 16.10.31
// blue 는 나에게 정권단련과도 같다.

// - blue Features
// * 커널 레벨 포크, execve(로더), 태스크 종료
// * IPC : 시그널 , 파이프 , 공유메모리
// * 가상파일시스템, proc파일 시스템등
// * 시스템콜을 위한 베이스
// * 포식스 프로세스 모델. 스레드,
// * 가상주소 구현 및 유저 레벨 구현

// - bugs 150329
// 4번 pit 작동안함...화면에 카운터값 안찍힘
// 인터럽트 분산 에러잡기
// 가변인자 다섯번째 부터 제대로 전달되지 않음... 호출 후 해석의 문제인듯

#include "types.h"
#include "keyboard.h"
#include "gdt.h"
#include "idt.h"
#include "interrupt.h"
#include "pic.h"
#include "console.h"
#include "task.h"
#include "pit.h"
#include "multiprocessor.h"
#include "syscall.h"
#include "builtinshell.h"
#include "util.h"
#include "sys.h"
#include "dynamic.h"
#include "printk.h"
#include "builtinshell.h"
#include "asm_util.h"
#include "syscall.h"
#include "task.h"
#include "scheduler.h"

extern void test_thread();

void main_bp(void) {
	machine_init();
	console_init();
	gdt_tss_init();
	idt_init();
	pic_init();
	pit_init(MSTOCOUNT(1), 1);
	keyboard_init();
	scheduler_init();
	smp_init();
	paging_init(128 * 1024);

	be_a_init();

	enable_intr();

	// run init program
	test_thread();

	_BARRIER_;
}

extern int ap_on_count;

void main_ap(void) {
	load_gdtr(&mygdt.gdtr);
	load_tr(GDT_TSSSEGMENT + (get_core_num() * sizeof(gdt_entry16_t)));
	load_idtr(&(myidt.idtr));

	enable_lapic();
	local_vector_table_init();

	lock_inc_dword(&ap_on_count); // criny : need lock

	disable_intr();

	while (1)
		;
}
