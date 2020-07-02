//
// Created by Jonah Rubin on 2020-07-01.
//

#ifndef COMMPROTOCOL_CIRC_BUFFER_H
#define COMMPROTOCOL_CIRC_BUFFER_H

#include <stdint.h>
#define BUFFER_SIZE 10
#define FRAME_SIZE 6
#define TRUE 1
#define FALSE 0

typedef struct Circ_Buffer {
    int head, tail;
    char frame[BUFFER_SIZE];

} Circ_Buffer;

void insert(Circ_Buffer *cb, const char *c);
void circulate(int *loc);
void disp_buff(const Circ_Buffer *cb);
int get_distance(int head, int tail);

#endif //COMMPROTOCOL_CIRC_BUFFER_H