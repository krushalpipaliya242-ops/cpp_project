# Build from the project root:  make        (creates Executable/Project  or Project.exe on Windows/MinGW)
CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
SRC      = $(wildcard Source/*.cpp)
ifeq ($(OS),Windows_NT)
  TARGET = Executable/Project.exe
else
  TARGET = Executable/Project
endif

$(TARGET): $(SRC) $(wildcard Source/*.h)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)
