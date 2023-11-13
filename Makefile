
CXX = g++
TARGET_EXEC ?= main.exe
SRC_DIR ?= ./src
BUILD_DIR ?= ./build

OPENCV_INCLUDE_PATH := "C:\Users\wildb\opencv\install\include"
PLOG_INCLUDE_PATH := "C:\Users\wildb\plog\include"
OPENCV_LIB_PATH := "C:\Users\wildb\opencv\install\x64\mingw\lib"
OPENCV_LIBS := -lopencv_core481 -lopencv_highgui481 -lopencv_imgproc481 -lopencv_imgcodecs481 -lopencv_videoio481
CPPFLAGS = -I $(OPENCV_INCLUDE_PATH)
LDFLAGS = -L $(OPENCV_LIB_PATH) $(OPENCV_LIBS)

# Get the list of source files
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
# Strip the source dir
RAW_SRCS = $(notdir $(SRCS))
# Build a list of object files
OBJS = $(addprefix $(BUILD_DIR)/, $(RAW_SRCS:.cpp=.o))


$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CXX) $(OBJS) $(LDFLAGS) -o $@

$(BUILD_DIR)/capturemanager.o: $(SRC_DIR)/capturemanager.cpp $(SRC_DIR)/capturemanager.hpp
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CPPFLAGS) -c $< -o $@

$(BUILD_DIR)/cameracapturemanager.o: $(SRC_DIR)/cameracapturemanager.cpp $(SRC_DIR)/cameracapturemanager.hpp ${BUILD_DIR}/capturemanager.o
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CPPFLAGS) -c $< -o $@

$(BUILD_DIR)/usbservocontroller.o: $(SRC_DIR)/usbservocontroller.cpp $(SRC_DIR)/usbservocontroller.hpp ${BUILD_DIR}/capturemanager.o ${BUILD_DIR}/ceSerial.o
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CPPFLAGS) -c $< -o $@

$(BUILD_DIR)/main.o: $(SRC_DIR)/main.cpp
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CPPFLAGS) -c $< -o $@

$(BUILD_DIR)/ceSerial.o: $(SRC_DIR)/ceSerial.cpp
	mkdir -p $(BUILD_DIR)
	$(CXX) -c $< -o $@

$(BUILD_DIR)/serial.o: $(SRC_DIR)/serial.cpp
	mkdir -p $(BUILD_DIR)
	$(CXX) -c $< -o $@

$(BUILD_DIR)/utils.o: $(SRC_DIR)/utils.cpp
	mkdir -p $(BUILD_DIR)
	$(CXX) -c $< -o $@	


$(BUILD_DIR)/test-com.exe: $(BUILD_DIR)/ceSerial.o
	$(CXX) $(SRC_DIR)/test-com.cpp $(BUILD_DIR)/ceSerial.o -o $(BUILD_DIR)/test-com.exe


PHONY: clean

clean:
	$(RM) -r $(BUILD_DIR)





