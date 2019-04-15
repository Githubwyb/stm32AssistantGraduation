#include "algorithm.h"

#include "stm32f1xx_hal.h"
#include <math.h>

#include "data.h"
#include "log.h"

int get_RValue(FPGA_RESULT result, Complex *R)
{
    R->real = (result.input.real * result.output.real + result.input.imagin * result.output.imagin) / (result.output.real * result.output.real + result.output.imagin * result.output.imagin) - 1;
    if (R->real < 0.0001f)
    {
        R->real = 0.0001f;
    }
    R->imagin = (result.input.imagin * result.output.real - result.input.real * result.output.imagin) / (result.output.real * result.output.real + result.output.imagin * result.output.imagin);
    return F_SUCCESS;
}

int get_div(double fre)
{
    DATA *pData = NULL;

    pData = data_getData();

    if (fre < 1000)
    {
        pData->curRealFre = fre * 10;
        pData->divFre = FPGA_SYSTEM_TICK / 32 / pData->curRealFre * 10;
    }
    else if (fre < 1000000 * 1.05)
    {
        pData->divFre = FPGA_SYSTEM_TICK / 32 / fre;
        pData->curRealFre = FPGA_SYSTEM_TICK / 32 / pData->divFre * 10;
    }
    else
    {
        LOG_ERROR("frequency max is 1000000");
        return F_FAILED;
    }

    return F_SUCCESS;
}

int get_fre(u8 mode)
{
    DATA *pData = NULL;
    SETTING *pSetting = NULL;

    pData = data_getData();
    pSetting = data_getSetting();

    if (mode == 1)
    {
        return get_div(pData->curFre);
    }
    else
    {
        switch (pSetting->mode)
        {
        case FPGA_MODE_LINE:
            return get_freLine();

        case FPGA_MODE_LOG:
            return get_freLog();

        default:
            return F_FAILED;
        }
    }

    //    return F_FAILED;
}

int get_divLine(double fre)
{
    DATA *pData = NULL;

    pData = data_getData();

    if (fre < 1000)
    {
        pData->curRealFre = fre * 10;
        pData->divFre = FPGA_SYSTEM_TICK / 32 / pData->curRealFre * 10;
    }
    else if (pData->curFre > 10000)
    {
        if (pData->fpgaFreChoose != 2) {
            pData->curRealFre = FPGA_SYSTEM_TICK / 32 / pData->divFre * 10;
        } else {
            pData->curRealFre = FPGA_SYSTEM_TICK_1 / 32 / pData->divFre * 10;
        }
    }
    else
    {
        pData->divFre = FPGA_SYSTEM_TICK / 32 / fre;
        pData->curRealFre = FPGA_SYSTEM_TICK / 32 / pData->divFre * 10;
    }

    return F_SUCCESS;
}

int get_freLine(void)
{
    static float add = 0;
    static int threshold1 = 0;
    static int threshold2 = 0;
    static int n1 = 0;
    static int n2 = 0;
    static DATA *pData = NULL;
    static SETTING *pSetting = NULL;
    int temp = 0;
    int temp1 = 0;
    int temp2 = 0;
    pData = data_getData();
    if (L_TRUE == pData->isFirst)
    {
        pSetting = data_getSetting();

        pData->lastDiv = 0xffffffff;

        temp = pSetting->sum / 5;
        add = 90.0 / temp;

        temp1 = (int)(FPGA_SYSTEM_TICK / 32 / 10000) - (int)(FPGA_SYSTEM_TICK / 32 / 100000) - 1;
        n1 = temp1 / (temp - 1);
        temp2 = temp1 % (temp - 1);
        threshold1 = (int)(FPGA_SYSTEM_TICK / 32 / 10000) - (n1 + 1) * temp2 + 1;

        temp1 = (int)(FPGA_SYSTEM_TICK / 32 / 100000) - (int)(FPGA_SYSTEM_TICK / 32 / 1000000);
        // LOG_DEBUG("temp1: %d", temp1);
        if (temp1 < temp)
        {
            n2 = 0;
            threshold2 = 0;
        }
        else
        {
            n2 = temp1 / (temp - 1);
            temp2 = temp1 % (temp - 1);
            threshold2 = (int)(FPGA_SYSTEM_TICK / 32 / 100000) - (n2 + 1) * temp2 + 1;
        }

        LOG_DEBUG("n1: %d threshold1: %d n2: %d threshold2: %d", n1, threshold1, n2, threshold2);
    }

    if (pData->curFre < pSetting->startFre)
    {
        LOG_DEBUG("%f %d", pData->curFre, pSetting->startFre);
        pData->isFirst = L_TRUE;
        return F_FAILED;
    }

    if (pData->curFre - pSetting->startFre < 0.1 && L_TRUE == pData->isFirst)
    {
        pData->isFirst = L_FALSE;
    }
    else if (pData->curFre < 50)
    {
        pData->curFre += add;
        if (50 - pData->curFre < 0.2)
        {
            pData->curFre = 50;
        }
    }
    else if (pData->curFre < 100)
    {
        pData->curFre += add;
        if (100 - pData->curFre < 0.2)
        {
            pData->curFre = 100;
        }
    }
    else if (pData->curFre < 1000)
    {
        pData->curFre += add * 10;
        if (1000 - pData->curFre < 2)
        {
            pData->curFre = 1000;
        }
    }
    else if (pData->curFre < 10000)
    {
        pData->curFre += add * 100;
        if (10000 - pData->curFre < 20)
        {
            pData->curFre = 10000;
            pData->divFre = FPGA_SYSTEM_TICK / 32 / pData->curFre;
        }
    }
    else if (pData->curFre < 100000 && pData->divFre > threshold1)
    {
        pData->divFre -= n1 + 1;
        pData->curFre = FPGA_SYSTEM_TICK / 32 / pData->divFre;
    }
    else if (pData->curFre < 100000 && pData->divFre <= threshold1)
    {
        pData->divFre -= n1;
        pData->curFre = FPGA_SYSTEM_TICK / 32 / pData->divFre;
        if (pData->curFre > 100000)
        {
            pData->curFre = 100000;
            pData->divFre = FPGA_SYSTEM_TICK / 32 / pData->curFre;
        }
    }
    else {
        switch (pSetting->sum) {
            case 251:
                if (pData->divFre > threshold2) {
                    pData->divFre -= n2 + 1;
                    pData->curFre = FPGA_SYSTEM_TICK / 32 / pData->divFre;
                }
                else if (pData->divFre < threshold2)
                {
                    pData->divFre -= n2;
                    pData->curFre = FPGA_SYSTEM_TICK / 32 / pData->divFre;
                }
                break;

            case 501:
                if (pData->fpgaFreChoose == 0) {
                    if (pData->divFre > 53) {
                        pData->divFre -= 2;
                        pData->curFre = FPGA_SYSTEM_TICK / 32 / pData->divFre;
                    }
                    else
                    {
                        pData->fpgaFreChoose = 1;
                        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);
                        pData->divFre -= 1;
                        pData->curFre = FPGA_SYSTEM_TICK / 32 / pData->divFre;
                    }
                } else if (pData->fpgaFreChoose == 1) {
                    pData->fpgaFreChoose = 2;
                    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);
                    pData->curFre = FPGA_SYSTEM_TICK_1 / 32 / pData->divFre;
                } else {
                    pData->fpgaFreChoose = 1;
                    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);
                    pData->divFre -= 1;
                    pData->curFre = FPGA_SYSTEM_TICK / 32 / pData->divFre;
                }
                break;

            case 1001:
                if (pData->fpgaFreChoose == 0) {
                    pData->fpgaFreChoose = 1;
                    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);
                    pData->divFre -= 1;
                    pData->curFre = FPGA_SYSTEM_TICK / 32 / pData->divFre;
                } else if (pData->fpgaFreChoose == 1) {
                    pData->fpgaFreChoose = 2;
                    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);
                    pData->curFre = FPGA_SYSTEM_TICK_1 / 32 / pData->divFre;
                } else {
                    pData->fpgaFreChoose = 1;
                    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);
                    pData->divFre -= 1;
                    pData->curFre = FPGA_SYSTEM_TICK / 32 / pData->divFre;
                }
                break;

            default:
                LOG_ERROR("Sum %d is error", pSetting->sum);
                break;
        }
    }

    get_divLine(pData->curFre);

    if (pData->curRealFre > pSetting->endFre * 10.6)
    {
        pData->isFirst = L_TRUE;
        return F_FAILED;
    }

    return F_SUCCESS;
}

int get_divLog(double fre)
{
    DATA *pData = data_getData();
    SETTING *pSetting = data_getSetting();

    if (fre < 1000)
    {
        pData->curRealFre = fre * 10;
        pData->divFre = FPGA_SYSTEM_TICK / 32 / pData->curRealFre * 10;
    }
    else
    {
        pData->divFre = FPGA_SYSTEM_TICK / 32 / fre;
        pData->curRealFre = FPGA_SYSTEM_TICK / 32 / pData->divFre * 10;
    }

    switch (pSetting->sum) {
        case 251:
            if (pData->divFre >= pData->lastDiv)
            {
                pData->divFre = pData->lastDiv - 1;
                pData->curRealFre = FPGA_SYSTEM_TICK / 32 / pData->divFre * 10;
            }
            break;

        case 501:
            if (pData->fpgaFreChoose == 0 && pData->divFre >= pData->lastDiv) {
                pData->fpgaFreChoose = 1;
                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);
                pData->divFre = pData->lastDiv - 1;
                pData->curRealFre = FPGA_SYSTEM_TICK / 32 / pData->divFre * 10;
            } else if (pData->fpgaFreChoose == 1) {
                pData->fpgaFreChoose = 2;
                pData->divFre = pData->lastDiv;
                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);
                pData->curRealFre = FPGA_SYSTEM_TICK_1 / 32 / pData->divFre * 10;
            } else if (pData->fpgaFreChoose == 2) {
                pData->fpgaFreChoose = 1;
                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);
                pData->divFre = pData->lastDiv - 1;
                pData->curRealFre = FPGA_SYSTEM_TICK / 32 / pData->divFre * 10;
            }
            break;

        case 1001:
            if (pData->fpgaFreChoose == 0 && pData->divFre >= pData->lastDiv) {
                pData->fpgaFreChoose = 1;
                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);
                pData->divFre = pData->lastDiv - 1;
                pData->curRealFre = FPGA_SYSTEM_TICK / 32 / pData->divFre * 10;
            } else if (pData->fpgaFreChoose == 1) {
                pData->fpgaFreChoose = 2;
                pData->divFre = pData->lastDiv;
                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);
                pData->curRealFre = FPGA_SYSTEM_TICK_1 / 32 / pData->divFre * 10;
            } else if (pData->fpgaFreChoose == 2) {
                pData->fpgaFreChoose = 1;
                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);
                pData->divFre = pData->lastDiv - 1;
                pData->curRealFre = FPGA_SYSTEM_TICK / 32 / pData->divFre * 10;
            }
            break;

        default:
            LOG_ERROR("Sum %d is error", pSetting->sum);
            break;
    }

    pData->lastDiv = pData->divFre;
    return F_SUCCESS;
}

int get_freLog(void)
{
    static double times = 0;
    DATA *pData = data_getData();
    SETTING *pSetting = data_getSetting();
    double temp = 0;

    if (L_TRUE == pData->isFirst)
    {
        pData->lastDiv = 0xffffffff;

        temp = log10(pSetting->endFre) - log10(pSetting->startFre);
        temp /= pSetting->sum;
        times = pow(10, temp);

        LOG_DEBUG("times: %.4f", times);
    }

    if (pData->curFre < pSetting->startFre)
    {
        pData->isFirst = L_TRUE;
        return F_FAILED;
    }

    if (pData->curFre - pSetting->startFre < 0.01 && L_TRUE == pData->isFirst)
    {
        pData->isFirst = L_FALSE;
    }
    else if (pData->curFre < 50)
    {
        pData->curFre = pData->curFre * times;
        if (50 - pData->curFre < 0.2)
        {
            pData->curFre = 50;
        }
    }
    else
    {
        pData->curFre = pData->curFre * times;
    }

    get_divLog(pData->curFre);

    if (pData->curRealFre > pSetting->endFre * 10.6)
    {
        pData->isFirst = L_TRUE;
        return F_FAILED;
    }

    return F_SUCCESS;
}

static void Add_Complex(Complex *src1, Complex *src2, Complex *dst)
{
    dst->imagin = src1->imagin + src2->imagin;
    dst->real = src1->real + src2->real;
}

static void Sub_Complex(Complex *src1, Complex *src2, Complex *dst)
{
    dst->imagin = src1->imagin - src2->imagin;
    dst->real = src1->real - src2->real;
}

static void Multy_Complex(Complex *src1, Complex *src2, Complex *dst)
{
    float r1 = 0.0, r2 = 0.0;
    float i1 = 0.0, i2 = 0.0;
    r1 = src1->real;
    r2 = src2->real;
    i1 = src1->imagin;
    i2 = src2->imagin;
    dst->imagin = r1 * i2 + r2 * i1;
    dst->real = r1 * r2 - i1 * i2;
}

static void getWN(float n, float size_n, Complex *dst)
{
    float x = 2.0 * PI * n / size_n;
    dst->imagin = -sin(x);
    dst->real = cos(x);
}

static int FFT_remap(float *src, int size_n)
{
    int i;
    float temp[32] = {0};
    if (size_n == 1)
    {
        return 0;
    }
    for (i = 0; i < size_n; i++)
    {
        if (i % 2 == 0)
        {
            temp[i / 2] = src[i];
        }
        else
        {
            temp[(size_n + i) / 2] = src[i];
        }
    }
    for (i = 0; i < size_n; i++)
    {
        src[i] = temp[i];
    }
    FFT_remap(src, size_n / 2);
    FFT_remap(src + size_n / 2, size_n / 2);
    return 1;
}

int fft(float *src, Complex *dst, int size_n)
{
    int i, j;
    int k = size_n;
    int z = 0;
    FFT_remap(src, size_n);
    while (k /= 2)
    {
        z++;
    }
    k = z;
    if (size_n != (1 << k))
    {
        return F_FAILED;
    }
    Complex src_com[32];
    if (src_com == NULL)
    {
        return F_FAILED;
    }
    for (i = 0; i < size_n; i++)
    {
        src_com[i].real = src[i];
        src_com[i].imagin = 0;
    }
    for (i = 0; i < k; i++)
    {
        z = 0;
        for (j = 0; j < size_n; j++)
        {
            if ((j / (1 << i)) % 2 == 1)
            {
                Complex wn;
                getWN(z, size_n, &wn);
                Multy_Complex(&src_com[j], &wn, &src_com[j]);
                z += 1 << (k - i - 1);
                Complex temp;
                int neighbour = j - (1 << (i));
                temp.real = src_com[neighbour].real;
                temp.imagin = src_com[neighbour].imagin;
                Add_Complex(&temp, &src_com[j], &src_com[neighbour]);
                Sub_Complex(&temp, &src_com[j], &src_com[j]);
            }
            else
            {
                z = 0;
            }
        }
    }

    for (i = 0; i < size_n; i++)
    {
        dst[i].imagin = src_com[i].imagin;
        dst[i].real = src_com[i].real;
    }

    return F_SUCCESS;
}
