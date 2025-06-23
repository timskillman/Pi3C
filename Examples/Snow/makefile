TARGET = Snow

CC = g++
CFLAGS = -std=c++17 -Wall -Wno-narrowing
LDLIBS = -lGL -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf
LDFLAGS = 

INC_DIRS:=  ../../SharedCode/core \
            ../../SharedCode/file \
            ../../SharedCode/ui \
            ../../SharedCode/helpers \
            ../../SharedCode/editing \
            ../../SharedCode/glutess \
            ../../ThirdParty/rapidjson \
            ../../ThirdParty/openFBX \
            ../../ThirdParty/gltf/fx \
            ../../ThirdParty/SDL2/include \
            ../../ThirdParty/SDL2_image/include \
            ../../ThirdParty/SDL2_ttf/include \
            ../../ThirdParty/SDL2_mixer/include

BUILDPATH = ../../../_build/Projects/$(TARGET)/

GLES_DIR = /opt/vc/include

SRC_DIR:= $(INC_DIRS) Source
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
