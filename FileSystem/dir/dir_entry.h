#ifndef FILESYSTEM_DIR_ENTRY_H
#define FILESYSTEM_DIR_ENTRY_H

/* Directory input, 32 bytes each */
typedef struct {
    uint8_t filename[18];
    uint8_t attributes;
    uint8_t reserved[7];
    uint16_t first_block;
    uint32_t size;
} dir_entry_t;

#endif //FILESYSTEM_DIR_ENTRY_H
