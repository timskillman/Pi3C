TARGET = PerlinMap

CC = g++
CFLAGS = -std=c++17 -Wall
LDLIBS = -lGL -lSDL2_image -lSDL2_mixer -lSDL2_ttf
LDFLAGS = $(shell sdl2-config --cflags --libs)

SHARED_DIR:= ../../SharedCode/core
FILE_DIR:= ../../SharedCode/file
UI_DIR:= ../../SharedCode/ui
HELPS_DIR:= ../../SharedCode/helpers
TESS_DIR:= ../../SharedCode/glutess
CPATH = $(SHARED_DIR)
CPATH = $(FILE_DIR)
CPATH = $(UI_DIR)
CPATH = $(HELPS_DIR)
CPATH = $(TESS_DIR)

BUILDPATH = ../../../_build/Projects/$(TARGET)/

GLES_DIR = /opt/vc/include
CPATH = $(GLES_DIR)

SRC_DIR:= $(SHARED_DIR) $(FILE_DIR) $(UI_DIR) $(HELPS_DIR) $(TESS_DIR) $(GLES_DIR) Source
BUILD_DIR:= $(addprefix $(BUILDPATH),$(SRC_DIR))

SRC:= $(foreach sdir,$(SRC_DIR),$(wildcard $(sdir)/*.cpp $(sdir)/*.c))
OBJ:= $(patsubst %.cpp,$(BUILDPATH)%.o,$(SRC))
INCS:= $(addprefix -I,$(SRC_DIR))

vpath %.cpp $(SRC_DIR)

define make-target
$1/%.o: %.cpp
	$(CC) $(INCS) -c $$< -o $$@ $(CFLAGS) $(LDLIBS) $(LDFLAGS)
endef

.PHONY: all checkdirs clean

all: checkdirs $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $^ -o $@ $(CFLAGS) $(LDLIBS) $(LDFLAGS)
	echo $(OBJ)

checkdirs: $(BUILD_DIR)

$(BUILD_DIR):
	@mkdir -p $@

clean:
	@rm -rf $(BUILD_DIR)

$(foreach bdir,$(BUILD_DIR),$(eval $(call make-target,$(bdir))))
