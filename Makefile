CC			 := g++
CFLAGS 		 := -O3 -D _DEBUG -lm -ggdb3 -std=c++17 -O0 -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations -Wc++14-compat -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector -fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer -pie -fPIE -Werror=vla
LOGGER_FLAGS :=
CFLAGS = -D _DEBUG
# CFLAGS += -D NDEBUG
# CFLAGS += -fsanitize=address

SOURCE_DIR         := source
LIB_RUN_NAME       := SPUproject
BUILD_DIR          := building

ifeq ($(DEBUG), 0)
	ASSERT_DEFINE = -DNDEBUG
endif

.PHONY: $(LIB_RUN_NAME) run $(BUILD_DIR) clean

# -------------------------   LIB RUN   -----------------------------

# LOGGER_SRC 			:= $(LOG_LIB_DIR)/colourfullPrint.cpp $(LOG_LIB_DIR)/debugMacros.cpp $(LOG_LIB_DIR)/logLib.cpp
# LOGGER_OBJ 			:= $(patsubst %.cpp, $(BUILD_DIR)/LOGGER_%.o, $(notdir ${LOGGER_SRC}))
SRC 	   				:= common/source/commands.cpp
OBJ 	   				:= $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(notdir ${SRC}))
OBJ_PROCESSOR			:= processor/building/main.o processor/building/processor.o processor/building/processorErrorsHandler.o
OBJ_ASSEMBLER			:= assembler/building/main.o assembler/building/assembly.o assembler/building/assemblerErrorsHandler.o

processor:
	make -C ./processor

assembler:
	make -C ./assembler

# running all commands without output (@ at the beginning)
$(LIB_RUN_NAME): $(OBJ)
	$(CC) $^ $(OBJ_PROCESSOR) -o $(BUILD_DIR)/$(LIB_RUN_NAME) -L./external/StackStruct/ -l:stackLib.a -lmy_loglib $(CFLAGS) -I processor/include/ -I external/StackStruct/include

$(BUILD_DIR)/%.o: common/source/%.cpp $(BUILD_DIR)
	$(CC) -c $< $(CFLAGS) -o $@ $(ASSERT_DEFINE)

run: $(LIB_RUN_NAME) processor
	./building/$(LIB_RUN_NAME)

# -------------------------   HELPER TARGETS   ---------------------------

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)
clean:
	rm -f $(BUILD_DIR)/*.o $(BUILD_DIR)/$(LIB_RUN_NAME)
