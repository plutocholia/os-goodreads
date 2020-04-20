OBJECTS = main.o Serial.o Utills.o Book.o
CC = g++ -std=c++11

all: main
	make clean

main: $(OBJECTS)
	$(CC) -o main $(OBJECTS)

main.o: main.cpp
	$(CC) -c main.cpp

Serial.o: Serial.h Serial.cpp
	$(CC) -c Serial.cpp

Utills.o: Utills.h Utills.cpp
	$(CC) -c Utills.cpp

Book.o: Book.h Book.cpp
	$(CC) -c Book.cpp

clean:
	rm *.o