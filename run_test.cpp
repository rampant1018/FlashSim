/* Copyright 2009, 2010 Brendan Tauras */

/* run_test.cpp is part of FlashSim. */

/* FlashSim is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version. */

/* FlashSim is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details. */

/* You should have received a copy of the GNU General Public License
 * along with FlashSim.  If not, see <http://www.gnu.org/licenses/>. */

/****************************************************************************/

/* Basic test driver
 * Brendan Tauras 2009-11-02
 *
 * driver to create and run a very basic test of writes then reads */

#include "ssd.h"

#define SIZE 10

using namespace ssd;

int main()
{
	load_config();
	print_config(NULL);
   printf("Press ENTER to continue...");
   getchar();
   printf("\n");

	Ssd *ssd = new Ssd();

	double result;

	for (int i = 0; i < SIZE; i++)
	{
		/* event_arrive(event_type, logical_address, size, start_time) */
		result = ssd -> event_arrive(WRITE, i, 1, (double) 1);
		result = ssd -> event_arrive(WRITE, i+10240, 1, (double) 1);
	}
	for (int i = 0; i < SIZE; i++)
	{
		/* event_arrive(event_type, logical_address, size, start_time) */
		result = ssd -> event_arrive(READ, 1, 1, (double) 1);
		result = ssd -> event_arrive(READ, i, 1, (double) 1);
	}
	delete ssd;
	return 0;
}
