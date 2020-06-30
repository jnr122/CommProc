//
// Created by Jonah Rubin on 2020-06-29.
//

#include <stdio.h>
#include "packet.h"

#define BUFFER_SIZE 1
#define START_FLAG 'S'
#define END_FLAG 'E'

int main() {
    Packet p = {.head=0, .done=FALSE};
    char input, nl;

    print_packet(&p);

    for (int i = 0; i < FRAME_SIZE; ++i) {
        // get input and snag new line
        input = getchar();
        nl = getchar();

        process(&p, &input);

        print_packet(&p);

    }

}

// process first. if start is seen before end, start over
int process(Packet *p, const char *c) {

    // starting flag found, reset head and done indicator
    if (*c == START_FLAG) {
        p->done = FALSE;
        p->head = 0;
        p->frame[p->head] = *c;
        ++p->head;

    // end flag found, check to make sure
    } else if (*c == START_FLAG) {
        if (p->head == FRAME_SIZE) {
            p->done = TRUE;
        }
    }


    return 0;
}

int print_packet(const Packet *p) {
    printf("Head at %d, done: %d\n", p->head, p->done);
    for (int i = 0; i < FRAME_SIZE; ++i) {
        printf("%d: ", i);
        printf("%c\n", p->frame[i]);
    }
    printf("\n");
}