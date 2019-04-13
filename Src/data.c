#include "data.h"

#include "flash.h"
#include "log.h"

static DATA data = {0};
static SETTING setting = {0};

DATA *data_getData(void)
{
    return &data;
}

int data_initData(void)
{
    data.isStart = L_FALSE;
    data.getIndex = setting.currentIndex;
    data.recievedFlag = 0;
    data.isFirst = L_TRUE;
    return F_SUCCESS;
}

SETTING *data_getSetting(void)
{
    return &setting;
}

int data_loadSetting(void)
{
    LOG_DEBUG("load setting...");
    if (F_FAILED == flash_read(0x000000, &setting, sizeof(SETTING)) || setting.flag != SETTING_FLAG)
    {
        LOG_DEBUG("load setting failed");
        LOG_DEBUG("expect flag 0x%02x but 0x%02x", SETTING_FLAG, setting.flag);
        return F_FAILED;
    }
    else
    {
        setting.id[MAX_ID_SIZE] = 0x00;
        LOG_DEBUG("load setting success");
    }

    return F_SUCCESS;
}

int data_saveSetting(void)
{
    LOG_DEBUG("save setting...");

    setting.flag = SETTING_FLAG;
    if (F_FAILED == flash_eraseDataBlock(setting.blockAddress[0], setting.blockSize[0] / 4096 + 1))
    {
        LOG_ERROR("setting erase error");
        return F_FAILED;
    }

    if (F_FAILED == flash_write(setting.blockAddress[0], &setting, sizeof(SETTING)))
    {
        LOG_ERROR("setting save error");
        return F_FAILED;
    }

    LOG_DEBUG("setting save success");

    return F_SUCCESS;
}

int data_showData(void)
{
    return F_SUCCESS;
}

int data_showSetting(void)
{
    int i = 0;

    LOG_INFO("setting.id: %s", setting.id);
    LOG_DEBUG("setting.ignoreNum = %d", setting.ignoreNum);
    LOG_DEBUG("setting.averageNum = %d", setting.averageNum);
    LOG_DEBUG("setting.sum = %d", setting.sum);
    LOG_DEBUG("setting.mode = %d", setting.mode);
    LOG_DEBUG("setting.currentIndex = %d", setting.currentIndex);
    LOG_DEBUG("setting.haveMeasuredNum = %d", setting.haveMeasuredNum);
    LOG_DEBUG("setting.R = %f", setting.R);
    LOG_DEBUG("setting.startFre = %d", setting.startFre);
    LOG_DEBUG("setting.endFre = %d", setting.endFre);

    for (i = 0; i < BLOCK_NUM; ++i)
    {
        LOG_DEBUG("setting.blockAddress[%d] = %p", i, setting.blockAddress[i]);
        LOG_DEBUG("setting.blockSize[%d] = %d", i, setting.blockSize[i]);
    }

    return F_SUCCESS;
}

void showResult(FPGA_RESULT result)
{
    LOG_DEBUG("inputRe: %f", result.input.real);
    LOG_DEBUG("inputIm: %f", result.input.imagin);
    LOG_DEBUG("outputRe: %f", result.output.real);
    LOG_DEBUG("outputIm: %f", result.output.imagin);
    LOG_DEBUG("RRe: %f", result.R.real);
    LOG_DEBUG("RIm: %f", result.R.imagin);
    LOG_DEBUG("fre: %.1f", result.fre / 10.0);
}

u8 data_getRecieved(void)
{
    return data.recievedFlag;
}

u8 data_getIgnoreNum(void)
{
    return setting.ignoreNum;
}

u8 data_getAverageNum(void)
{
    return setting.averageNum;
}

int data_resetRecieved(void)
{
    data.recievedFlag = 0;
    return F_SUCCESS;
}

int data_plusRecieved(void)
{
    data.recievedFlag++;
    return F_SUCCESS;
}

u8 data_isIgnore(void)
{
    return data.isIgnore;
}

int data_setIsIgnore(u8 state)
{
    data.isIgnore = state;
    return F_SUCCESS;
}

u8 data_isScram(void)
{
    return data.isScram;
}

int data_setIsScram(u8 state)
{
    data.isScram = state;
    return F_SUCCESS;
}

u8 data_isStart(void)
{
    return data.isStart;
}

int data_setIsStart(u8 state)
{
    data.isStart = state;
    return F_SUCCESS;
}

int data_setInputResult(Complex result)
{
    data.fpgaResult.input.real = result.real;
    data.fpgaResult.input.imagin = result.imagin;
    return F_SUCCESS;
}

int data_setOutputResult(Complex result)
{
    data.fpgaResult.output.real = result.real;
    data.fpgaResult.output.imagin = result.imagin;
    return F_SUCCESS;
}

int data_plusInputResult(Complex result)
{
    data.fpgaResult.input.real += result.real / setting.averageNum;
    data.fpgaResult.input.imagin += result.imagin / setting.averageNum;
    return F_SUCCESS;
}

int data_plusOutputResult(Complex result)
{
    data.fpgaResult.output.real += result.real / setting.averageNum;
    data.fpgaResult.output.imagin += result.imagin / setting.averageNum;
    return F_SUCCESS;
}

int data_resetInputResult(void)
{
    data.fpgaResult.input.real = 0;
    data.fpgaResult.input.imagin = 0;
    return F_SUCCESS;
}

int data_resetOutputResult(void)
{
    data.fpgaResult.output.real = 0;
    data.fpgaResult.output.imagin = 0;
    return F_SUCCESS;
}


u32 data_ntohl(u8 *data)
{
    u32 temp = 0;
    temp = data[0];
    temp = (temp << 8) | data[1];
    temp = (temp << 8) | data[2];
    temp = (temp << 8) | data[3];

    return temp;
}

u16 data_ntohs(u8 *data)
{
    u16 temp = 0;
    temp = data[0];
    temp = (temp << 8) | data[1];

    return temp;
}

int data_htonl(u32 num, u8 *data)
{
    data[0] = (num >> 24) & 0xff;
    data[1] = (num >> 16) & 0xff;
    data[2] = (num >> 8) & 0xff;
    data[3] = num & 0xff;

    return F_SUCCESS;
}

int data_htons(u16 num, u8 *data)
{
    data[0] = (num >> 8) & 0xff;
    data[1] = num & 0xff;

    return F_SUCCESS;
}

