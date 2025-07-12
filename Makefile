TARGET := cppc

all: $(TARGET)

$(TARGET): cppc.cpp
	g++ -std=c++23 cppc.cpp -o $(TARGET)

.PHONY: clean

clean:
	rm -rf $(TARGET)
