TAR := TrigonometryCalculator
SRC := $(wildcard *.cpp)
OBJ := $(patsubst %.cpp, %.o, $(SRC))
HDR := $(wildcard *.h)
CC := g++
CFLAGS := -g

$(TAR): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TAR)

$(OBJ): %.o: %.cpp $(HDR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@-rm $(TAR) $(OBJ)
