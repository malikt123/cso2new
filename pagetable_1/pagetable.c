#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdalign.h>
#include "mlpt.h"
#include "config.h"

#define PAGE_SIZE (1 << POBITS)
#define PAGE_TABLE_SIZE (1 << (64 - POBITS))
#define PTE_VALID_BIT (1UL << (64 - POBITS - 1))

size_t ptbr;

size_t translate(size_t va) {
    size_t offset_mask = (1 << POBITS) - 1;
    size_t pt_index_mask = POBITS - 3;
    // get offset/page table index from the virtual address
    size_t offset = va & offset_mask;
    size_t pt_index = (va >> POBITS) & ((1 << pt_index_mask) - 1);
    size_t pt_address = ptbr; // initialize page table address with page table base register
    size_t entry = *(size_t*)(ptbr + pt_index * 8);
    if(entry % 2 == 0){
        return -1;
    }
    size_t ppn = entry >> POBITS;
    printf("%zx\n", ppn);
    ppn = ppn << POBITS;
    printf("%zx\n", ppn);
    size_t physical_address = ppn + offset;
    printf("%zx\n", physical_address);
    // check if entry is valid (least significant bit) -- 0 is not valid. if 1, most significant (64 - POBITS)

    return physical_address;
}
    alignas(4096);
    static size_t page_of_data[512];
    alignas(4096);
    static size_t testing_page_table[512];
    alignas(4096);
    static char data_for_page_3[4096];
    static void set_testing_ptbr(void) {
        ptbr = (size_t) &testing_page_table[0];
    }
int main() {
    set_testing_ptbr();
    // printf("Physical Address: %lu\n", physical_address);
    size_t address_of_data_for_page_3_as_integer = (size_t) &data_for_page_3[0]; 
    size_t physical_page_number_of_data_for_page_3 = address_of_data_for_page_3_as_integer >> 12;
    // instead of >> 12, we could have written:
    // address_of_data_for_page_3_as_integer / 4096
    size_t page_table_entry_for_page_3 = (
    // physical page number in upper (64-POBITS) bits
            (physical_page_number_of_data_for_page_3 << 12)
        |
            // valid bit in least significant bit, set to 1
            1
    );
    // assuming testing_page_table initialized as above and ptbr points to it
    testing_page_table[3] = page_table_entry_for_page_3;
    return translate(0x3045) == (size_t) &data_for_page_3[0x45];
}
