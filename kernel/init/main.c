// - Goal
// 1. 최종적으로 일단 init을 돌리야되
// 2. 파이썬을 올려보자 아니면 자바를...

// - bugs 0329
// 4번 pit 작동안함...화면에 카운터값 안찍힘

// 인터럽트 분산 에러잡기
// 가변인자 다섯번째 부터 제대로 전달되지 않음... 호출 후 해석의 문제인듯

// - To do list
// 1. 가상주소 구현
// 2. 포식스 프로세스 모델. 스레드,
// 3. 포크, execve(로더), 태스크 종료,

// 4. IPC : 시그널 , 파이프 , 공유메모리
// 5. 가상파일시스템, proc파일 시스템등

// 6. 나머지 시스템콜을 위한 베이스

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

//////////////////////////////////////////////////
//
// 15.09.07
//
// 1. 우선 완벽한 RTOS 를 완성
//    - 파일 시스템 포함
// 2. 가상 메모리 추가와 함께 일반OS로 바꾸기
//
//////////////////////////////////////////////////

int pit_sched_start;

__attribute__((always_inline))
inline int syscall_fork() {
	__asm__ __volatile__("movq $0x0, %rax");
	__asm__ __volatile__("int $99");
	return CANDY;
}

__attribute__((always_inline))
inline int syscall_schedule() {
	__asm__ __volatile__("movq $0x1, %rax");
	__asm__ __volatile__("int $99");
	return CANDY;
}

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

	pit_sched_start = 1;

	enable_intr();

	// temp routine
	{
		static int global_test_i;

		if (syscall_fork() > 0) {
			while (1) {
				printk("{%}} parent : name(%s) core(%d))!@#$\n", global_test_i++,
						get_cur_task(get_core_num())->name,
						get_cur_task(get_core_num())->id);
				//syscall_schedule();
			}
		} else {
			//syacall_execve();

			while (1) {
				printk("{%d}} child : name(%s) core(%d)\n", global_test_i++,
						get_cur_task(get_core_num())->name,
						get_cur_task(get_core_num())->id);
				//syscall_schedule();
			}
		}

		//builtin_shell(); // execve
	}
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
		; //schedule();
}
