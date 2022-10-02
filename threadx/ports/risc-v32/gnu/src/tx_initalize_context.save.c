

VOID _tx_thread_context_save_user(VOID)
{
    if (_tx_thread_system_state++)
    {
        /*!< 当前有中断嵌套 */
        asm("sw      x4, 0x74(sp)  /*  Store x4 */ \n\t"
            "sw      x5, 0x4c(sp)  /*  Store t0 */ \n\t"
            "sw      x6, 0x48(sp)  /*  Store t1 */ \n\t"
            "sw      x7, 0x44(sp)  /*  Store t2 */ \n\t"
            "sw      x8, 0x30(sp)  /*  Store s0 */ \n\t"
            "sw      x9, 0x2c(sp)  /*  Store s1 */ \n\t"
            "sw      x10, 0x6C(sp) /*  Store a0 */ \n\t"
            "sw      x11, 0x68(sp) /*  Store a1 */ \n\t"
            "sw      x12, 0x64(sp) /*  Store a2 */ \n\t"
            "sw      x13, 0x60(sp) /*  Store a3 */ \n\t"
            "sw      x14, 0x5C(sp) /*  Store a4 */ \n\t"
            "sw      x15, 0x58(sp) /*  Store a5 */ \n\t"
            "sw      x16, 0x54(sp) /*  Store a6 */ \n\t"
            "sw      x17, 0x50(sp) /*  Store a7 */ \n\t"

            "sw      x18, 0x28(sp) /*  Store s2 */ \n\t"
            "sw      x19, 0x24(sp) /*  Store s3 */ \n\t"
            "sw      x20, 0x20(sp) /*  Store s4 */ \n\t"
            "sw      x21, 0x1c(sp) /*  Store s5 */ \n\t"
            "sw      x22, 0x18(sp) /*  Store s6 */ \n\t"
            "sw      x23, 0x14(sp) /*  Store s7 */ \n\t"
            "sw      x24, 0x10(sp) /*  Store s8 */ \n\t"
            "sw      x25, 0x0c(sp) /*  Store s9 */ \n\t"
            "sw      x26, 0x08(sp) /*  Store s10 */ \n\t"
            "sw      x27, 0x04(sp) /*  Store s11 */ \n\t"

            "sw      x28, 0x40(sp) /*  Store t3 */ \n\t"
            "sw      x29, 0x3C(sp) /*  Store t4 */ \n\t"
            "sw      x30, 0x38(sp) /*  Store t5 */ \n\t"
            "sw      x31, 0x34(sp) /*  Store t6 */ \n\t"
            "csrr    t0, mepc      /*  Load exception program counter */ \n\t"
            "sw      t0, 0x78(sp)  /*  Save it on the stack */ \n\t");
        return;
    }
    else if (_tx_thread_current_ptr)
    {
        /*!< 没有中断嵌套 且当前不是idle线程在执行 保存当前在执行的线程的CPU寄存器 */
        asm("sw      x4, 0x74(sp)  /*  Store x4 */ \n\t"
            "sw      x5, 0x4c(sp)  /*  Store t0 */ \n\t"
            "sw      x6, 0x48(sp)  /*  Store t1 */ \n\t"
            "sw      x7, 0x44(sp)  /*  Store t2 */ \n\t"
            "sw      x8, 0x30(sp)  /*  Store s0 */ \n\t"
            "sw      x10, 0x6C(sp) /*  Store a0 */ \n\t"
            "sw      x11, 0x68(sp) /*  Store a1 */ \n\t"
            "sw      x12, 0x64(sp) /*  Store a2 */ \n\t"
            "sw      x13, 0x60(sp) /*  Store a3 */ \n\t"
            "sw      x14, 0x5C(sp) /*  Store a4 */ \n\t"
            "sw      x15, 0x58(sp) /*  Store a5 */ \n\t"
            "sw      x16, 0x54(sp) /*  Store a6 */ \n\t"
            "sw      x17, 0x50(sp) /*  Store a7 */ \n\t"
            "sw      x28, 0x40(sp) /*  Store t3 */ \n\t"
            "sw      x29, 0x3C(sp) /*  Store t4 */ \n\t"
            "sw      x30, 0x38(sp) /*  Store t5 */ \n\t"
            "sw      x31, 0x34(sp) /*  Store t6 */ \n\t"
            "csrr    t0, mepc      /*  Load exception program counter */ \n\t"
            "sw      t0, 0x78(sp)  /*  Save it on the stack */ \n\t");
        return;
    }
    else
    {
        /*!< 没有中断嵌套 且当前是idle线程在执行 */
        return;
    }
}

VOID _tx_thread_context_restore_user(VOID)
{
    /*!< 关闭全局中断 */
    asm("csrci   mstatus, 0x08  /*  Disable interrupts */");
    if (--_tx_thread_system_state)
    {
        /*!< 中断嵌套 */
        asm("lw      t0, 0x78(sp)                                /*  Recover mepc */ \n\t"
            "csrw    mepc, t0                                    /*  Setup mepc */   \n\t"
            "li      t0, 0x1880                                  /*  Prepare MPIP */ \n\t"
            "csrw    mstatus, t0                                 /*  Enable MPIP */  \n\t"
            "lw      x1, 0x70(sp)                                /*  Recover RA */ \n\t"
            "lw      x4, 0x74(sp)                                /*  Recover tp */ \n\t"
            "lw      x5, 0x4C(sp)                                /*  Recover t0 */ \n\t"
            "lw      x6, 0x48(sp)                                /*  Recover t1 */ \n\t"
            "lw      x7, 0x44(sp)                                /*  Recover t2 */ \n\t"
            "lw      x8, 0x30(sp)                                /*  Recover s0 */ \n\t"
            "lw      x10, 0x6C(sp)                               /*  Recover a0 */ \n\t"
            "lw      x11, 0x68(sp)                               /*  Recover a1 */ \n\t"
            "lw      x12, 0x64(sp)                               /*  Recover a2 */ \n\t"
            "lw      x13, 0x60(sp)                               /*  Recover a3 */ \n\t"
            "lw      x14, 0x5C(sp)                               /*  Recover a4 */ \n\t"
            "lw      x15, 0x58(sp)                               /*  Recover a5 */ \n\t"
            "lw      x16, 0x54(sp)                               /*  Recover a6 */ \n\t"
            "lw      x17, 0x50(sp)                               /*  Recover a7 */ \n\t"

            "lw      x18, 0x28(sp)                               /*  Recover s2 */ \n\t"
            "lw      x19, 0x24(sp)                               /*  Recover s3 */ \n\t"
            "lw      x20, 0x20(sp)                               /*  Recover s4 */ \n\t"
            "lw      x21, 0x1c(sp)                               /*  Recover s5 */ \n\t"
            "lw      x22, 0x18(sp)                               /*  Recover s6 */ \n\t"
            "lw      x23, 0x14(sp)                               /*  Recover s7 */ \n\t"
            "lw      x24, 0x10(sp)                               /*  Recover s8 */ \n\t"
            "lw      x25, 0x0c(sp)                               /*  Recover s9 */ \n\t"
            "lw      x26, 0x08(sp)                               /*  Recover s10 */ \n\t"
            "lw      x27, 0x04(sp)                               /*  Recover s11 */ \n\t"

            "lw      x28, 0x40(sp)                               /*  Recover t3 */ \n\t"
            "lw      x29, 0x3C(sp)                               /*  Recover t4 */ \n\t"
            "lw      x30, 0x38(sp)                               /*  Recover t5 */ \n\t"
            "lw      x31, 0x34(sp)                               /*  Recover t6 */ \n\t"
            "addi    sp, sp, 128                                 /*  Recover stack frame - without floating point enabled */ \n\t"
            "mret                                                /*  Return to point of interrupt */ \n\t");
    }
    else if (((_tx_thread_current_ptr) &&
                  (_tx_thread_current_ptr == _tx_thread_execute_ptr) ||
              (_tx_thread_preempt_disable)))
    {
        /* 正在执行的线程没有被抢占 继续被中断的线程  */
        /*!< 当前应该是需要操作线程栈 */
            lw      t0, 0x78(sp)                                /* Recover mepc */
            csrw    mepc, t0                                    /* Setup mepc */
            li      t0, 0x1880                                  /* Prepare MPIP */
            csrw    mstatus, t0                                 /* Enable MPIP */

            lw      x1, 0x70(sp)                                /* Recover RA */
            lw      x4, 0x74(sp)                                /* Recover tp */
            lw      x5, 0x4C(sp)                                /* Recover t0 */
            lw      x6, 0x48(sp)                                /* Recover t1 */
            lw      x7, 0x44(sp)                                /* Recover t2 */
            lw      x8, 0x30(sp)                                /* Recover s0 */
            lw      x10, 0x6C(sp)                               /* Recover a0 */
            lw      x11, 0x68(sp)                               /* Recover a1 */
            lw      x12, 0x64(sp)                               /* Recover a2 */
            lw      x13, 0x60(sp)                               /* Recover a3 */
            lw      x14, 0x5C(sp)                               /* Recover a4 */
            lw      x15, 0x58(sp)                               /* Recover a5 */
            lw      x16, 0x54(sp)                               /* Recover a6 */
            lw      x17, 0x50(sp)                               /* Recover a7 */
            lw      x28, 0x40(sp)                               /* Recover t3 */
            lw      x29, 0x3C(sp)                               /* Recover t4 */
            lw      x30, 0x38(sp)                               /* Recover t5 */
            lw      x31, 0x34(sp)                               /* Recover t6 */

            addi    sp, sp, 128                                 /* Recover stack frame - without floating point enabled */

            mret                                                /* Return to point of interrupt */

    }
    else
    {
        /*!< _tx_thread_preempt_disable == 0 */
        if (_tx_thread_current_ptr == NULL && (_tx_thread_current_ptr != _tx_thread_execute_ptr))
        {
            /*!< schedule */
        }
        else if ((_tx_thread_current_ptr) && (_tx_thread_current_ptr != _tx_thread_execute_ptr))
        {
            /* 当前执行的线程被抢占或者轮转出去 需要保存当前线程的上下文 恢复即将执行线程的上下文 */
            lw      t0, 8(t1)                                   /*  Pickup thread's stack pointer */
            ori     t3, x0, 1                                   /*  Build interrupt stack type */
            sw      t3, 0(t0)                                   /*  Store stack type */

            /* Store standard preserved registers.  */

            sw      x9, 0x2C(t0)                                /*  Store s1 */
            sw      x18, 0x28(t0)                               /*  Store s2 */
            sw      x19, 0x24(t0)                               /*  Store s3 */
            sw      x20, 0x20(t0)                               /*  Store s4 */
            sw      x21, 0x1C(t0)                               /*  Store s5 */
            sw      x22, 0x18(t0)                               /*  Store s6 */
            sw      x23, 0x14(t0)                               /*  Store s7 */
            sw      x24, 0x10(t0)                               /*  Store s8 */
            sw      x25, 0x0C(t0)                               /*  Store s9 */
            sw      x26, 0x08(t0)                               /*  Store s10 */
            sw      x27, 0x04(t0)                               /*  Store s11 */
                        /*  Note: s0 is already stored! */   

            if (_tx_timer_time_slice)
            {

            }
            /*!< schedule */
        }
        else if ((_tx_thread_current_ptr == NULL) && (_tx_thread_current_ptr == _tx_thread_execute_ptr))
        {
            /*!< schedule */
        }
        else
        {
        }
    }
}