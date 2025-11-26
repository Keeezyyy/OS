#pragma once
#include "../stdint/stdint.h"

extern void halt();

extern uint8_t read_io_byte(uint32_t ADDRESS);


extern void write_io_byte(uint32_t ADDRESS, uint8_t VAL);