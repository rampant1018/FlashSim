# Copyright 2009, 2010 Brendan Tauras

# Makefile is part of FlashSim.

# FlashSim is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# any later version.

# FlashSim is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with FlashSim.  If not, see <http://www.gnu.org/licenses/>.

##############################################################################

# FlashSim makefile
# Brendan Tauras 2010-08-03
# 
# Use the "ssd" (default) make target for separate compilation to include
# with external programs, such as DiskSim or a custom drivers.
#
# Use the "test" make target to run the most basic test of your FTL scheme
# after adding your content to the FTL, wear-leveler, and garbage-collector
# classes.
#
# Use the "trace" make target to run a more involved test of your FTL scheme
# after adding your content to the FTL, wear-leveler, and garbage-collector
# classes.  It is suggested to test with the "test" make target first.

CC = /usr/bin/gcc
CFLAGS = -Wall -Wextra -g
CXX = /usr/bin/g++
CXXFLAGS = $(CFLAGS)
ELF0 = run_test
ELF1 = run_trace
ELF2 = run_test2
HDR = ssd.h
SRC = ssd_address.cpp ssd_block.cpp ssd_bus.cpp ssd_channel.cpp ssd_config.cpp ssd_controller.cpp ssd_die.cpp ssd_event.cpp ssd_ftl.cpp ssd_gc.cpp ssd_package.cpp ssd_page.cpp ssd_plane.cpp ssd_quicksort.cpp ssd_ram.cpp ssd_ssd.cpp ssd_wl.cpp
OBJ = ssd_address.o ssd_block.o ssd_bus.o ssd_channel.o ssd_config.o ssd_controller.o ssd_die.o ssd_event.o ssd_ftl.o ssd_gc.o ssd_package.o ssd_page.o ssd_plane.o ssd_quicksort.o ssd_ram.o ssd_ssd.o ssd_wl.o
LOG = log
PERMS = 660
EPERMS = 770

ssd: $(HDR) $(SRC)
	$(CXX) $(CXXFLAGS) -c $(SRC)
	-chmod $(PERMS) $(OBJ)
#script -c "$(CXX) $(CXXFLAGS) -c $(SRC)" $(LOG)
#-chmod $(PERMS) $(LOG) $(OBJ)

test: $(HDR) $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(ELF0) run_test.cpp $(OBJ)
	-chmod $(PERMS) $(OBJ)
	-chmod $(EPERMS) $(ELF0)
#script -c "$(CXX) $(CXXFLAGS) -o $(ELF0) run_test.cpp $(OBJ)" $(LOG)
#-chmod $(PERMS) $(LOG) $(OBJ)

test2: $(HDR) $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(ELF2) run_test2.cpp $(OBJ)
	-chmod $(PERMS) $(OBJ)
	-chmod $(EPERMS) $(ELF2)
#script -c "$(CXX) $(CXXFLAGS) -o $(ELF2) run_test2.cpp $(OBJ)" $(LOG)
#-chmod $(PERMS) $(LOG) $(OBJ)

trace: $(HDR) $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(ELF1) run_trace.cpp $(OBJ)
	-chmod $(PERMS) $(OBJ)
	-chmod $(EPERMS) $(ELF1)
#script -c "$(CXX) $(CXXFLAGS) -o $(ELF1) run_trace.cpp $(OBJ)" $(LOG)
#-chmod $(PERMS) $(LOG) $(OBJ)

clean:
	-rm -f $(OBJ) $(LOG) $(ELF0) $(ELF1) $(ELF2)

files:
	echo $(SRC) $(HDR)
