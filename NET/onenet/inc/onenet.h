#ifndef _ONENET_H_
#define _ONENET_H_


typedef struct
{

	int fengshan;

} FENGSHAN_STATUS;

extern FENGSHAN_STATUS fengshan_status;


_Bool OneNet_DevLink(void);

void OneNet_SendData(void);

void OneNet_RevPro(unsigned char *cmd);

#endif
