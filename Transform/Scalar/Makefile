all: Local.so

CXXFLAGS = -rdynamic -fPIC $(shell llvm-config --cxxflags) -g 

%.so: %.o
	$(CXX) -fPIC -shared $^ -o $@
clean:
	rm -f *.o *.so
cleanest:
	rm -f *.o *.so
	rm -f *.bc *.ll