#include "Debug.h"
#include "MainRegister.h"
#include "Can_Service.h"
#include "Xenon_Lamp.h"

#ifdef DEBUG

#define DEBUG_TX_TIMEOUT_5MS  (100/5)

volatile uint8_t Debug_Timeout_Cnt = 0;
volatile uint8_t Debug_TxBusyFlag = 0;
volatile uint8_t Debug_HasKeyFlag = 0;
volatile uint8_t Debug_Key = 0;
uint32_t Debug_ResetCounter = 0;
uint8_t Debug_ResetFlag = 0;

#ifdef USE_DEBUG_RINGBUFFER
/********************************************** Use Ringbuffer ***********************************************/
#define DEBUG_TX_BUFFER_LEN     256
#define DEBUG_TX_BUFFER_MASK    (DEBUG_TX_BUFFER_LEN - 1)
uint8_t Debug_TX_Buffer[DEBUG_TX_BUFFER_LEN] = {0};
volatile uint8_t Debug_TX_Header = 0;
volatile uint8_t Debug_TX_Tailer = 0;

#define DEBUG_RX_BUFFER_LEN     256
#define DEBUG_RX_BUFFER_MASK    (DEBUG_RX_BUFFER_LEN - 1)
uint8_t Debug_RX_Buffer[DEBUG_RX_BUFFER_LEN] = {0};
volatile uint8_t Debug_RX_Header = 0;
volatile uint8_t Debug_RX_Tailer = 0;

void Debug_TxCallBack_ISR(void)
{
    if (Debug_TX_Tailer == Debug_TX_Header) {
        Debug_TxBusyFlag = 0;
    } else {
        Debug_Tx_SendData(Debug_TX_Buffer[Debug_TX_Tailer++]);
        Debug_TX_Tailer &= DEBUG_TX_BUFFER_MASK;
    }
}

void Debug_RxCallBack_ISR(uint8_t Data)
{
    Debug_RX_Buffer[Debug_RX_Header++] = Data;
    Debug_RX_Header &= DEBUG_RX_BUFFER_MASK;
}

void Debug_PutChar(char ch)
{
    uint32_t FreeSpace;
    do {
        if (Debug_TX_Header < Debug_TX_Tailer) {
            FreeSpace = Debug_TX_Tailer - Debug_TX_Header;
        } else {
            FreeSpace = Debug_TX_Tailer + DEBUG_TX_BUFFER_MASK + 1 - Debug_TX_Header;
        }
    } while (FreeSpace < 2);
    Debug_TX_Buffer[Debug_TX_Header] = ch;
    if (!Debug_TxBusyFlag) {
        Debug_TxBusyFlag = 1;
        Debug_Tx_SendData(ch);
    } else {
        Debug_TX_Header = (Debug_TX_Header + 1) & DEBUG_TX_BUFFER_MASK;
    }
}

uint8_t Debug_HasKey(void)
{
    if (Debug_RX_Header == Debug_RX_Tailer) {
        return 0;
    } else {
        return 1;
    }
}

void Debug_ClrKeyFlag(void)
{

}

int32_t Debug_GetKey(void)
{
    uint8_t Data = Debug_RX_Buffer[Debug_RX_Tailer++];
    Debug_RX_Tailer &= DEBUG_RX_BUFFER_MASK;
    return Data;
}

#ifdef __GNUC__
int __io_putchar(int ch)
{
    Debug_PutChar(ch);
    return ch;
}
#else
FILE __stdout;
int fputc(int ch, FILE *stream)
{
    Debug_PutChar(ch);
    return ch;
}
#endif

void Debug_Timeout_WDT_Reload(void)
{
    Debug_Timeout_Cnt = DEBUG_TX_TIMEOUT_5MS;
}

void Debug_Timeout_WDT_Service_5ms(void)
{
    if (Debug_Timeout_Cnt > 0) {
        Debug_Timeout_Cnt--;
        if (Debug_Timeout_Cnt == 0) {
            Debug_TxBusyFlag = 0;
        }
    }
}

void Debug_Timeout_WDT_Disable(void)
{
    Debug_Timeout_Cnt = 0;
}

#else
/********************************************** Not Ringbuffer ***********************************************/
void Debug_TxCallBack_ISR(void)
{
    Debug_TxBusyFlag = 0;
}

void Debug_RxCallBack_ISR(uint8_t Data)
{
    Debug_Key = Data;
    Debug_HasKeyFlag = 1;
}

void Debug_Timeout_WDT_Reload(void)
{
    Debug_Timeout_Cnt = DEBUG_TX_TIMEOUT_5MS;
}

void Debug_Timeout_WDT_Service_5ms(void)
{
    if (Debug_Timeout_Cnt > 0) {
        Debug_Timeout_Cnt--;
        if (Debug_Timeout_Cnt == 0) {
            Debug_TxBusyFlag = 0;
        }
    }
}

void Debug_Timeout_WDT_Disable(void)
{
    Debug_Timeout_Cnt = 0;
}

void Debug_PutChar(uint8_t sendData)
{
    Debug_Timeout_WDT_Reload();
    while (Debug_TxBusyFlag) {
    }
    Debug_TxBusyFlag = 1;
    Debug_Tx_SendData(sendData);
    Debug_Timeout_WDT_Disable();
}

uint8_t Debug_HasKey(void)
{
    return Debug_HasKeyFlag;
}

void Debug_ClrKeyFlag(void)
{
    Debug_HasKeyFlag = 0;
}

int8_t Debug_GetKey(void)
{
    return Debug_Key;
}


void printMsg(void *str)
{
    int8_t* String = (int8_t*)str;
    int8_t sendData = *(String++);
    while (sendData) {
        Debug_PutChar((uint8_t)sendData);
        sendData = *(String++);
    }
}

void printData(void *str, int32_t Value)
{
    int8_t* String = (int8_t*)str;
    int8_t sendData = *(String++);
    uint8_t noneZero = 0;
    uint32_t divider = 1000000000;
    int8_t dispValue;

    while (sendData) {
        if (sendData == (int8_t)'%') {
            sendData = *(String++);
            if (sendData == (int8_t)'d' || sendData == (int8_t)'x') {
                if ((sendData == (int8_t)'d') && (Value < 0)) {
                    Debug_PutChar('-');
                    Value *= -1;
                }
                if (Value) {
                    noneZero = 0;
                    divider = 1000000000;
                    if (sendData == (int8_t)'x') { divider = 0x10000000; }
                    while (divider) {
                        dispValue = (int8_t)(Value / divider);
                        Value -= dispValue * divider;
                        if (dispValue) { noneZero = 1; }
                        if (noneZero) {
                            if (dispValue > 9) {
                                dispValue += 55;
                            } else {
                                dispValue += 0x30;
                            }
                            Debug_PutChar(dispValue);
                        }
                        if (sendData == (int8_t)'d') {
                            divider /= 10;
                        } else {
                            divider /= 0x10;
                        }
                    }
                } else {
                    Debug_PutChar('0');
                }
            }
        } else {
            Debug_PutChar(sendData);
        }
        sendData = *(String++);
    }
}
#endif

void Debug_Initial(void)
{
    printData("\nBegin : %d\n", MCU_SYSTEMCOLK);
}

void Debug_Handler(void)
{
    if (SYS_TIM_FLAG_5MS) {
        Debug_Timeout_WDT_Service_5ms();
    }

    if (SYS_TIM_FLAG_1000MS) {
        printData("Second %d\n", (int)SystemTimeBase_Second);
        printData("BusOffCount = %d\n", Get_BusOff_Count());
        printData("R_ENABLE = 0x%X\n", R_ENABLE);
        printData("R_BRIGHTNESS_SET = %d\n", R_BRIGHTNESS_SET);
        printData("R_BRIGHTNESS_CURR = %d\n", R_BRIGHTNESS_CURR);
        printData("R_LIGHT_WORK_HOUR = %d\n", R_LIGHT_WORK_HOUR);
        printData("R_LIGHT_LIFE_HOUR = %d\n", R_LIGHT_LIFE_HOUR);
        printData("R_LIGHT_FILTER_SET = %d\n", R_LIGHT_FILTER_SET);
        printData("R_LIGHT_FILTER_CURR = %d\n", R_LIGHT_FILTER_CURR);
        printData("XenonLampCtrlStep = %d\n", XenonLampCtrlStep);
        printData("XenonLampCurrSet = %.3f\n", XenonLampCurrSet);
        printData("XenonLampVol = %.3f\n", XenonLampVol);
        printData("XenonLampCurr = %.3f\n", XenonLampCurr);
        printData("XenonLampTemp = %.3f\n", XenonLampTemp);
        printData("ModuleError.flg = 0x%lX\n", ModuleError.flg);
        printMsg("\n");
    }

#ifdef USE_DEBUG_RECEIVE
    if (Debug_HasKey()) {
        Debug_ClrKeyFlag();
        Debug_Key = Debug_GetKey();
        Debug_ResetFlag = 0;

        switch (Debug_Key) {
            case 0x7F:
                Debug_ResetFlag = 1;
                break;
            case '0':
                R_BRIGHTNESS_SET = 0;
                break;
            case '1':
                R_BRIGHTNESS_SET = 20;
                break;
            case '2':
                R_BRIGHTNESS_SET = 40;
                break;
            case '3':
                R_BRIGHTNESS_SET = 60;
                break;
            case '4':
                R_BRIGHTNESS_SET = 80;
                break;
            case '5':
                R_BRIGHTNESS_SET = 100;
                break;
            default:
                break;
        }
        if (Debug_ResetFlag == 1) {
            if (++Debug_ResetCounter >= 100) {
                NVIC_SystemReset();
            }
        } else {
            Debug_ResetCounter = 0;
        }
    }
#endif /* USE_DEBUG_RECEIVE */
}

#endif
