#ifndef _CAN_SERVICE_H_
#define _CAN_SERVICE_H_
#include "fdcan.h"
#include "MainRegister.h"

#define HOST_CAN_HANDLE    hfdcan1

#define CAN_HOST_ID         0x10
#define CAN_SLAVE_ID        0x11        // module itself
#define CAN_DATA_LEN        8

/*CAN MESSAGE PROTOCOL
 * 1. CAN data 6-8 bytes；
 * 2. CAN message： nodeid+operateCode+addressHigh8bit+addressLow8bit+value1High8bit+value1Low8bit+value2High8bit+value2Low8bit
 * value1 对应的是address的数值；value2 对应的是address+1的数值,value2可以没有。
 * 3.nodeid 对应的是RS485的节点定义（1-254），数据发出方的ID.
 * 4. address对应的modbus address。
 * 5. operateCode 03H=读register，83H=register告知；06H=修改register，86H=修改register的告知。
 */
#define CAN_OPERATE_CODE_READ                   0x03
#define CAN_OPERATE_CODE_READ_RESPONSE          (0x80 | CAN_OPERATE_CODE_READ)
#define CAN_OPERATE_CODE_WRITE                  0x06
#define CAN_OPERATE_CODE_WRITE_RESPONSE         (0x80 | CAN_OPERATE_CODE_WRITE)

extern void CAN_WriteData(uint32_t id, uint8_t len);
extern uint8_t CAN_ReadData(uint8_t *buf);

extern void CAN_Initial(void);
extern void CAN_Handler(void);

#endif







