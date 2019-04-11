#ifndef requestHandler_h
#define requestHandler_h

#include "includes.h"
#include "structs.h"






void numberToByte(uint64_t number, unsigned char *req, int size);
uint64_t byteToNumber(unsigned char *req, int size);

void newHelloCourt(TLV *message, uint64_t id);

void newPad1(TLV *message);

void newPadN(TLV *message);

void newHelloLong(TLV *message, uint64_t sourceid, uint64_t destid);

void newNeighbour(TLV *message, uint128_t ip, uint16_t port);

void newData(TLV *message, uint64_t senderid, uint32_t nonce, unsigned char type,  char *data );

void newAck(TLV *message, uint64_t senderid, uint64_t nonce);




int Verif(unsigned char *req,int taille, int tlvLength);
void decomposeRequest(unsigned char *req, TLV *tlv);
void createRequest(unsigned char *req, TLV tlv);
void checkRecieved (TLV tlv,struct sockaddr_in6 peer);
#endif