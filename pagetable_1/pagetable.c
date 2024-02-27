#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include "mlpt.h"
#include "config.h"

// Define constants and data structures if needed

#define PAGE_SIZE (1 << POBITS)
#define PAGE_TABLE_SIZE (1 << (64 - POBITS))
#define PTE_VALID_BIT (1UL << (64 - POBITS - 1))

typedef struct {
    size_t physical_page_number;
} PageTableEntry;

size_t ptbr;

// Define helper functions if needed
void setup_page_table() {
    // Initialize ptbr with the base address of the page table
    ptbr = 0; 
}

size_t translate(size_t va) {
    size_t offset_mask = (1 << POBITS) - 1;
    size_t pt_index_mask = ((size_t)PAGE_TABLE_SIZE - 1) << POBITS;
    size_t pt_index_shift = 64 - POBITS;

    // get page table indices from v address
    size_t offset = va & offset_mask;
    size_t pt_index = (va & pt_index_mask) >> pt_index_shift;

    size_t pt_address = ptbr;
    for (int level = 0; level < LEVELS; level++) {
        PageTableEntry* page_table = (PageTableEntry*)pt_address;
        PageTableEntry pte = page_table[pt_index];

        if (!(pte.physical_page_number & PTE_VALID_BIT)) { // check if PTE is valid
            
            return ~(size_t)0;
        }
        pt_address = pte.physical_page_number & ~(PTE_VALID_BIT);
        
        pt_index = (va >> ((size_t)(pt_index_shift - (level + 1) * POBITS))) & (PAGE_TABLE_SIZE - 1); // update page table
    }

    // determine physical address
    size_t physical_address = (pt_address << POBITS) | offset;

    return physical_address;
}

int main() {
    // Your main program logic here
    setup_page_table();

    // Example usage of translate function
    size_t virtual_address = 0x123456789ABCDEF;
    size_t physical_address = translate(virtual_address);

    printf("Physical Address: %lu\n", physical_address);

    return 0;
}