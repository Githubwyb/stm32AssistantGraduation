#include "spi_dds.h"

#include "stm32f1xx_hal.h"

#include "log.h"

extern SPI_HandleTypeDef hspi3;

static u16 spiDdsRecvData[6];

int spi_dds_recvStart(void) {
    HAL_SPI_Receive_IT(&hspi3, (u8 *)spiDdsRecvData, sizeof(spiDdsRecvData) / sizeof(spiDdsRecvData[0]));
    return F_SUCCESS;
}

int parsSpiDdsData(void) {
    double ddsFre = 0;
    int frequence_LSB, frequence_MSB;
    double   frequence_mid, frequence_DATA;
    long int frequence_hex;

    // LOG_DEBUG("Spi dds recv:");
    // for (int i = 0; i < sizeof(spiDdsRecvData) / sizeof(spiDdsRecvData[0]); ++i) {
    //     PRINT("0x%04x ", spiDdsRecvData[i]);
    // }
    // PRINT("\r\n");

    if (0x0100 == spiDdsRecvData[0] && 0x2100 == spiDdsRecvData[1]) {
        frequence_LSB = spiDdsRecvData[2] & 0x3fff;
        frequence_MSB = spiDdsRecvData[3] & 0x3fff;
        frequence_hex = frequence_LSB | (frequence_MSB << 14);
        frequence_DATA = frequence_hex;

        frequence_mid = 268435456 / 25;
        frequence_DATA /= frequence_mid;
        frequence_DATA *= 1000000;
        ddsFre = frequence_DATA;
    }
    LOG_DEBUG("Dds recv fre %.1lf", ddsFre);

    switch (spiDdsRecvData[5]) {
        case 0x2002:
            LOG_DEBUG("Dds wave is TRI_WAVE");
            break;

        case 0x2028:
            LOG_DEBUG("Dds wave is SQU_WAVE");
            break;

        case 0x2000:
            LOG_DEBUG("Dds wave is SIN_WAVE");
            break;
    }

    // spi_dds_recvStart();
    return F_SUCCESS;
}

// static void AD9833_Delay(void)
// {
//     int i;
//     for (i = 0; i < 100; i++);
// }

// void AD9833_Write(unsigned int TxData)
// {
//     unsigned char i;

//     DDS_SPI_CLK_1;
//     //AD9833_Delay();
//     DDS_SPI_CS_DISABLE;
//     AD9833_Delay();
//     DDS_SPI_CS_ENABLE;
//     AD9833_Delay();
//     for(i = 0; i < 16; i++)
//     {
//         if (TxData & 0x8000)
//         {
//             DDS_SPI_DIO_1;
//         }
//         else
//         {
//             DDS_SPI_DIO_0;
//         }

//         AD9833_Delay();
//         DDS_SPI_CLK_0;
//         AD9833_Delay();
//         DDS_SPI_CLK_1;

//         TxData <<= 1;
//     }
//     DDS_SPI_CS_DISABLE;
// }

// int dds_waveSeting(double Freq, u32 Freq_SFR, u32 WaveMode, u32 Phase)
// {
//     int frequence_LSB, frequence_MSB, Phs_data;
//     double   frequence_mid, frequence_DATA;
//     long int frequence_hex;

//     frequence_mid = 268435456 / 25;
//     frequence_DATA = Freq;
//     frequence_DATA = frequence_DATA / 1000000;
//     frequence_DATA = frequence_DATA * frequence_mid;
//     frequence_hex = frequence_DATA;  //Õâ¸öfrequence_hexµÄÖµÊÇ32Î»µÄÒ»¸öºÜ´óµÄÊý×Ö£¬ÐèÒª²ð·Ö³ÉÁ½¸ö14Î»½øÐÐ´¦Àí£»
//     frequence_LSB = frequence_hex; //frequence_hexµÍ16Î»ËÍ¸øfrequence_LSB
//     frequence_LSB = frequence_LSB & 0x3fff;//È¥³ý×î¸ßÁ½Î»£¬16Î»Êý»»È¥µô¸ßÎ»ºó±ä³ÉÁË14Î»
//     frequence_MSB = frequence_hex >> 14; //frequence_hex¸ß16Î»ËÍ¸øfrequence_HSB
//     frequence_MSB = frequence_MSB & 0x3fff;//È¥³ý×î¸ßÁ½Î»£¬16Î»Êý»»È¥µô¸ßÎ»ºó±ä³ÉÁË14Î»

//     Phs_data = Phase | 0xC000;  //ÏàÎ»Öµ
//     AD9833_Write(0x0100); //¸´Î»AD9833,¼´RESETÎ»Îª1
//     AD9833_Write(0x2100); //Ñ¡ÔñÊý¾ÝÒ»´ÎÐ´Èë£¬B28Î»ºÍRESETÎ»Îª1

//     if(Freq_SFR == 0)               //°ÑÊý¾ÝÉèÖÃµ½ÉèÖÃÆµÂÊ¼Ä´æÆ÷0
//     {
//         frequence_LSB = frequence_LSB | 0x4000;
//         frequence_MSB = frequence_MSB | 0x4000;

//         AD9833_Write(frequence_LSB);
//         AD9833_Write(frequence_MSB);
//         AD9833_Write(Phs_data);
//     }
//     if(Freq_SFR == 1)
//     {
//         frequence_LSB = frequence_LSB | 0x8000;
//         frequence_MSB = frequence_MSB | 0x8000;
//         AD9833_Write(frequence_LSB);
//         AD9833_Write(frequence_MSB);
//         AD9833_Write(Phs_data);
//     }

//     if(WaveMode == TRI_WAVE)
//     {
//         AD9833_Write(0x2002);
//     }
//     if(WaveMode == SQU_WAVE)
//     {
//         AD9833_Write(0x2028);
//     }
//     if(WaveMode == SIN_WAVE)
//     {
//         AD9833_Write(0x2000);
//     }

//     return F_SUCCESS;
// }
