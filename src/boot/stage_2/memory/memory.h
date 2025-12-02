#pragma once
#include "../stdint/stdint.h"



typedef struct{
    uint64_t BaseAddress;    
    uint64_t Length;         
    uint32_t Type;           
    uint32_t ACPI_Extended;  
}E820Entry;

bool memcmp(const void *lhs, const void *rhs, unsigned int count);

void * memcpy ( void * destination, const void * source, uint32_t num );

