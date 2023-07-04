#ifndef ZIGIL_DIR_H
#define ZIGIL_DIR_H

#include "zigil.h"

#define MAX_DIRLISTING_ENTRIES 64

typedef enum zgl_DirEntryCode {
    DEC_OTHER = 0,
    DEC_FILE = 1,
    DEC_DIR = 2,
    
    NUM_DEC
} zgl_DirEntryCode;

typedef struct zgl_DirEntry {
    char *name;
    zgl_DirEntryCode code;
    size_t size; // bytes
} zgl_DirEntry;

typedef struct zgl_DirListing {
    size_t num_entries;
    zgl_DirEntry entries[MAX_DIRLISTING_ENTRIES];
} zgl_DirListing;

extern void zgl_GetDirListing(zgl_DirListing *dl, char const *dirpath);
extern void zgl_PrintDir(char const *dirpath);

#endif
