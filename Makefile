# 
# TODO:
# Compile each dependency
# Compile main
# Unit tests
#
warn_flags = -O3 -g -Wall -Wextra -Werror
std_flags = std=c++17 -Irelative
prefix = include/

bin = main
objects = main.cpp

$(bin) : $(objects)
	$(compiler) $(objects) $(warn_flags) $(std_flags) 

clean:
	rm -r *.o $(bin)
