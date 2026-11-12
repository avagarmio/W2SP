#include <assert.h>
#include "ringbuf.h"
#include <stdio.h>

int main(void)
{
    struct ringbuf *rb = rb_create(5);
    assert(rb != NULL);
    assert(rb_capacity(rb) == 5);
    assert(rb_size(rb) == 0);
    rb_destroy(rb);

    assert(rb_create(0) == NULL);
    rb_destroy(NULL);

    printf("Done!\n");
    return 0;
}