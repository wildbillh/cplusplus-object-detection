# Makefile to build the project
# Outside dependencies:
#	opencv
#	spdlog

CXX = g++ -DSPDLOG_COMPILED_LIB=1
TARGET_EXEC ?= main.exe
SRC_DIR ?= ./src
BUILD_DIR ?= ./build
LOCAL := /c/Users/wildb/local

OPENCV_INCLUDE_PATH = "$(LOCAL)/opencv/include"
OPENCV_LIB_PATH = "$(LOCAL)/opencv/lib"
SPDLOG_INCLUDE_PATH = "$(LOCAL)/spdlog/include"
SPDLOG_LIB_PATH = "$(LOCAL)/spdlog/lib"
LIBS = -lopencv_core481 -lopencv_highgui481 -lopencv_imgproc481 -lopencv_imgcodecs481 -lopencv_videoio481 -lspdlog
CPPFLAGS = -I $(OPENCV_INCLUDE_PATH) -I $(SPDLOG_INCLUDE_PATH)
LDFLAGS = -L $(OPENCV_LIB_PATH) -L $(SPDLOG_LIB_PATH) $(LIBS) 

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

$(BUILD_DIR)/usbservocontroller.o: $(SRC_DIR)/usbservocontroller.cpp $(SRC_DIR)/usbservocontroller.hpp ${BUILD_DIR}/capturemanager.o
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CPPFLAGS) -c $< -o $@

$(BUILD_DIR)/main.o: $(SRC_DIR)/main.cpp
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CPPFLAGS) -c $< -o $@


$(BUILD_DIR)/serial.o: $(SRC_DIR)/serial.cpp $(SRC_DIR)/serial.hpp
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CPPFLAGS) -c $< -o $@

$(BUILD_DIR)/utils.o: $(SRC_DIR)/utils.cpp $(SRC_DIR)/utils.hpp
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CPPFLAGS) -c $< -o $@	



PHONY: clean

clean:
	$(RM) -r $(BUILD_DIR)





