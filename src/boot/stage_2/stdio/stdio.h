
#include "../stdint/stdint.h"
#include <stdarg.h>
#pragma once

#define VRAM 0xB8000

#define CHAR_COLOR_WHITE 0x0F


void clear();

void putc(char c);

void putS(const char *str);

void putByte(unsigned char val);

void printf(const char* str, ...);

void printHexDump(void* adr, uint16_t lines);

void *memset(void *ptr, uint8_t value, uint32_t num);