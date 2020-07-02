//
// Created by Jonah Rubin on 2020-07-01.
//

#include "circ_buffer.h"
#include <stdio.h>

#define START_FLAG 'S'

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

        // slide head and tail until tail hits a start flag - maybe just tail? slide until when?
        while (cb->frame[cb->tail] != START_FLAG && cb->tail != cb->head) {
//            circulate(&cb->head);
            circulate(&cb->tail);
        }

        // make packet and check CRC
        for (int i = cb->tail; i != cb->head-1; circulate(&i)) {
            // if success, do something and throw packet out, move tail up to head


            // if failure, slide until when?
        }
    } else
        circulate(&cb->head);


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