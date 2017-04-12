vpath %.cc src/
vpath %.h src/

%.o: %.cc %.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm *.o || true
