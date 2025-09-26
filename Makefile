CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pthread
BUILD_DIR = build
INC_DIR = include

TARGET = $(BUILD_DIR)/test_cli
SRCS = src/libtslog.cpp tests/test_cli.cpp
OBJS = $(SRCS:%.cpp=$(BUILD_DIR)/%.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) *.log

