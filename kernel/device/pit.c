#include "pit.h"

// the number of PIT controller rising
volatile QWORD jiffies = 0;

extern int pit_sched_start;

void pit_handler() {
	if (get_core_num() == 0)
		jiffies++; // increase jiffies
}

// 1Ghz = 1,000,000,000 rise per 1 sec
// Set PIT to 1000hz
// PIT rise in 1ms(1/1000s)
// 1000000 machine code per one PIT time
void pit_init( WORD count, BOOL periodic) {
	out_port_b( PIT_PORT_CONTROL, PIT_COUNTER0_ONCE);

	if (periodic == TRUE) {
		out_port_b( PIT_PORT_CONTROL, PIT_COUNTER0_PERIODIC);
	}

	out_port_b( PIT_PORT_COUNTER0, count);
	out_port_b( PIT_PORT_COUNTER0, count >> 8);
}
