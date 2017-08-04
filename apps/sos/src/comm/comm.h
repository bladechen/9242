#ifndef _COMM_H_
#define _COMM_H_
#include <autoconf.h>
#include <sel4/bootinfo.h>

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <sel4/types.h>
#include <nfs/nfs.h>


#include <cspace/cspace.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <sel4/types.h>
#include <cspace/cspace.h>
#include <sys/panic.h>
#include <sel4/sel4.h>
/* To differencient between async and and sync IPC, we assign a
 * badge to the async endpoint. The badge that we receive will
 * be the bitwise 'OR' of the async endpoint badge and the badges
 * of all pending notifications. */
#define IRQ_EP_BADGE         (1 << (seL4_BadgeBits - 1))
#define IRQ_BADGE_NETWORK (1 << 0)
#define IRQ_EPIT1_BADGE (1 << 2)
#define IRQ_GPT_BADGE   (1 << 1)



static inline seL4_CPtr badge_irq_ep(seL4_CPtr ep, seL4_Word badge) {

    seL4_CPtr badged_cap = cspace_mint_cap(cur_cspace, cur_cspace, ep, seL4_AllRights, seL4_CapData_Badge_new(badge | IRQ_EP_BADGE));
    conditional_panic(!badged_cap, "Failed to allocate badged cap");
    return badged_cap;
}


// static inline void


static inline seL4_CPtr
enable_irq(int irq, seL4_CPtr aep) {
    seL4_CPtr cap;
    int err;
    /* Create an IRQ handler */
    cap = cspace_irq_control_get_cap(cur_cspace, seL4_CapIRQControl, irq);
    conditional_panic(!cap, "Failed to acquire and IRQ control cap");
    /* Assign to an end point */
    err = seL4_IRQHandler_SetEndpoint(cap, aep);
    conditional_panic(err, "Failed to set interrupt endpoint");
    /* Ack the handler before continuing */
    err = seL4_IRQHandler_Ack(cap);
    conditional_panic(err, "Failure to acknowledge pending interrupts");
    return cap;
}
#endif
