//
// Created by Jonah Rubin on 2020-06-29.
//

#include <stdio.h>
#include "packet.h"

#define BUFFER_SIZE 1
#define START_FLAG 'S'
#define END_FLAG 'E'

int main() {
    Packet p = {.head=0, .done=FALSE, .corrupt=FALSE};
    char input, nl;

    print_packet(&p);

    while (TRUE) {
        // get input and snag new line
        input = getchar();
        nl = getchar();

        process(&p, &input);

        print_packet(&p);

    }

}

int process(Packet *p, const char *c) {

    // start flag found, reset 'head' and 'done' indicator
    if (*c == START_FLAG) {

        // start flag not at the beginning, someone else is talking
        if (p->head != 0) {
            printf("overwritten, starting packet over\n");
        }

        // reset
        p->done = FALSE;
        p->corrupt = FALSE;
        p->head = 0;
        p->frame[p->head] = *c;
        ++p->head;

    // end flag found, check to make sure actually at frame end, else note corruption
    } else if (*c == END_FLAG) {
        if (p->head == FRAME_SIZE-1) {
            p->done = TRUE;
        } else {
            p->corrupt = TRUE;
        }
        // set end flag and then reset head
        p->frame[p->head] = *c;
        p->head = 0;

    // make sure message fits within start and end flags
    } else if (p->head>0 && p->head<FRAME_SIZE-1){
        p->frame[p->head] = *c;
        ++p->head;
    } else {
        p->corrupt = TRUE;
        p->head = 0;
    }

    return 0;
}

int print_packet(const Packet *p) {
    printf("Head at %d, done: %d, corrupt: %d\n", p->head, p->done, p->corrupt);
    for (int i = 0; i < FRAME_SIZE; ++i) {
        printf("%d: ", i);
        printf("%c\n", p->frame[i]);
    }
    printf("\n");
}