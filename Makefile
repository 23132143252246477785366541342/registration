# the compiler: gcc for C program, define as g++ for C++
  CC = g++

  TARGET = ./scripts/main
 
  all: $(TARGET)
 
  $(TARGET): $(TARGET).cpp
	$(CC) $(TARGET).cpp -o ./output/main -I ./sqlite/ -l sqlite3
  clean:
	$(RM) $(TARGET)