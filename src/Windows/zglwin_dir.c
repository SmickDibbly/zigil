#include "src/zigil_dir.h"
#include "src/diblib_local/dibstr.h"
#include "src/Windows/zglwin.h"
#include <stdio.h>

void zgl_PrintDir(char const *dirpath) {
    char dir[MAX_PATH];
    DWORD dwError = 0;

    printf("Directory %s:\n", dirpath);
        
    if (strlen(dirpath) > (MAX_PATH - 3)) {
        puts("Directory path is too long.\n");
        return;
    }
    strncpy(dir, dirpath, MAX_PATH);
    strncat(dir, "\\*", MAX_PATH-3);

    WIN32_FIND_DATA ffd;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    size_t num_entries = 0;
    char entryname[MAX_PATH];
    uint64_t filesize;
    
    hFind = FindFirstFileA(dir, &ffd);

    if (INVALID_HANDLE_VALUE == hFind) {
        return;
    }
    
    do {
        strncpy(entryname, ffd.cFileName, MAX_PATH);
        if (strcmp(".", entryname) == 0) continue;
        if (strcmp("..", entryname) == 0) continue;

        if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            fprintf(stdout, "  Dir: %s\n", entryname);
        }
        else {
            filesize = (ffd.nFileSizeHigh * (MAXDWORD+1)) + ffd.nFileSizeLow;
            fprintf(stdout, " File: %s : %llu bytes\n",
                    entryname, filesize);
        }

        num_entries++;
    } while (FindNextFileA(hFind, &ffd) != 0);
 
    dwError = GetLastError();
    if (dwError != ERROR_NO_MORE_FILES) {
        return;
    }
    
    FindClose(hFind);
}

char buffer[8192];

extern void zgl_GetDirListing(zgl_DirListing *dl, char const *dirpath) {
    char dir[MAX_PATH];

    if (strlen(dirpath) > (MAX_PATH - 3)) {
        puts("Directory path is too long.\n");
        return;
    }
    strncpy(dir, dirpath, MAX_PATH-3);
    strncat(dir, "\\*", MAX_PATH-3);
    
    WIN32_FIND_DATA ffd;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    size_t num_entries = 0;
    char entryname[MAX_PATH];
    char *p_buf = buffer;
    
    hFind = FindFirstFileA(dir, &ffd);

    if (INVALID_HANDLE_VALUE == hFind) {
        return;
    }
    
    for (size_t i_entry = 0; i_entry < MAX_DIRLISTING_ENTRIES;) {
        strncpy(entryname, ffd.cFileName, MAX_PATH);
        strncpy(p_buf, entryname, MAX_PATH);
        dl->entries[i_entry].name = p_buf;
        p_buf += strlen(entryname);
        *p_buf = '\0';
        p_buf++;
        i_entry++;
        num_entries++;

        if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            dl->entries[i_entry].code = DEC_DIR;
        }
        else {
            dl->entries[i_entry].code = DEC_FILE;
        }
            
        if (dl->entries[i_entry].code == DEC_FILE) {
            dl->entries[i_entry].size =
                (ffd.nFileSizeHigh * (MAXDWORD+1)) + ffd.nFileSizeLow;
        }

        if (FindNextFileA(hFind, &ffd) == 0) break;
    }

    dl->num_entries = num_entries;
}
