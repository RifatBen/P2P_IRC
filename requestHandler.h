#ifndef requestHandler_h
#define requestHandler_h

#include "stdint.h"
#include "includes.h"
#include "structs.h"


typedef __int128 int128_t;
typedef unsigned __int128 uint128_t;




void numberToByte(uint64_t number, unsigned char *req, int size);
uint64_t byteToNumber(unsigned char *req, int size);

void newHelloCourt(TLV *message, uint64_t id);

void newPad1(TLV *message);

void newPadN(TLV *message);

void newHelloLong(TLV *message, uint64_t sourceid, uint64_t destid);

void newNeighbour(TLV *message, uint128_t ip, uint16_t port);

void newData(TLV *message, uint64_t senderid, uint32_t nonce, unsigned char type,  char *data );

void newAck(TLV *message, uint64_t senderid, uint64_t nonce);




int Verif(char *req,int length_TLV);
void createRequest(unsigned char *req, TLV tlv);
void checkRecieved (TLV tlv);
#endif