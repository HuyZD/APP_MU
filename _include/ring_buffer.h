/**
 * \file ring_buffer.h
 * \brief Ring (circular) buffer API.
 *
 * A ring (circular) buffer implementation for use in the
 * serial port driver.
 *
 * To avoid the use of malloc to dynamically allocate the
 * ring-buffer structure and its internal buffer, the
 * initialization routine takes a previously allocated
 * ring-buffer structure and data buffer. For example, the
 * serial port driver structure contains a ring buffer
 * for transmit and another for receive.
 */
#ifndef RING_BUFFER_H
#define RING_BUFFER_H

/* Ring-buffer state */
typedef struct ring_buffer {
	unsigned int  wr;
	unsigned int  rd;
	unsigned int  count;
	unsigned int  size;
	char         *data;
} ring_buffer_t;

/* Initialize a statically allocated ring buffer with a
 * statically allocated data buffer data[size];
 */
void ring_buffer_init(
	unsigned int      size,
	char             *data,
	ring_buffer_t    *buf);

/* Remove a character from the ring buffer
 * (returns -1 on empty)
 */
int ring_buffer_remove(
	ring_buffer_t *buf);

/* Add a character to the ring buffer
 * (returns -1 on full)
 */
int ring_buffer_add(
	ring_buffer_t *buf,
	char           ch);

/* Number of characters in the buffer */
unsigned int ring_buffer_count(
	ring_buffer_t *buf);

#endif
