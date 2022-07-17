CFLAGS = -O3 -Wall -Wpedantic -std=c11 -Wno-unused-result -Wno-format

SRC = src
BUILD = build
TARGET = hexane

NAMES = comments \
buffer \
cmdline \
cursor \
io \
keys \
main \
msg \
render \
selection \
state \
util \
mark \
search \
main

OBJ = $(addprefix $(BUILD)/, $(addsuffix .o, $(NAMES)))
HEADERS = $(addprefix $(SRC)/, $(addsuffix .h, $(filter-out main, $(NAMES))))

.PHONY: all
all: $(TARGET)

$(TARGET): $(OBJ) $(HEADERS)
	$(CC) $(CFLAGS) $(filter-out %.h, $^) -o $(TARGET)

$(SRC)/%.c: $(SRC)/%.h

$(BUILD)/%.o: $(SRC)/%.c
	@ mkdir -p $(BUILD)
	$(CC) $(CFLAGS) $< -c -o $@

tests: $(addprefix $(BUILD)/, util.o search.o) test/main.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	- rm -rf $(BUILD)
	- rm $(TARGET)
