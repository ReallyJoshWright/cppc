TARGET := cppc
WINDOWS_TARGET := cppc.exe

all: $(TARGET)

$(TARGET): cppc.cpp
	g++ -std=c++23 cppc.cpp -o $(TARGET)

$(WINDOWS_TARGET): cppc.cpp
	cl /std:c++latest /EHsc cppc.cpp /Fe$(WINDOWS_TARGET)

.PHONY: clean windows

clean:
	rm -rf $(TARGET)

windows: $(WINDOWS_TARGET)
