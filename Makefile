vpath %.cc src/
vpath %.h src/

CXXFLAGS += -Werror -Wall -Wextra -Wpedantic -Wshadow -Wundef -Wpointer-arith -Wcast-align -Wstrict-overflow=5 -Wwrite-strings -Wcast-qual -Wswitch-enum -Wno-return-type

.PHONY: default
default: varint.o

%.o: %.cc %.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm *.o || true
