//
// Created by Jonah Rubin on 2020-07-01.
//

#include "circ_buffer.h"
#include <stdio.h>
#include <stdlib.h>

#define START_FLAG 'S'
#define CRC1 'C'
#define CRC2 'R'

/**
 * Add new input to buffer
 * @param cb the buffer
 * @param c the new character
 */
void insert(Circ_Buffer *cb, const char *c) {
    cb->frame[cb->head] = *c;
    circulate(&cb->head);
    if (get_distance(cb->head, cb->tail) == FRAME_SIZE)
        make_packet(cb);
}

/**
 * Move tail up to next start, or until it reaches the head
 * @param cb the buffer
 * @return TRUE if there is a next start
 */
int next_start(Circ_Buffer *cb) {
    for (int i = cb->tail; i != cb->head; circulate(&i)) {
        if (cb->frame[i] == START_FLAG) {
            cb->tail = i;
            return TRUE;
        }
    }
    return FALSE;
}

/**
 * Use head and tail location to make a packet
 * @param cb the buffer
 */
void make_packet(Circ_Buffer *cb) {
    if (cb->frame[cb->tail] == START_FLAG) {
        int j = 0;
        for (int i = cb->tail; i != cb->head; circulate(&i)) {
            cb->p.frame[j] = cb->frame[i];
            ++j;
        }

        checkCRC(&cb->p);
        // decide to do with packet based on corrupt value of CRC check here

        disp_packet(&cb->p);
    }
    // move the tail to the head after finishing packet
    if (get_distance(cb->head, cb->tail) == FRAME_SIZE) {
        ++cb->tail;
        if (!next_start(cb)) {
            cb->tail = cb->head;
        }
    }

}

/**
 * Validate CRC of packet, standin for now
 * @param p the packet
 */
void checkCRC(Packet *p) {
    p->corrupt = (p->frame[1] != CRC1 || p->frame[2] != CRC2);
}

/**
* @param head
* @param tail
* @return dist [,) of head from tail
*/
int get_distance(int head, int tail) {
    if (head >= tail)
        return head-tail;
    else
        return head+(BUFFER_SIZE-tail);
}

/**
 * Increment around a loop
 * @param loc current location
 */
void circulate(int *loc) {
    *loc = (*loc+1) % BUFFER_SIZE;
}

/**
 * Display buffer
 * @param cb the buffer
 */
void disp_buff(const Circ_Buffer *cb) {
    printf("Head at %d\n", cb->head);
    for (int i = 0; i < BUFFER_SIZE; ++i) {
        printf("%d: ", i);

        if (i == cb->head && i == cb->tail)
            printf("%c  <-H,T\n", cb->frame[i]);
        else if (i == cb->head)
            printf("%c  <-H\n", cb->frame[i]);
        else if (i == cb->tail)
            printf("%c  <-T\n", cb->frame[i]);
        else
            printf("%c\n", cb->frame[i]);

    }
    printf("\n");
}

/**
 * Display packet
 * @param p the packet
 */
void disp_packet(const Packet *p) {
    printf("\n----------------\n");
    printf("corrupt: %d\n", p->corrupt);
    for (int i = 0; i < FRAME_SIZE; ++i)
        printf("%c", p->frame[i]);
    printf("\n----------------\n");
    printf("\n");
}