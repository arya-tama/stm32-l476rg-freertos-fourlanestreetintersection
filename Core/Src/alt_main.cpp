/*
 * alt_main.cpp
 *
 *  Created on: Feb 25, 2026
 *      Author: aryat
 */

#include "alt_main.h"
#include "cmsis_os.h"

int alt_main()
{
	/* Initialization */

	/* Start scheduler */
	osKernelStart();

	/* We should never get here as control is now taken by the scheduler */

	while (1)
	{
		/* Super loop */
	}
}


