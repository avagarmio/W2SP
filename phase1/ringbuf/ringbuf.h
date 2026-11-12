#ifndef RINGBUF_H
#define RINGBUF_H

#include <stddef.h>

struct ringbuf;   /* непрозрачный тип: поля видны только в ringbuf.c */

struct ringbuf *rb_create(size_t capacity);   /* NULL при ошибке */
void   rb_destroy(struct ringbuf *rb);        /* rb_destroy(NULL) безопасен */

size_t rb_write(struct ringbuf *rb, const void *data, size_t len);
size_t rb_read(struct ringbuf *rb, void *buf, size_t len);

size_t rb_size(const struct ringbuf *rb);     /* сколько байтов лежит */
size_t rb_capacity(const struct ringbuf *rb);

#endif /* RINGBUF_H */
