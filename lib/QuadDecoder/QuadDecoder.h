#ifndef QUADDECODER_H
#define QUADDECODER_H

/*  QuadEncoder.h Library was created
*   By Anmol Modur
*   9/20/19
*   v1.2
*   Encoder 1 Pins: 1(A), 2(B), 3(Z)
*   Encoder 2 Pins: 4(A), 5(B), 7(Z)


						XBAR1 -> XBARA1
GPIO	PHASE	PAD	IOMUXREG	XBARPORT IOMUX	XBAR1-INPUT	XBAR1-OUTPUT	SELn	n

7   ENC2Z   B1_01       ALT1    XBAR_INOUT15    XBAR1_IN15  XBAR1_OUT73     73  15
5	ENC2B	EMC_08		ALT3	XBAR_INOUT17	XBAR1_IN17	XBAR1_OUT72	    72	17
4   ENC2A   EMC_06      ALT3    XBAR_INOUT08    XBAR1_IN08  XBAR1_OUT71     71  08
3	ENC1Z	EMC_05		ALT3	XBAR_INOUT07	XBAR1_IN07	XBAR1_OUT68    	68	07
2	ENC1B	EMC_04		ALT3	XBAR_INOUT06 	XBAR1_IN06	XBAR1_OUT67 	67	06
1	ENC1A	AD_B0_02	ALT1	XBAR_INOUT16	XBAR1_IN16	XBAR1_OUT66 	66	16
0	    	AD_B0_03	ALT1	XBAR_INOUT17	XBAR1_IN17
*/

#include <Arduino.h>

//QuadDecoder class with N = 1 or 2
template <int N>
class QuadDecoder
{
private:
    int mode = 0;
    // 0 - Position Control Mode
    // 1 - Velocity Control Mode
    unsigned int XBARA_IN;
    unsigned int XBARB_OUT;
    void XBAR_CONNECT(unsigned int, unsigned int);
    void XBAR_INIT();
    void setENC_CTRL(unsigned int);
    void ENC_INIT(uint32_t);
    void IOMUXC_INIT();
    void PIT_INIT(uint32_t us = 100000);
    int PTRIGGERus = 0;
    int VCPR = 0;

public:
    QuadDecoder(int OpMode = 0);
    void begin(uint32_t CPR, uint32_t us = 100000);
    uint32_t getCount();
    int32_t getDCount();
    double getVelocity();
};

template <int N>
QuadDecoder<N>::QuadDecoder(int OpMode)
{
    mode = OpMode;
    IOMUXC_INIT();
    XBAR_INIT();
}

template <int N>
void QuadDecoder<N>::begin(uint32_t CPR, uint32_t us)
{   
    VCPR = CPR;
    PTRIGGERus = us;
    ENC_INIT(CPR);
    PIT_INIT(us); 
    
}

template <int N>
uint32_t QuadDecoder<N>::getCount()
{
    // Want unheld position when in position mode but held position in velocity mode
    int N2 = (mode == 0 ? N + 4 : N);
    switch (N2)
    {
    case 1:
        return (ENC1_UPOSH << 16) + (ENC1_LPOSH);
    case 2:
        return (ENC2_UPOSH << 16) + (ENC2_LPOSH);
    case 3:
        return (ENC3_UPOSH << 16) + (ENC3_LPOSH);
    case 4:
        return (ENC4_UPOSH << 16) + (ENC4_LPOSH);
    case 5:
        return (ENC1_UPOS << 16) + (ENC1_LPOS);
    case 6:
        return (ENC2_UPOS << 16) + (ENC2_LPOS);
    case 7:
        return (ENC3_UPOS << 16) + (ENC3_LPOS);
    case 8:
        return (ENC4_UPOS << 16) + (ENC4_LPOS);
    default:
        return 0;
    }
}

template <int N>
int32_t QuadDecoder<N>::getDCount()
{
    switch (N)
    {
    case 1:
        return (ENC1_POSDH);
    case 2:
        return (ENC2_POSDH);
    case 3: 
        return (ENC3_POSDH);
    case 4:
        return (ENC4_POSDH);
    default:
        return 0;
    }
}

template <int N>
double QuadDecoder<N>::getVelocity() // RPM
{
    if (mode == 0)
        return 0;
    
    return double(((short)getDCount())/double(VCPR)) * double(60000000.0 / PTRIGGERus);
}

template <int N>
void QuadDecoder<N>::XBAR_CONNECT(unsigned int input, unsigned int output)
{
    if (input >= 88)
        return;
    if (output >= 132)
        return;
    volatile uint16_t *xbar = &XBARA1_SEL0 + (output / 2);
    uint16_t val = *xbar;
    if (!(output & 1))
    {
        val = (val & 0xFF00) | input;
    }
    else
    {
        val = (val & 0x00FF) | (input << 8);
    }
    *xbar = val;
}

template <int N>
void QuadDecoder<N>::XBAR_INIT()
{
    CCM_CCGR2 |= CCM_CCGR2_XBAR1(CCM_CCGR_ON); // XBAR CLK ON
    switch (N)
    {
    case 1:
        XBAR_CONNECT(16, 66);
        XBAR_CONNECT(6, 67);
        // XBAR_CONNECT(7, 68);
        break;
    case 2:
        // XBAR_CONNECT(8, 71);
        XBAR_CONNECT(7, 71);
        // XBAR_CONNECT(17, 72);
        XBAR_CONNECT(17, 72);
        // XBAR_CONNECT(15, 73);
        break;
    default:
        return;
    }
}

template <int N>
void QuadDecoder<N>::setENC_CTRL(unsigned int value)
{
    uint16_t ENCN_CTRL = ENC1_CTRL + ((N - 1) * 0x4000);
    volatile uint16_t *enc1 = &ENCN_CTRL;
    uint16_t val = *enc1;
    val = val | value;
    *enc1 = val;
}

template <int N>
void QuadDecoder<N>::ENC_INIT(uint32_t CPR)
{
    uint16_t LMOD = (CPR & 0x0000FFFF);
    uint16_t UMOD = (CPR & 0xFFFF0000) >> 16;
    switch (N)
    {
    case 1:
        CCM_CCGR4 |= CCM_CCGR4_ENC1(CCM_CCGR_ON);
        ENC1_CTRL &= ~(0xFFFF);
        ENC1_CTRL |= 0x0404;
        ENC1_CTRL2 &= ~(0xFFFF);
        ENC1_CTRL2 |= (mode == 0 ? 0x0104 : 0x0105);
        ENC1_LMOD |= LMOD;
        ENC1_UMOD |= UMOD;
        break;
    case 2:
        CCM_CCGR4 |= CCM_CCGR4_ENC2(CCM_CCGR_ON);
        ENC2_CTRL &= ~(0xFFFF);
        ENC2_CTRL |= 0x0404;
        ENC2_CTRL2 &= ~(0xFFFF);
        ENC2_CTRL2 |= (mode == 0 ? 0x0104 : 0x0105);
        ENC2_LMOD |= LMOD;
        ENC2_UMOD |= UMOD;
        break;
    case 3:
        CCM_CCGR4 |= CCM_CCGR4_ENC3(CCM_CCGR_ON);
        ENC3_CTRL &= ~(0xFFFF);
        ENC3_CTRL |= 0x0404;
        ENC3_CTRL2 &= ~(0xFFFF);
        ENC3_CTRL2 |= (mode == 0 ? 0x0104 : 0x0105);
        ENC3_LMOD |= LMOD;
        ENC3_UMOD |= UMOD;
        break;
    case 4:
        CCM_CCGR4 |= CCM_CCGR4_ENC4(CCM_CCGR_ON);
        ENC4_CTRL &= ~(0xFFFF);
        ENC4_CTRL |= 0x0404;
        ENC4_CTRL2 &= ~(0xFFFF);
        ENC4_CTRL2 |= (mode == 0 ? 0x0104 : 0x0105);
        ENC4_LMOD |= LMOD;
        ENC4_UMOD |= UMOD;
        break;
    default:
        break;
    }
}

template <int N>
void QuadDecoder<N>::IOMUXC_INIT()
{
    switch (N)
    {
    case 1:
        IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_B0_02 &= ~(0x00000007);
        IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_B0_02 |= 0x1;
        IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_04 &= ~(0x00000007);
        IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_04 |= 0x3;
        break;
    case 2:
        IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_05 &= ~(0x00000007);
        IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_05 |= 0x3;
        IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_08 &= ~(0x00000007);
        IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_08 |= 0x3;
        break;
    default:
        return;
    }
}

template <int N>
void QuadDecoder<N>::PIT_INIT(uint32_t us)
{
    CCM_CCGR1 |= CCM_CCGR1_PIT(CCM_CCGR_ON);
    PIT_MCR = 0x00; // Turn On Timer
    uint32_t cycles = (24000000 / 1000000) * us - 1;
    switch (N)
    {
    case 1:
        XBAR_CONNECT(56, 70);
        PIT_TCTRL0 &= ~(0x1);
        PIT_LDVAL0 = cycles;
        PIT_TCTRL0 |= 1;
        break;
    case 2:
        XBAR_CONNECT(57, 75);
        PIT_TCTRL1 &= ~(0x1);
        PIT_LDVAL1 = cycles;
        PIT_TCTRL1 |= 1;
        break;
    case 3:
        XBAR_CONNECT(58, 80);
        PIT_TCTRL2 &= ~(0x1);
        PIT_LDVAL2 = cycles;
        PIT_TCTRL2 |= 1;
        break;
    case 4:
        XBAR_CONNECT(59, 85);
        PIT_TCTRL3 &= ~(0x1);
        PIT_LDVAL3 = cycles;
        PIT_TCTRL3 |= 1;
        break;
    default:
        break;
    }
}

#endif