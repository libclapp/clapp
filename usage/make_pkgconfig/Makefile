CFLAGS = --std=c++17 `pkg-config --cflags libclapp`
LDFLAGS = `pkg-config --libs libclapp`
OBJ = ../main.o

%.o: %.cpp $(DEPS)
	$(CXX) -c -o $@ $< $(CFLAGS)

usage_make_pkgconfig: $(OBJ)
	$(CXX) -o $@ $^ $(CFLAGS) $(LDFLAGS)
