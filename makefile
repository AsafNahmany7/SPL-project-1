# Please implement your Makefile rules and targets below.
# Customize this file to define how to build your project.
# שם הקובץ המוצלח
TARGET = main

# קומפיילר והדגלים
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -Iinclude

# תיקיות
SRC_DIR = src
INCLUDE_DIR = include
OBJ_DIR = obj

# קבצי מקור ואובייקטים
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))

# מטרת ברירת מחדל
all: $(TARGET)

# בניית הקובץ הסופי
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# יצירת קבצי אובייקט
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# יצירת תיקיית אובייקטים
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# ניקוי
clean:
	rm -rf $(OBJ_DIR) $(TARGET)

# כלי בדיקה
.PHONY: all clean
