#ifndef ZIGIL_MEM_H
#define ZIGIL_MEM_H

#include <stddef.h>
#include <stdlib.h>

#define ZGL_MEMCHECK_LEVEL 0

#if ZGL_MEMCHECK_LEVEL == 0
extern void zgl_PrintMemSummary(void);

# define zgl_Malloc(SIZE) malloc((SIZE))
# define zgl_Calloc(NMEMB, SIZE) calloc((NMEMB), (SIZE))
# define zgl_Realloc(PTR, SIZE) realloc((PTR), (SIZE))
# define zgl_Free(PTR) free((PTR))

#elif ZGL_MEMCHECK_LEVEL == 1
extern void *_zgl_Malloc(size_t size, char const *file, int line);
extern void *_zgl_Calloc(size_t nmemb, size_t size, char const *file, int line);
extern void *_zgl_Realloc(void *ptr, size_t size, char const *file, int line);
extern void _zgl_Free(void *ptr, char const *file, int line);
extern void zgl_PrintMemSummary(void);

# define zgl_Malloc(SIZE) _zgl_Malloc((SIZE), __FILE__, __LINE__)
# define zgl_Calloc(NMEMB, SIZE) _zgl_Calloc((NMEMB), (SIZE), __FILE__, __LINE__)
# define zgl_Realloc(PTR, SIZE) _zgl_Realloc((PTR), (SIZE), __FILE__, __LINE__)
# define zgl_Free(PTR) _zgl_Free((PTR), __FILE__, __LINE__)

#elif ZGL_MEMCHECK_LEVEL == 2
extern void *_zgl_Malloc(size_t size, char const *file, int line);
extern void *_zgl_Calloc(size_t nmemb, size_t size, char const *file, int line);
extern void *_zgl_Realloc(void *ptr, size_t size, char const *file, int line);
extern void _zgl_Free(void *ptr, char const *file, int line);
extern void zgl_PrintMemSummary(void);

# define zgl_Malloc(SIZE) _zgl_Malloc((SIZE), __FILE__, __LINE__)
# define zgl_Calloc(NMEMB, SIZE) _zgl_Calloc((NMEMB), (SIZE), __FILE__, __LINE__)
# define zgl_Realloc(PTR, SIZE) _zgl_Realloc((PTR), (SIZE), __FILE__, __LINE__)
# define zgl_Free(PTR) _zgl_Free((PTR), __FILE__, __LINE__)
#endif

#endif /* ZIGIL_MEM_H */
