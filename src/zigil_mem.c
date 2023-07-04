#include "src/zigil_mem.h"
#include "src/zigil.h"
#include "src/diblib_local/dibassert.h"

/* -------------------------------------------------------------------------- */
#if ZGL_MEMCHECK_LEVEL == 0
/* -------------------------------------------------------------------------- */

void zgl_PrintMemSummary(void) {
    // do nothing
}

/* -------------------------------------------------------------------------- */
#elif ZGL_MEMCHECK_LEVEL == 1
/* -------------------------------------------------------------------------- */

#include "src/diblib_local/dibstr.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef ZGL_LINUX
#include <malloc.h>
#endif

static int num_allocs = 0;
#ifdef ZGL_LINUX
static int alloc_bytes = 0;
#endif


void *_zgl_Malloc(size_t size, char const *file, int line) {
    if (size == 0) {
        printf("ZIGIL.MEM.ERROR: Allocation request for 0 bytes %s : %i : zgl_Malloc( %zu )\n", file, line, size);
        abort();
    }
    
    void *ptr = malloc(size);
    
    if ( ! ptr) {
        printf("ZIGIL.MEM.ERROR: Failed to allocate memory %s : %i : zgl_Malloc( %zu )\n", file, line, size);
        abort();
    }

    num_allocs++;
#ifdef ZGL_LINUX
    alloc_bytes += (int)malloc_usable_size(ptr);
#endif
    return ptr;
}

void *_zgl_Calloc(size_t nmemb, size_t sizem, char const *file, int line) {
    if (sizem == 0) {
        printf("ZIGIL.MEM.ERROR: Allocation request for 0 bytes %s : %i : zgl_Calloc( %zu, %zu )\n", file, line, nmemb, sizem);
        abort();
    }
    
    void *ptr = calloc(nmemb, sizem);
    
    if ( ! ptr) {
        printf("ZIGIL.MEM.ERROR: Failed to allocate memory %s : %i : zgl_Calloc( %zu, %zu )\n", file, line, nmemb, sizem);
        abort();
    }

    num_allocs++;
#ifdef ZGL_LINUX
    alloc_bytes += (int)malloc_usable_size(ptr);
#endif
    return ptr;
}

void *_zgl_Realloc(void *ptr, size_t size, char const *file, int line) {
    if (size == 0) {
        printf("ZIGIL.MEM.ERROR: Allocation request for 0 bytes %s : %i : zgl_Realloc( %p, %zu )\n", file, line, ptr, size);
        abort();
    }

#ifdef ZGL_LINUX
    alloc_bytes -= (int)malloc_usable_size(ptr);
#endif
    
    void *new_ptr = realloc(ptr, size);
    
    if ( ! new_ptr) {
        printf("ZIGIL.MEM.ERROR: Failed to reallocate memory %s : %i : zgl_Realloc( %p, %zu )\n", file, line, ptr, size);
        abort();
    }
    
#ifdef ZGL_LINUX
    alloc_bytes += (int)malloc_usable_size(new_ptr);
#endif
    return new_ptr;
}

void _zgl_Free(void *ptr, char const *file, int line) {
    if (ptr == NULL) {
        printf("ZIGIL.MEM.WARN: NULL-pointer passed to zgl_Free() %s : %i\n", file, line);
        return;
    }
    
#ifdef ZGL_LINUX
    alloc_bytes -= (int)malloc_usable_size(ptr);
#endif
    free(ptr);

    num_allocs--;
}

void zgl_PrintMemSummary(void) {
    /* This doesn't use zgl_Log since it should be available before and after
       logging system is active. */

    puts_underline("Zigil Memory Summary");
#if ZGL_LINUX
    printf("Remaining unfreed: %i allocs; %i bytes\n", num_allocs, alloc_bytes);
#elif ZGL_WINDOWS
    printf("Remaining unfreed: %i allocs\n", num_allocs);
#endif
}



/* -------------------------------------------------------------------------- */
#elif ZGL_MEMCHECK_LEVEL == 2
/* -------------------------------------------------------------------------- */

#include "src/diblib_local/dibstr.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef ZGL_LINUX
#include <malloc.h>
#endif

static int num_allocs = 0;
#ifdef ZGL_LINUX
static int alloc_bytes = 0;
static int user_alloc_bytes = 0;
static int overhead_bytes = 0;
#endif


typedef struct ListNode {
    struct ListNode *next;
    struct ListNode *prev;
} ListNode;

#define list_init(entry) (entry)->prev = (entry)->next = entry
#define node_to_record(ptr)                                             \
    ((MemRecord *)((char *)(ptr)-(uint64_t)(&((MemRecord *)0)->node)))
#define list_empty(list) ((list)->next == (list))
#define list_head(list) (list)->next
#define list_add_tail(list, entry)              \
    (entry)->next = (list);                     \
    (entry)->prev = (list)->prev;               \
    (list)->prev->next = (entry);               \
    (list)->prev = (entry)
#define list_remove(entry)                      \
    dibassert((entry)->next != NULL);           \
    (entry)->prev->next = (entry)->next;        \
    (entry)->next->prev = (entry)->prev

static const uint64_t SENTRY = 0x12345678;

typedef struct MemRecord {
    uint64_t head_sentry;
    ListNode node;
    size_t req_size;
    char *file;
    int32_t line;
    char *tail_sentry;
} MemRecord;

static ListNode record_list;

static bool _init_mem = false;

static void dump_records(void) {
    ListNode *entry = list_head(&record_list);
    
    uint32_t leaked_byte_count = 0;
    
    bool closing_separator = false;
    if (entry != &record_list) closing_separator = true;

    while (entry != &record_list) {
        MemRecord *rec = node_to_record(entry);
        printf("-------------\n"
               "Leak detected \n"
               "Size: %zu bytes\n"
               "File: %s\n"
               "Line: %i\n",
               rec->req_size, rec->file, rec->line);
        leaked_byte_count += (uint32_t)rec->req_size;
        entry = entry->next;
    }

    if (closing_separator)
        printf("-------------\n\n");

    if (leaked_byte_count > 0) {
        printf("Total leakage: %i bytes\n", leaked_byte_count);
    } else {
        printf("No memory leaks detected\n");
    }
}

static void *create_MemRecord(const char* filestr, int line, size_t req_size) {
    size_t filestr_len = strlen(filestr);
    
    MemRecord *rec = malloc(sizeof(*rec)
                            + req_size
                            + 1 + filestr_len
                            + sizeof(SENTRY));
    if (rec == NULL) return NULL;

    size_t overhead = sizeof(*rec)
        + 0
        + 1 + filestr_len
        + sizeof(SENTRY);

    rec->head_sentry = SENTRY;
    rec->req_size = req_size;
    rec->line = line;

    rec->file = (char *)((char *)rec
                         + sizeof(*rec)
                         + req_size);
    strcpy(rec->file, filestr);

    rec->tail_sentry = ((char *)rec
                        + sizeof(*rec)
                        + req_size
                        + 1 + filestr_len);
    memcpy(rec->tail_sentry, &rec->head_sentry, sizeof(SENTRY));
    
    list_add_tail(&record_list, &rec->node);

    num_allocs++;
#ifdef ZGL_LINUX
    alloc_bytes += (int)malloc_usable_size(rec);
    user_alloc_bytes += (int)req_size;
    overhead_bytes += (int)overhead;
#endif
    
    return rec + 1;
}

static void destroy_MemRecord(const char *file, int line, void *ptr) {
    if (ptr == NULL) {
        return;
    }

    MemRecord *rec = (MemRecord *)ptr - 1;
    if (rec->head_sentry != SENTRY) {
        printf("MEMORY ERROR: Invalid free( [%p] ). \nFILE of allocation: %s : %d\n\n", ptr, file, line);
        abort();
    }

    uint64_t tail_sentry;
    memcpy(&tail_sentry, rec->tail_sentry, sizeof(SENTRY));
    size_t overhead = (char *)rec->tail_sentry + sizeof(SENTRY) - (char *)rec - rec->req_size;
        
    if (tail_sentry != SENTRY) {
        printf("MEMORY ERROR: Corrupt sentry found during free( [%p] )\nFILE: %s : %d\n", ptr, file, line);
        abort();
    }
    
    list_remove(&rec->node);

#ifdef ZGL_LINUX
    alloc_bytes -= (int)malloc_usable_size(rec);
    user_alloc_bytes -= (int)rec->req_size;
    overhead_bytes -= (int)overhead;
#endif
    num_allocs--;

    free(rec);
}


void *_zgl_Malloc(size_t size, char const *file, int line) {
    if ( ! _init_mem) {
        list_init(&record_list);
        _init_mem = true;
    }

    if (size == 0) {
        printf("ZIGIL.MEM.ERROR: Allocation request for 0 bytes %s : %i : zgl_Malloc( %zu )\n", file, line, size);
        abort();
    }
    
    void *ptr = create_MemRecord(file, line, size);
    
    if ( ! ptr) {
        printf("ZIGIL.MEM.ERROR: Failed to allocate memory %s : %i : zgl_Malloc( %zu )\n", file, line, size);
        abort();
    }

    //printf("malloc( %zu )\nFILE: %s : %d\n\n", size, file, line);
    
    return ptr;
}


void *_zgl_Calloc(size_t nmemb, size_t sizem, char const *file, int line) {
    if ( ! _init_mem) {
        list_init(&record_list);
        _init_mem = true;
    }

    if (sizem == 0) {
        printf("ZIGIL.MEM.ERROR: Allocation request for 0 bytes %s : %i : zgl_Calloc( %zu, %zu )\n", file, line, nmemb, sizem);
        abort();
    }
    
    int size = (int)(sizem * nmemb);
    void *ptr = create_MemRecord(file, line, size);

    if ( ! ptr) {
        printf("ZIGIL.MEM.ERROR: Failed to allocate memory %s : %i : zgl_Calloc( %zu, %zu )\n", file, line, nmemb, sizem);
        abort();
    }

    memset(ptr, 0, size);
    
    //printf("calloc( %zu, %zu ) [%p]\nFILE: %s : %d\n\n", nmemb, sizem, ptr, file, line);
    
    return ptr;
}


void *_zgl_Realloc(void *ptr, size_t size, char const *file, int line) {
    if ( ! _init_mem) {
        list_init(&record_list);
        _init_mem = true;
    }

    if (size == 0) {
        printf("ZIGIL.MEM.ERROR: Allocation request for 0 bytes %s : %i : zgl_Realloc( %p, %zu )\n", file, line, ptr, size);
        abort();
    }

    void *new_ptr = create_MemRecord(file, line, size);
    
    if ( ! new_ptr) {
        printf("ZIGIL.MEM.ERROR: Failed to reallocate memory %s : %i : zgl_Realloc( %p, %zu )\n", file, line, ptr, size);
        abort();
    }

    MemRecord *old_rec = (MemRecord *)ptr - 1;
    memcpy(new_ptr, ptr, size < old_rec->req_size ? size : old_rec->req_size);

    _zgl_Free(ptr, file, line);
    
    return new_ptr;
}


void _zgl_Free(void *ptr, char const *file, int line) {
    if ( ! _init_mem) {
        list_init(&record_list);
        _init_mem = true;
    }

    if (ptr == NULL) {
        printf("ZIGIL.MEM.WARN: NULL-pointer passed to zgl_Free() %s : %i\n", file, line);
        return;
    }

    //MemRecord *block = (MemRecord *)ptr - 1;
    //int size = block->byte_size;

    destroy_MemRecord(file, line, ptr);

    //printf("free( %d )\nFILE: %s : %d\n\n", size, file, line);
}


void zgl_PrintMemSummary(void) {
    puts_box("BEGIN Zigil Memory Summary");
#if ZGL_LINUX
    printf("Remaining unfreed: %i alloc%s; %i requested bytes with %i overhead bytes\n", num_allocs, num_allocs == 1 ? "" : "s", user_alloc_bytes, overhead_bytes);
#elif ZGL_WINDOWS
    printf("Remaining unfreed: %i allocs; %i bytes\n", num_allocs, user_alloc_bytes);
#endif

    putchar('\n');
    
    dump_records();

    puts_box("END Zigil Memory Summary");
}

/* -------------------------------------------------------------------------- */
#endif
/* -------------------------------------------------------------------------- */
