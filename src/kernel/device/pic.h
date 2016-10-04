#ifndef PIC_H_
#define PIC_H_

#include "types.h"


#define PIC_MASTER_PORT1    0x20
#define PIC_MASTER_PORT2    0x21
#define PIC_SLAVE_PORT1     0xA0
#define PIC_SLAVE_PORT2     0xA1

#define PIC_IRQSTARTVECTOR  0x20

void pic_init(void);
void mask_pic(WORD data);

#endif /* PIC_H_ */
