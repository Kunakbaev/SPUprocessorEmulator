CC			 := g++
CFLAGS 		 := -D _DEBUG -lm -ggdb3 -std=c++17 -O0 -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations -Wc++14-compat -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector -fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer -pie -fPIE -Werror=vla
LOGGER_FLAGS :=
# CFLAGS = -D _DEBUG
CFLAGS += -D NDEBUG
# CFLAGS += -fsanitize=address

MY_LOG_LIB_NAME    := my_loglib
SOURCE_DIR         := source
LIB_RUN_NAME       := processor
BUILD_DIR          := building
COMMANDS_DIR	   := ../common/commands
COMMANDS_BUILD_DIR := $(COMMANDS_DIR)/building
STACK_EXT_LIB_DIR  := ../external/StackStruct
LOGGER_EXT_LIB_DIR := ../external/LoggerLib
STACK_LIB_NAME	   := stackLib.a
SFML_LIBS		   := -lsfml-graphics -lsfml-window -lsfml-system
SFML_PATH		   := -I/libs/sfml/include

ifeq ($(DEBUG), 0)
	ASSERT_DEFINE = -DNDEBUG
endif

.PHONY: $(LIB_RUN_NAME) run $(BUILD_DIR) clean

# -------------------------   LIB RUN   -----------------------------

SRC 	   		   := $(wildcard ./$(SOURCE_DIR)/*.cpp)
OBJ 	   		   := $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(notdir ${SRC}))
COMMON_OBJ		   := $(COMMANDS_BUILD_DIR)/commands.o $(COMMANDS_BUILD_DIR)/commandsErrorsHandler.o

CFLAGS += -I $(LOGGER_EXT_LIB_DIR)/include
CFLAGS += -I $(STACK_EXT_LIB_DIR)/include

$(LIB_RUN_NAME): $(OBJ)
	@echo $(SRC)
	@make -C $(COMMANDS_DIR)
	@$(CC) $^ $(COMMON_OBJ) -o $(BUILD_DIR)/$(LIB_RUN_NAME) $(SFML_LIBS) -l$(MY_LOG_LIB_NAME) -L$(STACK_EXT_LIB_DIR) -l:$(STACK_LIB_NAME) $(CFLAGS)

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.cpp $(BUILD_DIR)
	@$(CC) -c $< $(CFLAGS) -o $@ $(ASSERT_DEFINE)

run: $(LIB_RUN_NAME)
	./building/$(LIB_RUN_NAME)

# -------------------------   HELPER TARGETS   ---------------------------

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)
clean:
	@rm -f $(BUILD_DIR)/*.o $(BUILD_DIR)/$(LIB_RUN_NAME)
