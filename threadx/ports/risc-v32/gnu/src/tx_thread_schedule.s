/**************************************************************************/
/*                                                                        */
/*       Copyright (c) Microsoft Corporation. All rights reserved.        */
/*                                                                        */
/*       This software is licensed under the Microsoft Software License   */
/*       Terms for Microsoft Azure RTOS. Full text of the license can be  */
/*       found in the LICENSE file at https://aka.ms/AzureRTOS_EULA       */
/*       and in the root directory of this software.                      */
/*                                                                        */
/**************************************************************************/


/**************************************************************************/
/**************************************************************************/
/**                                                                       */ 
/** ThreadX Component                                                     */ 
/**                                                                       */
/**   Thread                                                              */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


/* #define TX_SOURCE_CODE  */


/* Include necessary system files.  */

/*  #include "tx_api.h"
    #include "tx_thread.h"
    #include "tx_timer.h"  */


    .global      _tx_thread_execute_ptr
    .global      _tx_thread_current_ptr
    .global      _tx_timer_time_slice
/* #ifdef TX_ENABLE_EXECUTION_CHANGE_NOTIFY */
#if (defined(TX_ENABLE_EXECUTION_CHANGE_NOTIFY) || defined(TX_EXECUTION_PROFILE_ENABLE))
    .global      _tx_execution_thread_enter
#endif

    .section .text
/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    _tx_thread_schedule                                RISC-V32/IAR     */
/*                                                           6.1          */
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    William E. Lamie, Microsoft Corporation                             */ 
/*    Tom van Leeuwen, Technolution B.V.                                  */
/*                                                                        */ 
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This function waits for a thread control block pointer to appear in */ 
/*    the _tx_thread_execute_ptr variable.  Once a thread pointer appears */ 
/*    in the variable, the corresponding thread is resumed.               */ 
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */ 
/*    None                                                                */ 
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*    None                                                                */
/*                                                                        */ 
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    None                                                                */
/*                                                                        */ 
/*  CALLED BY                                                             */ 
/*                                                                        */ 
/*    _tx_initialize_kernel_enter          ThreadX entry function         */ 
/*    _tx_thread_system_return             Return to system from thread   */ 
/*    _tx_thread_context_restore           Restore thread's context       */ 
/*                                                                        */ 
/*  RELEASE HISTORY                                                       */ 
/*                                                                        */ 
/*    DATE              NAME                      DESCRIPTION             */ 
/*                                                                        */ 
/*  09-30-2020     William E. Lamie         Initial Version 6.1           */ 
/*                                                                        */ 
/**************************************************************************/ 
/* VOID   _tx_thread_schedule(VOID)
{  */
    .global _tx_thread_schedule
_tx_thread_schedule:

    /* Enable interrupts.  */
    csrsi   mstatus, 0x08                               /*  Enable interrupts */
    
    /* Wait for a thread to execute.  */
    /* do
    {  */

    la      t0, _tx_thread_execute_ptr                  /*  Pickup address of execute ptr */

/*
    csrr    t2, mepc
    la      t1,_tx_thread_schedule_loop
    csrw    mepc,t1
    mret */

_tx_thread_schedule_loop:
    lw      t1, 0(t0)                                   /*  Pickup next thread to execute */
    beqz    t1, _tx_thread_schedule_loop                /*  If NULL, wait for thread to execute */

    /* csrw    mepc,t2 */
    /* }
    while(_tx_thread_execute_ptr == TX_NULL)*/
    
    /* Yes! We have a thread to execute.  Lockout interrupts and
       transfer control to it.  */
    csrci   mstatus, 0x08                               /*  Lockout interrupts */

    /* Setup the current thread pointer.  */
    /* _tx_thread_current_ptr =  _tx_thread_execute_ptr */

    la      t0, _tx_thread_current_ptr                  /*  Pickup current thread pointer address */
    sw      t1, 0(t0)                                   /*  Set current thread pointer */

    /* Increment the run count for this thread.  */
    /* _tx_thread_current_ptr -> tx_thread_run_count++ */

    lw      t2, 4(t1)                                   /*  Pickup run count */
    lw      t3, 24(t1)                                  /*  Pickup time slice value */
    addi    t2, t2, 1                                   /*  Increment run count */
    sw      t2, 4(t1)                                   /*  Store new run count */

    /* Setup time-slice, if present.  */
    /* _tx_timer_time_slice =  _tx_thread_current_ptr -> tx_thread_time_slice */

    la      t2, _tx_timer_time_slice                    /*  Pickup time-slice variable address */

    /* Switch to the thread's stack.  */
    /* SP =  _tx_thread_execute_ptr -> tx_thread_stack_ptr */

    lw      sp, 8(t1)                                   /*  Switch to thread s stack */
    sw      t3, 0(t2)                                   /*  Store new time-slice */

/* #ifdef TX_ENABLE_EXECUTION_CHANGE_NOTIFY */
#if (defined(TX_ENABLE_EXECUTION_CHANGE_NOTIFY) || defined(TX_EXECUTION_PROFILE_ENABLE))
    sw      x1, 124(sp)
    call    _tx_execution_thread_enter                  /*  Call the thread execution enter function */
    lw      x1, 124(sp)
#endif

    /* Determine if an interrupt frame or a synchronous task suspension frame
       is present.  */

    lw      t2, 0(sp)                                   /*  Pickup stack type */
    beqz    t2, _tx_thread_synch_return                 /*  If 0, solicited thread return */

    /* Determine if floating point registers need to be recovered.  */


    /* Recover standard registers.  */

    lw      t0, 0x78(sp)                                /*  Recover mepc */
    csrw    mepc, t0                                    /*  Store mepc */
    li      t0, 0x1880                                  /*  Prepare MPIP */
    csrw    mstatus, t0                                 /*  Enable MPIP */

    lw      x1, 0x70(sp)                                /*  Recover RA */
    /* !! */
    lw      x4, 0x74(sp)                                /*  Recover tp */
    /* !! */
    lw      x5, 0x4C(sp)                                /*  Recover t0 */
    lw      x6, 0x48(sp)                                /*  Recover t1 */
    lw      x7, 0x44(sp)                                /*  Recover t2 */
    lw      x8, 0x30(sp)                                /*  Recover s0 */
    lw      x9, 0x2C(sp)                                /*  Recover s1 */
    lw      x10, 0x6C(sp)                               /*  Recover a0 */
    lw      x11, 0x68(sp)                               /*  Recover a1 */
    lw      x12, 0x64(sp)                               /*  Recover a2 */
    lw      x13, 0x60(sp)                               /*  Recover a3 */
    lw      x14, 0x5C(sp)                               /*  Recover a4 */
    lw      x15, 0x58(sp)                               /*  Recover a5 */
    lw      x16, 0x54(sp)                               /*  Recover a6 */
    lw      x17, 0x50(sp)                               /*  Recover a7 */
    lw      x18, 0x28(sp)                               /*  Recover s2 */
    lw      x19, 0x24(sp)                               /*  Recover s3 */
    lw      x20, 0x20(sp)                               /*  Recover s4 */
    lw      x21, 0x1C(sp)                               /*  Recover s5 */
    lw      x22, 0x18(sp)                               /*  Recover s6 */
    lw      x23, 0x14(sp)                               /*  Recover s7 */
    lw      x24, 0x10(sp)                               /*  Recover s8 */
    lw      x25, 0x0C(sp)                               /*  Recover s9 */
    lw      x26, 0x08(sp)                               /*  Recover s10 */
    lw      x27, 0x04(sp)                               /*  Recover s11 */
    lw      x28, 0x40(sp)                               /*  Recover t3 */
    lw      x29, 0x3C(sp)                               /*  Recover t4 */
    lw      x30, 0x38(sp)                               /*  Recover t5 */
    lw      x31, 0x34(sp)                               /*  Recover t6 */


    addi    sp, sp, 128                                 /*  Recover stack frame - without floating point registers */
    mret                                                /*  Return to point of interrupt */

_tx_thread_synch_return:

    /* Recover standard preserved registers.  */
    /* Recover standard registers.  */

    lw      x1, 0x34(sp)                                /*  Recover RA */
    lw      x8, 0x30(sp)                                /*  Recover s0 */
    lw      x9, 0x2C(sp)                                /*  Recover s1 */
    lw      x18, 0x28(sp)                               /*  Recover s2 */
    lw      x19, 0x24(sp)                               /*  Recover s3 */
    lw      x20, 0x20(sp)                               /*  Recover s4 */
    lw      x21, 0x1C(sp)                               /*  Recover s5 */
    lw      x22, 0x18(sp)                               /*  Recover s6 */
    lw      x23, 0x14(sp)                               /*  Recover s7 */
    lw      x24, 0x10(sp)                               /*  Recover s8 */
    lw      x25, 0x0C(sp)                               /*  Recover s9 */
    lw      x26, 0x08(sp)                               /*  Recover s10 */
    lw      x27, 0x04(sp)                               /*  Recover s11 */
    lw      t0, 0x38(sp)                                /*  Recover mstatus */
    csrw    mstatus, t0                                 /*  Store mstatus, enables interrupt */

    addi    sp, sp, 64                                  /*  Recover stack frame */
    ret                                                 /*  Return to thread */

/* }  */
    
