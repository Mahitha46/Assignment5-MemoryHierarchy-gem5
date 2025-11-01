# Makefile for vecadd variants
BIN_DIR ?= bin
SRC_DIR ?= src
CFLAGS  ?= -O3 -march=x86-64 -mtune=native

all: $(BIN_DIR)/vecadd_scalar $(BIN_DIR)/vecadd_sse $(BIN_DIR)/vecadd_avx

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(BIN_DIR)/vecadd_scalar: $(SRC_DIR)/vecadd_scalar.c | $(BIN_DIR)
	gcc $(CFLAGS) -fno-tree-vectorize $< -o $@

$(BIN_DIR)/vecadd_sse: $(SRC_DIR)/vecadd_sse.c | $(BIN_DIR)
	gcc $(CFLAGS) -msse2 -mfpmath=sse $< -o $@

$(BIN_DIR)/vecadd_avx: $(SRC_DIR)/vecadd_avx.c | $(BIN_DIR)
	gcc $(CFLAGS) -mavx2 -mfpmath=sse $< -o $@

clean:
	rm -rf $(BIN_DIR)/*
