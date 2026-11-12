#include <stdlib.h>   /* malloc, free */
#include <string.h>   /* memcpy */
#include "ringbuf.h"

struct ringbuf {
    unsigned char *buf_;
    size_t capacity_;
    size_t read_pos_;
    size_t write_pos_;
    size_t count_;
};

struct ringbuf *rb_create(size_t capacity)
{
    if(capacity == 0) return NULL;

    struct ringbuf *rb = malloc(sizeof *rb);
    if(rb == NULL) return NULL;

    rb->buf_ = malloc(sizeof(char) * capacity);
    if(rb->buf_ == NULL) {
        free(rb);
        return NULL;
    }

    rb->capacity_ = capacity;
    rb->read_pos_ = 0;
    rb->write_pos_ = 0;
    rb->count_ = 0;


    return rb;
}

void rb_destroy(struct ringbuf *rb)
{
    if(rb != NULL){
        free(rb->buf_);
    }   

    free(rb);
}

size_t rb_size(const struct ringbuf *rb)
{
    return rb->count_;
}

size_t rb_capacity(const struct ringbuf *rb)
{
    return rb->capacity_;
}

size_t rb_write(struct ringbuf *rb, const void *data, size_t len)
{
    size_t bytes_written = 0;
    size_t empty_space = rb->capacity_ - rb->count_;
    const unsigned char *src = data;

    for(size_t i = 0; i < len; ++i){
        if(rb->write_pos_ +1 > rb->capacity_){
            rb->write_pos_ = 0;
        }
        if(empty_space == bytes_written) return bytes_written;

        rb->buf_[rb->write_pos_++] = src[i];
        bytes_written++;
        rb->count_++;
    }

    return bytes_written;
}

size_t rb_read(struct ringbuf *rb, void *buf, size_t len)
{
    size_t bytes_read = 0;
    size_t available = rb->count_;
    unsigned char *out = buf;

    for(size_t i = 0; i < len; ++i){
        if(rb->read_pos_ +1 > rb->capacity_){
            rb->read_pos_ = 0;
        }

        if(available == bytes_read) return bytes_read;

        out[i] = rb->buf_[rb->read_pos_++];
        bytes_read++;
        rb->count_--;
    }

    return bytes_read;
}
