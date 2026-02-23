CXX = g++
CXXFLAGS = -std=c++17 -Wall -g -pthread # Añadido -pthread
LDFLAGS = -pthread # Añadido -pthread

TARGET = main
TEST_TARGET = tests # <-- 1. Declaramos el nombre del ejecutable de pruebas

SRCS = main.cpp util.cpp pointers.cpp \
       DemoBTree.cpp \
       #sorting.cpp DemoArray.cpp

OBJS = $(SRCS:.cpp=.o)

# 2. Modificamos 'all' para que construya TANTO el programa principal como las pruebas
all: $(TARGET) $(TEST_TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) $^ -o $@

# 3. Nueva regla específica para el juez automático
$(TEST_TARGET): test_btree.cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $< -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 4. Agregamos el ejecutable de pruebas a la regla de limpieza
clean:
	rm -f $(OBJS) $(TARGET) $(TEST_TARGET)

.PHONY: all clean
