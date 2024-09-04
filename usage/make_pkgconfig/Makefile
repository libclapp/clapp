CFLAGS = --std=c++20 `pkg-config --cflags libclapp`
LDFLAGS = `pkg-config --libs libclapp`
OBJ = ../main.o

%.o: %.cpp $(DEPS)
	$(CXX) -c -o $@ $< $(CFLAGS)

usage_make_pkgconfig: $(OBJ)
	$(CXX) -o $@ $^ $(CFLAGS) $(LDFLAGS)
