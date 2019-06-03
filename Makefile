CC := g++
LD := g++

CC_FLAGS := -std=c++2a -Wall -Wextra -MMD -MP -Ofast
CC_INCLUDE := -I ./include
CC_DEFINES :=

LD_FLAGS :=
LD_LIBS := -lSDL2 -lGL -lGLEW

SRC_DIR := \
	src

OBJ_DIR := obj

SRC := $(wildcard $(addsuffix /*.cpp, $(SRC_DIR)))
OBJ := $(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(SRC)))

OUT := hockey
OUT_NAME := $(addprefix ./, $(OUT))

all:
	@$(MAKE) --no-print-directory --jobs 4 $(OUT)

$(OUT): $(OBJ)
	$(LD) $(LD_FLAGS) $^ $(LD_LIBS) -o $(OUT)

$(OBJ_DIR)/%.o: %
	@mkdir --parent $(@D)
	$(CC) $(CC_FLAGS) $(CC_INCLUDE) $(CC_DEFINES) -c $< -o $@

run: all
	@$(OUT_NAME)

clean:
	@rm -rf $(OBJ_DIR)
	@rm -f $(OUT)

-include $(OBJ:.o=.d)
