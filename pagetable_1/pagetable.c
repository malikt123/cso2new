#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdalign.h>
#include "mlpt.h"
#include "config.h"
#include <math.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

size_t ptbr = 0;

size_t translate(size_t va) {
    // arithmetic to find segment size
    size_t offset_mask = (1 << POBITS) - 1; // mask same size as offset
    size_t offset = va & offset_mask; // determine offset by itself
    size_t page_size = 1 << POBITS;
    size_t entries_per_page = page_size / 8;
    size_t segment_size = log(entries_per_page) / log(2);
    size_t total_bits = segment_size * LEVELS;
    size_t bitmask = (1 << total_bits) - 1; // mask same size as segments
    size_t just_segments = (va >> POBITS) & bitmask; // determine segements by themselves
    size_t pt_address = ptbr;
    if(ptbr == 0){
        return -1; // return -1 if ptbr = 0
    }
    for(int i = 0; i < LEVELS; i++){
        // extract segment_size bits from the left of just_segments
        size_t segment = just_segments >> ((LEVELS - i - 1) * segment_size); // isolate individual segment size
        size_t segment_mask = (1 << segment_size) - 1; // mask same size as one segment
        segment = segment & segment_mask; // determine segment by itself
        size_t *address_pointer = pt_address + (segment * 8); 
        size_t pte_address = *(address_pointer);
        if(pte_address % 2 == 0){
            return -1; // not valid
        }
        pte_address = (pte_address >> POBITS) * page_size;
        pt_address = pte_address; // acts as next ptbr
    }
    size_t ppn = pt_address >> POBITS; 
    ppn = ppn << POBITS; // make enough 0's for offset to get added
    size_t physical_address = ppn + offset; //physical address
    return physical_address;
}

void page_allocate(size_t va) {
    // arithmetic to find segment size
    size_t offset_mask = (1 << POBITS) - 1; // mask same size as offset
    size_t offset = va & offset_mask; // determine offset by itself
    size_t page_size = 1 << POBITS;
    size_t entries_per_page = page_size / 8;
    size_t segment_size = log(entries_per_page) / log(2);
    size_t total_bits = segment_size * LEVELS;
    size_t bitmask = (1 << total_bits) - 1; // mask same size as segments
    size_t just_segments = (va >> POBITS) & bitmask; // determine segements by themselves
    if(ptbr == 0){ // make page table if not made yet
        size_t *new_page_table;
        posix_memalign(&new_page_table, page_size, page_size); // make space for next table
        memset(new_page_table, 0, page_size); // set memory to 0 to avoid seg faults
        ptbr = new_page_table; // point to beginning of new table
    }
    size_t pt_address = ptbr;
    for (int i = 0; i < LEVELS; i++) {
        // extract segment_size bits from the left of just_segments
        size_t segment = just_segments >> ((LEVELS - i - 1) * segment_size); // isolate individual segment size
        size_t segment_mask = (1 << segment_size) - 1; // mask same size as one segment
        segment = segment & segment_mask; // determine segment by itself
        size_t *address_pointer = pt_address + (segment * 8); 
        size_t pte_address = *(address_pointer);
        if (pte_address % 2 == 0) { // mapping doesn't exist
            size_t *new_page_table;
            posix_memalign(&new_page_table, page_size, page_size); // make space for next table
            memset(new_page_table, 0, page_size); // set memory to 0 to avoid seg faults
            *address_pointer = (size_t)new_page_table | 1; // set valid bit to 1
            // update pt_address for next level
            address_pointer = ((size_t)new_page_table >> POBITS) * page_size;
            pt_address = (size_t)address_pointer; // acts as next ptbr
        } else {
            // mapping exists
            pte_address = (pte_address >> POBITS) * page_size;
            pt_address = pte_address;
        }
    }
}
// int main(){
//     page_allocate(0x3000);
//     size_t *pointer_to_table;
//     pointer_to_table = (size_t *) ptbr;
//     size_t page_table_entry = pointer_to_table[3];
//     printf("PTE @ index 3: valid bit=%d  physical page number=0x%lx\n",
//     (int) (page_table_entry & 1),
//     (long) (page_table_entry >> 12)
// );
// }