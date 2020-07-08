#include <stdint.h>

//
// Created by Jonah Rubin on 2020-07-01.
//

#ifndef COMMPROTOCOL_CIRC_BUFFER_H
#define COMMPROTOCOL_CIRC_BUFFER_H

#define BUFFER_SIZE 10
#define FRAME_SIZE 6
#define TRUE 1
#define FALSE 0

// packet
typedef struct Packet {
    int corrupt;
    char frame[FRAME_SIZE];

} Packet;

// buffer
typedef struct Circ_Buffer {
    uint16_t head, tail;
    char frame[BUFFER_SIZE];
    Packet p;

} Circ_Buffer;

// packet
void make_packet(Circ_Buffer *cb);
void disp_packet(const Packet *p);
void checkCRC(Packet *p);

// CRC check
uint16_t CRC_CCITT(uint16_t length, uint8_t *source);
void calcPacketCRC(uint16_t *CRC, uint8_t *object, uint16_t size);
void calcCRC(uint16_t *CRC, uint8_t DataByte);

// buffer
void insert(Circ_Buffer *cb, const char *c);
void circulate(int *loc);
void disp_buff(const Circ_Buffer *cb);
uint16_t get_distance(uint16_t head, uint16_t tail);
int next_start(Circ_Buffer *cb);


#endif //COMMPROTOCOL_CIRC_BUFFER_H
