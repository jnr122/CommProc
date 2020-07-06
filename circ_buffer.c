//
// Created by Jonah Rubin on 2020-07-01.
//

#include "circ_buffer.h"
#include <stdio.h>
#include <stdlib.h>

#define START_FLAG 'S'
#define CRC1 'C'
#define CRC2 'R'

int main() {
    Circ_Buffer cb = {.head=0, .tail=0};
    char input, nl;

    disp_buff(&cb);

    while (TRUE) {
        // get input and snag new line
        input = getchar();
        nl = getchar();

        insert(&cb, &input);
        disp_buff(&cb);

    }
}

// read in to circ buffer, once packet size is reached check crc.
//
// if good make packet and throw out
// else search for next start in buffer
//      if there is a start try to make a new packet
//          if enough, make new packet and check, restart
//          else wait till new buffer input, try again
void insert(Circ_Buffer *cb, const char *c) {
    cb->frame[cb->head] = *c;

    // if head-tail = distance then packet size reached
    if (get_distance(cb->head, cb->tail) == FRAME_SIZE-1) {

//        // slide tail until tail hits a start flag or catches head
//        while (cb->frame[cb->tail] != START_FLAG && cb->tail != cb->head) {
////            circulate(&cb->head);
//            circulate(&cb->tail);
//        }

        if (next_start(cb))
            make_packet(cb);


    } else
        circulate(&cb->head);
}

int next_start(Circ_Buffer *cb) {

    // while there is still another tail, send it back for packet making

    for (int i = cb->tail; i != cb->head; circulate(&i)) {

        if (cb->frame[i] == START_FLAG) {
            cb->tail = i;
            return TRUE;
        }
    }

    return FALSE;
}

void make_packet(Circ_Buffer *cb) {
    int j = 0;

    for (int i = cb->tail; i != cb->head; circulate(&i)) {
        cb->p.frame[j] = cb->frame[i];
        ++j;
    }

    checkCRC(&cb->p);
    disp_packet(&cb->p);

    if (get_distance(cb->head, cb->tail) == FRAME_SIZE -1) {
        ++cb->tail;
        if (!next_start(cb)) {
            cb->tail = cb->head;
        }
    }



    // no start within window size

    // start within window size

}

void checkCRC(Packet *p) {
    p->corrupt = (p->frame[1] != CRC1 || p->frame[2] != CRC2);
}

// dist [,)
int get_distance(int head, int tail) {
    if (head >= tail)
        return head-tail;
    else
        return head+(BUFFER_SIZE-tail);
}

void circulate(int *loc) { *loc = (*loc+1) % BUFFER_SIZE; }

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

int disp_packet(const Packet *p) {
    printf("Head at %d, done: %d, corrupt: %d\n", p->head, p->done, p->corrupt);
    for (int i = 0; i < FRAME_SIZE; ++i) {
        printf("%d: ", i);
        printf("%c\n", p->frame[i]);
    }
    printf("\n");
}