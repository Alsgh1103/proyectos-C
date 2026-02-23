CXX = g++
CXXFLAGS = -std=c++17 -Wall -g -pthread
LDFLAGS = -pthread

TARGET = main
SRCS = main.cpp util.cpp
TEST_AVLTREE = test_binarytree # <-- Nuevo ejecutable de prueba

SRCS = main.cpp util.cpp pointers.cpp DemoBTree.cpp # etc...
OBJS = $(SRCS:.cpp=.o)

# Ahora "make" compilará todo: tu programa, el test del btree y el test del avl
all: $(TARGET) $(TEST_BTREE) $(TEST_AVLTREE)

$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) $^ -o $@

$(TEST_BTREE): test_btree.cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $< -o $@

$(TEST_AVLTREE): test_binarytree.cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $< -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET) $(TEST_BTREE) $(TEST_AVLTREE)

.PHONY: all clean
