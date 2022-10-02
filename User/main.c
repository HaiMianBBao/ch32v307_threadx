#include "debug.h"
#include "tx_api.h"

void GPIO_Toggle_INIT(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
}

void EXTI0_INT_INIT(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    EXTI_InitTypeDef EXTI_InitStructure = {0};
    NVIC_InitTypeDef NVIC_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* GPIOA ----> EXTI_Line0 */
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);
    EXTI_InitStructure.EXTI_Line = EXTI_Line0;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    tx_kernel_enter();
}

static TX_THREAD app_init_thread;
static uint32_t app_init_thread_stack[256];
static TX_THREAD thread_1;
static uint32_t stack_1[256];

static TX_THREAD thread_2;
static uint32_t stack_2[256];

TX_SEMAPHORE app_init_sem;

void app_init_thread_entry(ULONG input)
{
    tx_semaphore_get(&app_init_sem, TX_WAIT_FOREVER);
    USART_Printf_Init(115200);
    printf("SystemClk:%d\r\n", SystemCoreClock);

    GPIO_Toggle_INIT();
    EXTI0_INT_INIT();

    tx_thread_sleep(1000);
    tx_semaphore_put(&app_init_sem);
}
void thread_1_entry(ULONG input)
{
    tx_semaphore_get(&app_init_sem, TX_WAIT_FOREVER);
    tx_semaphore_delete(&app_init_sem);
    tx_thread_resume(&thread_2);
    while (1)
    {
        printf("thread_1_entry \r\n");
        tx_thread_sleep(2000);
    }
}

void thread_2_entry(ULONG input)
{
    while (1)
    {
        printf("thread_2_entry \r\n");
        tx_thread_sleep(200);
    }
}

void tx_application_define(void *first_unused_memory)
{
    (void)first_unused_memory;
    tx_thread_create(&app_init_thread,
                     "app_init_thread",
                     app_init_thread_entry,
                     0,
                     app_init_thread_stack,
                     sizeof(app_init_thread_stack),
                     0,
                     0,
                     TX_NO_TIME_SLICE,
                     TX_AUTO_START);
    tx_semaphore_create(&app_init_sem,
                        "app init sem",
                        1);
    tx_thread_create(&thread_1,
                     "thread 1",
                     thread_1_entry,
                     0,
                     stack_1,
                     sizeof(stack_1),
                     1,
                     1,
                     TX_NO_TIME_SLICE,
                     TX_AUTO_START);
    tx_thread_create(&thread_2,
                     "thread 1",
                     thread_2_entry,
                     0,
                     stack_2,
                     sizeof(stack_2),
                     5,
                     5,
                     TX_NO_TIME_SLICE,
                     TX_DONT_START);
}
