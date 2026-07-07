#include "include.h"

static void Init_All(void);

int main(void)
{
    Init_All();

    while (1)
    {
        Recv_Instruction();
        Detection_Cycle();
    }
}

static void Init_All(void)
{
    delay_init(168);
    uart_init1(115200);
    uart_init6(115200);

    InitAD7606();
    Init_AD9959();
    Match_AD9959(10000);
}
