#include <cx_api.h>
#include <stdlib.h>
#include <memory.h>


const cx_status_t cx_ok = {0};


void* cx_new_copy(size_t size, const void *src)
{
    void *p = calloc(size, 1);
    memcpy(p, src, size);
    return p;
}
