#include "auxiliary/aux_hw/aux_hw.h"
#include "auxiliary/aux_rtos/aux_rtos.h"

int main(void)
{
	init_hw();
	init_rtos();
	return 0;
}
