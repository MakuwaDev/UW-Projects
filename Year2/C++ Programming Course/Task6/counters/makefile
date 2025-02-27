CXX = /opt/llvm/19.1.4/bin/clang++
CXXFLAGS = -std=c++20 -Wall -Wextra -O2
MODULE_FLAGS = -Wno-experimental-header-units -fprebuilt-module-path=. --precompile
HEADERS = iostream unordered_map queue cstdint sstream memory vector regex array

.PHONY: all clean

all: counters

%.pch:
	$(CXX) $(CXXFLAGS) -Wno-experimental-header-units -Wno-pragma-system-header-outside-header -Wno-inline-namespace-reopened-noninline -xc++-system-header --precompile $* -o $@

all-pch: $(HEADERS:%=%.pch)

counters.pcm: cstdint.pch queue.pch iostream.pch counters.cppm
	$(CXX) $(CXXFLAGS) $(MODULE_FLAGS) -fmodule-file=cstdint.pch -fmodule-file=queue.pch -fmodule-file=iostream.pch counters.cppm -o $@

counter_collection.pcm: memory.pch unordered_map.pch counters.pcm
	$(CXX) $(CXXFLAGS) $(MODULE_FLAGS) -fmodule-file=memory.pch -fmodule-file=unordered_map.pch -fmodule-file=counters=counters.pcm counter_collection.cppm -o $@

main.pcm: iostream.pch cstdint.pch sstream.pch memory.pch queue.pch regex.pch array.pch counter_collection.pcm
	$(CXX) $(CXXFLAGS) $(MODULE_FLAGS) -fmodule-file=iostream.pch -fmodule-file=array.pch -fmodule-file=regex.pch -fmodule-file=cstdint.pch -fmodule-file=sstream.pch -fmodule-file=memory.pch -fmodule-file=queue.pch -fmodule-file=counter_collection=counter_collection.pcm main.cppm -o $@

%.o: %.pcm
	$(CXX) $(CXXFLAGS) -fprebuilt-module-path=. -c $^ -o $@

counters: counters.o counter_collection.o main.o
	$(CXX) $(CXXFLAGS) $^ -o $@

clean:
	rm -f counters *.o *.pcm *.pch