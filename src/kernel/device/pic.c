#include "pic.h"

void pic_init(void) {
	// Master PIC
	out_port_b( PIC_MASTER_PORT1, 0x11);
	out_port_b( PIC_MASTER_PORT2, PIC_IRQSTARTVECTOR);
	out_port_b( PIC_MASTER_PORT2, 0x04);
	out_port_b( PIC_MASTER_PORT2, 0x01);

	// Slave PIC
	out_port_b( PIC_SLAVE_PORT1, 0x11);
	out_port_b( PIC_SLAVE_PORT2, PIC_IRQSTARTVECTOR + 8);
	out_port_b( PIC_SLAVE_PORT2, 0x02);
	out_port_b( PIC_SLAVE_PORT2, 0x01);

	// Enable All ports
	//mask_pic(0);
	// Disable All ports
	mask_pic(0xffff);
	printk("pic controller [ok?]\n");
}

void mask_pic( WORD data) {
	out_port_b( PIC_MASTER_PORT2, ( BYTE ) data);
	out_port_b( PIC_SLAVE_PORT2, ( BYTE ) (data >> 8));
}

