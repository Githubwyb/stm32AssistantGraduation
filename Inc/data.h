#ifndef _DATA_H_
#define _DATA_H_

#include "types.h"

#define BLOCK_NUM 11
#define BLOCK_MAX_SIZE 4096
#define BLOCK_START_ADDRESS 0x00000000
#define BLOCK_DATA_SIZE 10 * 4096
#define SETTING_FLAG 0x01
#define MAX_ID_SIZE 16

typedef enum
{
    FPGA_MODE_LINE = 0x00,
    FPGA_MODE_LOG = 0x01,
    FPGA_MODE_NUM
} FPGA_MODE_ENUM;

typedef struct
{
    u8 flag;
    u8 ignoreNum;
    u8 averageNum;
    u8 mode;
    u8 currentIndex;
    u8 haveMeasuredNum;
    u16 sum;
    float R;
    u32 startFre;
    u32 endFre;
    u32 blockAddress[BLOCK_NUM];
    u16 blockSize[BLOCK_NUM];
    u8 id[MAX_ID_SIZE + 1];
} SETTING;

typedef struct
{
    u8 isScram;     //急停键
    u8 isStart;
    u8 recievedFlag;
    u8 isFirst;
    u8 isIgnore;
    u8 getIndex;
    u8 fpgaFreChoose;
    double curFre;
    u32 curRealFre;
    u32 divFre;
    u32 lastDiv;
    FPGA_RESULT fpgaResult;
} DATA;

DATA *data_getData(void);
int data_initData(void);
SETTING *data_getSetting(void);
int data_loadSetting(void);
int data_saveSetting(void);
int data_showData(void);
int data_showSetting(void);
void showResult(FPGA_RESULT result);
u8 data_getRecieved(void);
u8 data_getIgnoreNum(void);
u8 data_getAverageNum(void);
int data_resetRecieved(void);
int data_plusRecieved(void);
u8 data_isStart(void);
u8 data_isIgnore(void);
int data_setIsIgnore(u8 state);
u8 data_isScram(void);
int data_setIsScram(u8 state);
int data_setIsStart(u8 state);
int data_setInputResult(Complex result);
int data_setOutputResult(Complex result);
int data_plusInputResult(Complex result);
int data_plusOutputResult(Complex result);
int data_resetInputResult(void);
int data_resetOutputResult(void);
u32 data_ntohl(const u8 *data);
u16 data_ntohs(const u8 *data);
int data_htonl(u32 num, u8 *data);
int data_htons(u16 num, u8 *data);

#endif // !_DATA_H_
