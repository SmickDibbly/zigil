#define _DEFAULT_SOURCE
#include <dirent.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>

#include "src/zigil_dir.h"
#include "src/X11/zglx.h"

void zgl_PrintDir(char const *dirpath) {
    DIR *p_dir = opendir(dirpath);

    printf("Directory %s:\n", dirpath);

    struct dirent *p_entry;
    size_t num_entries = 0;
    char entryname[256];
    
    while (true) {
        p_entry = readdir(p_dir);
	
        if (p_entry == NULL) {
            break;
        }
        
        strcpy(entryname, p_entry->d_name);
        if (strcmp(".", entryname) == 0) continue;
        if (strcmp("..", entryname) == 0) continue;

        if (p_entry->d_type == DT_REG) {
            char filepath[256];
            strcpy(filepath, dirpath);
            strcat(filepath, p_entry->d_name);
            struct stat st;
            stat(filepath, &st);
            fprintf(stdout, " File: %s : %li bytes\n", entryname, st.st_size);
        }
        else if (p_entry->d_type == DT_DIR) {
            fprintf(stdout, "  Dir: %s\n", entryname);
        }
        else {
            fprintf(stdout, "Other: %s\n", entryname);
        }

        
        /*
          if (suffix(".dibj", entryname)) {
          fprintf(stdout, "DIBJ: %s\n", entryname);
          }
        */
        
        num_entries++;
    }
    
    
    closedir(p_dir);
}

char buffer[8192] = {'\0'};

void zgl_GetDirListing(zgl_DirListing *dl, char const *dirpath) {
    DIR *p_dir = opendir(dirpath);

    char *p_buf = buffer;

    struct dirent *p_entry;
    size_t num_entries = 0;
    char entryname[256];
    
    for (size_t i_entry = 0; i_entry < MAX_DIRLISTING_ENTRIES;) {
        p_entry = readdir(p_dir);
	
        if (p_entry == NULL) {
            break;
        }

        strcpy(entryname, p_entry->d_name);
        strcpy(p_buf, entryname);
        dl->entries[i_entry].name = p_buf;
        p_buf += strlen(entryname);
        *p_buf = '\0';
        p_buf++;
        i_entry++;
        num_entries++;

        if (p_entry->d_type == DT_DIR) {
            dl->entries[i_entry].code = DEC_DIR;
        }
        else if (p_entry->d_type == DT_REG) {
            dl->entries[i_entry].code = DEC_FILE;
            
            char filepath[256];
            strcpy(filepath, dirpath);
            strcat(filepath, p_entry->d_name);
            struct stat st;
            stat(filepath, &st);

            dl->entries[i_entry].size = st.st_size;
        }
        else {
            dl->entries[i_entry].code = DEC_OTHER;
        }
    }

    zgl_PrintDir(dirpath);
    
    
    dl->num_entries = num_entries;
}
