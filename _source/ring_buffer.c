/**
 * \file ring_buffer.c
 * \brief Ring (circular) buffer API.
 */
//#include <includes.h>
#include "ring_buffer.h"

void ring_buffer_init(
	unsigned int   size,
	char          *data,
	ring_buffer_t *buf)
{
	buf->wr    = 0;
	buf->rd    = 0;
	buf->count = 0;
	buf->size  = size;
	buf->data  = data;
}

int ring_buffer_remove(
	ring_buffer_t *buf)
{
	int ch;
	/* Empty check */
	if (buf->count == 0) {
		return -1;
	}
	ch = buf->data[buf->rd];
	buf->rd++;
	buf->count--;
	/* Buffer read-pointer wrap */
	if (buf->rd == buf->size) {
		buf->rd = 0;
	}
	return ch;
}

int ring_buffer_add(
	ring_buffer_t *buf,
	char           ch)
{
	/* Full check */
	if (buf->count == buf->size) {
		return -1;
	}
	buf->data[buf->wr] = ch;
	buf->wr++;
	buf->count++;
	/* Buffer write-pointer wrap */
	if (buf->wr == buf->size) {
		buf->wr = 0;
	}
	return 0;
}

unsigned int ring_buffer_count(
	ring_buffer_t *buf)
{
	return buf->count;
}

