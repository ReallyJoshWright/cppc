TARGET := cppc
WINDOWS_TARGET := cppc.exe

all: $(TARGET)

$(TARGET): cppc.cpp
	g++ -std=c++23 cppc.cpp -o $(TARGET)

$(WINDOWS_TARGET): cppc.cpp
	x86_64-w64-mingw32-g++ -std=c++23 cppc.cpp -o $(WINDOWS_TARGET) -static-libgcc -static-libstdc++

.PHONY: clean windows

clean:
	rm -rf $(TARGET)

windows: $(WINDOWS_TARGET)
