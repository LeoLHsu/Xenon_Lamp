#include "Can_Service.h"

uint8_t canTxData[CAN_DATA_LEN] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
uint8_t canRxData[CAN_DATA_LEN] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
FDCAN_RxHeaderTypeDef canRxHeader;

void CAN_Filter_Config(void)
{
    FDCAN_FilterTypeDef  sFilterConfig;
    sFilterConfig.IdType =  FDCAN_STANDARD_ID;
    sFilterConfig.FilterIndex = 0;
    sFilterConfig.FilterType = FDCAN_FILTER_MASK;
    sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
    sFilterConfig.FilterID1 = CAN_HOST_ID;
    sFilterConfig.FilterID2 = 0x07FF;       // Only CAN_HOST_ID

    HAL_FDCAN_ConfigFilter(&HOST_CAN_HANDLE, &sFilterConfig);
    HAL_FDCAN_ConfigGlobalFilter(&HOST_CAN_HANDLE, FDCAN_REJECT, FDCAN_REJECT, FDCAN_REJECT_REMOTE, FDCAN_REJECT_REMOTE);
}

void CAN_Initial(void)
{
    CAN_Filter_Config();
    HAL_FDCAN_Start(&HOST_CAN_HANDLE);
}

void CAN_WriteData(uint32_t id, uint8_t len)
{
    FDCAN_TxHeaderTypeDef TxHeader;

    if (len <= 8) {
        TxHeader.Identifier = id;
        TxHeader.IdType = FDCAN_STANDARD_ID;
        TxHeader.TxFrameType = FDCAN_DATA_FRAME;
        if (FDCAN_DLC_BYTES_1 == 0x00010000) {
            TxHeader.DataLength = (len << 16);
        } else {
            TxHeader.DataLength = len;
        }
        TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
        TxHeader.BitRateSwitch = FDCAN_BRS_OFF;
        TxHeader.FDFormat = FDCAN_CLASSIC_CAN;
        TxHeader.TxEventFifoControl = FDCAN_STORE_TX_EVENTS;
        TxHeader.MessageMarker = 0x01;

        HAL_FDCAN_AddMessageToTxFifoQ(&HOST_CAN_HANDLE, &TxHeader, canTxData);
    }
}

uint8_t CAN_ReadData(uint8_t *buf)
{
    if (HAL_FDCAN_GetRxFifoFillLevel(&HOST_CAN_HANDLE, FDCAN_RX_FIFO0)) {
        if (HAL_FDCAN_GetRxMessage(&HOST_CAN_HANDLE, FDCAN_RX_FIFO0, &canRxHeader, buf) == HAL_OK) {
            if (FDCAN_DLC_BYTES_1 == 0x00010000) {
                return canRxHeader.DataLength >> 16;
            } else {
                return canRxHeader.DataLength;
            }
        }
    }
    return 0;
}

uint8_t CAN_Response_ReadReg(uint32_t id, uint16_t addr, uint8_t len)
{
    uint16_t size = 0;
    uint16_t *pdata = 0;
    pdata = GetMainReg(addr, &size);
    if ((size <= 0) || (pdata == 0)) {
        return 1;
    }
    uint8_t pos = 0;
    canTxData[pos++] = CAN_HOST_ID;
    canTxData[pos++] = CAN_OPERATE_CODE_READ_RESPONSE;
    canTxData[pos++] = ((addr >> 8) & 0xFF);
    canTxData[pos++] = ((addr    ) & 0xFF);
    if (len == 1) {
        canTxData[pos++] = ((*pdata >> 8) & 0xFF);
        canTxData[pos++] = ((*pdata    ) & 0xFF);
    } else {    // len == 2
        canTxData[pos++] = ((*pdata >> 8) & 0xFF);
        canTxData[pos++] = ((*pdata    ) & 0xFF);
        pdata += 1;
        canTxData[pos++] = ((*pdata >> 8) & 0xFF);
        canTxData[pos++] = ((*pdata    ) & 0xFF);
    }
    CAN_WriteData(id, pos);
    return 0;
}

uint8_t CAN_Response_WriteReg(uint32_t id, uint16_t addr, uint8_t len)
{
    uint16_t size = 0;
    uint16_t *pdata = 0;
    pdata = GetMainReg(addr, &size);
    if ((size <= 0) || (pdata == 0)) {
        return 1;
    }
    uint8_t pos = 0;
    canTxData[pos++] = CAN_HOST_ID;
    canTxData[pos++] = CAN_OPERATE_CODE_WRITE_RESPONSE;
    canTxData[pos++] = ((addr >> 8) & 0xFF);
    canTxData[pos++] = ((addr    ) & 0xFF);
    if (len == 1) {
        canTxData[pos++] = ((*pdata >> 8) & 0xFF);
        canTxData[pos++] = ((*pdata    ) & 0xFF);
    } else {    // len == 2
        canTxData[pos++] = ((*pdata >> 8) & 0xFF);
        canTxData[pos++] = ((*pdata    ) & 0xFF);
        pdata += 1;
        canTxData[pos++] = ((*pdata >> 8) & 0xFF);
        canTxData[pos++] = ((*pdata    ) & 0xFF);
    }
    CAN_WriteData(id, pos);
    return 0;
}

/*CAN MESSAGE PROTOCOL
 * 1. CAN data 6-8 bytes；
 * 2. CAN message： nodeid+operateCode+addressHigh8bit+addressLow8bit+value1High8bit+value1Low8bit+value2High8bit+value2Low8bit
 * value1 对应的是address的数值；value2 对应的是address+1的数值,value2可以没有。
 * 3.nodeid 对应的是RS485的节点定义（1-254），数据发出方的ID.
 * 4. address对应的modbus address。
 * 5. operateCode 03H=读register，83H=register告知；06H=修改register，86H=修改register的告知。
 */
uint8_t CAN_ParseMessage(uint8_t *pData, uint8_t len)
{
    uint8_t result = 0;
    uint16_t addr = 0, size = 0, value = 0;

    if (len < 4 || len > 8) {
        result = 1;
    } else {
        switch (pData[0]) {
            case CAN_SLAVE_ID:
                if (pData[1] == CAN_OPERATE_CODE_READ) {
                    addr = pData[2];
                    addr = (addr << 8);
                    addr += pData[3];
                    size = pData[4];
                    size = (size << 8);
                    size += pData[5];
                    return CAN_Response_ReadReg(pData[0], addr, size);
                } else if (pData[1] == CAN_OPERATE_CODE_WRITE) {
                    addr = pData[2];
                    addr = (addr << 8);
                    addr += pData[3];
                    value = pData[4];
                    value = (value << 8);
                    value += pData[5];
                    WriteMainReg(addr, value);
                    return CAN_Response_WriteReg(pData[0], addr, 1);
                }
                result = 0;
                break;
            default:
                result = 2;
                break;
        }
    }

    return result;
}

void Can_Error_Service(void)
{
    static Module_Error_Union_t Error_Temp = {0};
    uint8_t i = 0;
    Error_Operate_t action;

    if (Error_Temp.flg != ModuleError.flg) {
        for (i = 0; i < 32; i++) {
            if (((Error_Temp.flg >> i) & 0x0001) != ((ModuleError.flg >> i) & 0x0001)) {
                if (((ModuleError.flg >> i) & 0x0001)) {
                    action = REPORT;
                } else {
                    action = CLEAN;
                }
                R_ERROR_INFO = (action << 15) | ModuleErrorList[i];
                CAN_Response_ReadReg(CAN_SLAVE_ID, R_ERROR_INFO_ADDR, 1);
            }
        }
        Error_Temp.flg = ModuleError.flg;
    }
}

void CAN_Handler(void)
{
    uint8_t rxLenth = 0;

    while (!!(rxLenth = CAN_ReadData(canRxData))) {
        CAN_ParseMessage(canRxData, rxLenth);
    }

    Can_Error_Service();
}

/**************************END OF FILE************************************/

