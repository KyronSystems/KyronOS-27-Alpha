CXX ?= g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Werror -I../include

ksfs_tests: ksfs_tests.o ../src/fs/ksfs.o
	$(CXX) $^ -o $@

ksfs_tests.o: ksfs_tests.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

../src/fs/ksfs.o: ../src/fs/ksfs.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f *.o ksfs_tests
