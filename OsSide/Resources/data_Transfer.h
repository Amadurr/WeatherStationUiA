
#ifndef DATA_TRANSFER_H__
#define DATA_TRANSFER_H__
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef struct{
	uint8_t flags;	//signal flags
	//bit[0] = read/write
	void *p;		//payload package
	uint8_t sId; //senderID
	uint8_t rId; //recieverID
}inMail_t; //internal Mail


#endif //DATA_TRANSFER_H__