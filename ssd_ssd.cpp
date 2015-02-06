/* Copyright 2009, 2010 Brendan Tauras */

/* ssd_ssd.cpp is part of FlashSim. */

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

/* Ssd class
 * Brendan Tauras 2009-11-03
 *
 * The SSD is the single main object that will be created to simulate a real
 * SSD.  Creating a SSD causes all other objects in the SSD to be created.  The
 * event_arrive method is where events will arrive from DiskSim. */

#include <cmath>
#include <new>
#include <assert.h>
#include <stdio.h>
#include "ssd.h"

using namespace ssd;

/* use caution when editing the initialization list - initialization actually
 * occurs in the order of declaration in the class definition and not in the
 * order listed here */
Ssd::Ssd(uint ssd_size): 
	size(ssd_size), 
	controller(*this), 
	ram(RAM_READ_DELAY, RAM_WRITE_DELAY), 
	bus(size, BUS_CTRL_DELAY, BUS_DATA_DELAY, BUS_TABLE_SIZE, BUS_MAX_CONNECT), 

	/* use a const pointer (Package * const data) to use as an array
	 * but like a reference, we cannot reseat the pointer */
	data((Package *) malloc(ssd_size * sizeof(Package))), 

	/* set erases remaining to BLOCK_ERASES to match Block constructor args 
	 *	in Plane class
	 * this is the cheap implementation but can change to pass through classes */
	erases_remaining(BLOCK_ERASES), 

	/* assume all Planes are same so first one can start as least worn */
	least_worn(0), 

	/* assume hardware created at time 0 and had an implied free erasure */
	last_erase_time(0.0)
{
	uint i;

	/* new cannot initialize an array with constructor args so
	 *		malloc the array
	 *		then use placement new to call the constructor for each element
	 * chose an array over container class so we don't have to rely on anything
	 * 	i.e. STL's std::vector */
	/* array allocated in initializer list:
	 * data = (Package *) malloc(ssd_size * sizeof(Package)); */
	if(data == NULL){
		fprintf(stderr, "Ssd error: %s: constructor unable to allocate Package data\n", __func__);
		exit(MEM_ERR);
	}
	for (i = 0; i < ssd_size; i++)
	{
		(void) new (&data[i]) Package(*this, bus.get_channel(i), PACKAGE_SIZE);
	}
	
	return;
}

Ssd::~Ssd(void)
{
	uint i;
	/* explicitly call destructors and use free
	 * since we used malloc and placement new */
	for (i = 0; i < size; i++)
	{
		data[i].~Package();
	}
	free(data);
	return;
}

/* This is the function that will be called by DiskSim
 * Provide the event (request) type (see enum in ssd.h),
 * 	logical_address (page number), size of request in pages, and the start
 * 	time (arrive time) of the request
 * The SSD will process the request and return the time taken to process the
 * 	request.  Remember to use the same time units as in the config file. */
double Ssd::event_arrive(enum event_type type, ulong logical_address, uint size, double start_time)
{
	assert(start_time >= 0.0);
	assert((long long int) logical_address <= (long long int) SSD_SIZE * PACKAGE_SIZE * DIE_SIZE * PLANE_SIZE * BLOCK_SIZE);

	/* allocate the event and address dynamically so that the allocator can
	 * handle efficiency issues for us */
	Event *event = NULL;
	/* STUB ONLY */
	#if 0
	Address *address = NULL;
	#endif
	/* END STUB ONLY */

	if((event = new Event(type, logical_address, size, start_time)) == NULL)
	{
		fprintf(stderr, "Ssd error: %s: could not allocate Event\n", __func__);
		exit(MEM_ERR);
	}

	/* REAL SSD ONLY */
	if(controller.event_arrive(*event) != SUCCESS)
	{
		fprintf(stderr, "Ssd error: %s: request failed:\n", __func__);
		event -> print(stderr);
	}
	/* END REAL SSD ONLY */

	/* STUB ONLY
	 * real SSD will let the FTL determine the physical address */
	#if 0
	if((address = new Address()) == NULL)
	{
		fprintf(stderr, "Ssd error: %s: could not allocate Address\n", __func__);
		exit(MEM_ERR);
	}

	address -> page = logical_address % BLOCK_SIZE;
	logical_address /= BLOCK_SIZE;
	address -> block = logical_address % PLANE_SIZE;
	logical_address /= PLANE_SIZE;
	address -> plane = logical_address % DIE_SIZE;
	logical_address /= DIE_SIZE;
	address -> die = logical_address % PACKAGE_SIZE;
	logical_address /= PACKAGE_SIZE;
	address -> package = logical_address % SSD_SIZE;
	logical_address /= SSD_SIZE;
	address -> valid = PAGE;

	event -> set_address(address);

	/* the bus locking should be done in the controller in the real SSD */
	if(type == READ){
		if(bus.lock(address -> package, start_time, BUS_CTRL_DELAY, *event) != SUCCESS){
			fprintf(stderr, "Ssd error: %s: locking bus channel %u for read command failed:\n", __func__, address -> package);
		} else{
			if(data[address -> package].read(*event) != SUCCESS)
				fprintf(stderr, "Ssd error: %s: read request failed:\n", __func__);
			else{
				if(bus.lock(address -> package, start_time, BUS_CTRL_DELAY + BUS_DATA_DELAY, *event) != SUCCESS)
					fprintf(stderr, "Ssd error: %s: locking bus channel %u for read data failed:\n", __func__, address -> package);
			}
		}

	} else if(type == WRITE){
		if(bus.lock(address -> package, start_time, BUS_CTRL_DELAY + BUS_DATA_DELAY, *event) != SUCCESS){
			fprintf(stderr, "Ssd error: %s: locking bus channel %u for write data failed:\n", __func__, address -> package);
		} else{
			if(data[address -> package].write(*event) != SUCCESS)
				fprintf(stderr, "Ssd error: %s: write request failed:\n", __func__);
		}

	} else
		fprintf(stderr, "Ssd error: %s: incoming request was not of type read or write\n", __func__);
	event -> print();
	delete address;
	#endif
	/* END STUB ONLY */

	/* use start_time as a temporary for returning time taken to service event */
	start_time = event -> get_time_taken();
	delete event;
	return start_time;
}

/* read write erase and merge should only pass on the event
 * 	the Controller should lock the bus channels
 * technically the Package is conceptual, but we keep track of statistics
 * 	and addresses with Packages, so send Events through Package but do not 
 * 	have Package do anything but update its statistics and pass on to Die */
enum status Ssd::read(Event &event)
{
	assert(data != NULL && event.get_address().package < size && event.get_address().valid >= PACKAGE);
	return data[event.get_address().package].read(event);
}

enum status Ssd::write(Event &event)
{
	assert(data != NULL && event.get_address().package < size && event.get_address().valid >= PACKAGE);
	return data[event.get_address().package].write(event);
}


enum status Ssd::erase(Event &event)
{
	assert(data != NULL && event.get_address().package < size && event.get_address().valid >= PACKAGE);
	enum status status = data[event.get_address().package].erase(event);

	/* update values if no errors */
	if (status == SUCCESS)
		update_wear_stats(event.get_address());
	return status;
}

enum status Ssd::merge(Event &event)
{
	assert(data != NULL && event.get_address().package < size && event.get_address().valid >= PACKAGE);
	return data[event.get_address().package].merge(event);
}

/* add up the erases remaining for all packages in the ssd*/
ssd::ulong Ssd::get_erases_remaining(const Address &address) const
{
	assert (data != NULL);
	
	if (address.package < size && address.valid >= PACKAGE)
		return data[address.package].get_erases_remaining(address);
	else return erases_remaining;
}

void Ssd::update_wear_stats(const Address &address)
{
	assert(data != NULL);
	uint i;
	uint max_index = 0;
	ulong max = data[0].get_erases_remaining(address);
	for(i = 1; i < size; i++)
		if(data[i].get_erases_remaining(address) > max)
			max_index = i;
	least_worn = max_index;
	erases_remaining = max;
	last_erase_time = data[max_index].get_last_erase_time(address);
	return;
}

void Ssd::get_least_worn(Address &address) const
{
	assert(data != NULL && least_worn < size);
	address.package = least_worn;
	address.valid = PACKAGE;
	data[least_worn].get_least_worn(address);
	return;
}

double Ssd::get_last_erase_time(const Address &address) const
{
	assert(data != NULL);
	if(address.package < size && address.valid >= PACKAGE)
		return data[address.package].get_last_erase_time(address);
	else
		return last_erase_time;
}

enum page_state Ssd::get_state(const Address &address) const
{
	assert(data != NULL);
	assert(address.package < size && address.valid >= PACKAGE);
	return data[address.package].get_state(address);
}

void Ssd::get_free_page(Address &address) const
{
	assert(address.package < size && address.valid >= PACKAGE);
	data[address.package].get_free_page(address);
	return;
}

ssd::uint Ssd::get_num_free(const Address &address) const
{  
	return 0;
/* 	return data[address.package].get_num_free(address); */
}

ssd::uint Ssd::get_num_valid(const Address &address) const
{  
	assert(address.valid >= PACKAGE);
	return data[address.package].get_num_valid(address);
}

