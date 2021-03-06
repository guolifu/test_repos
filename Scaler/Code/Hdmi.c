/********************************************************************************/
/*   The  Software  is  proprietary,  confidential,  and  valuable to Realtek   */
/*   Semiconductor  Corporation  ("Realtek").  All  rights, including but not   */
/*   limited  to  copyrights,  patents,  trademarks, trade secrets, mask work   */
/*   rights, and other similar rights and interests, are reserved to Realtek.   */
/*   Without  prior  written  consent  from  Realtek,  copying, reproduction,   */
/*   modification,  distribution,  or  otherwise  is strictly prohibited. The   */
/*   Software  shall  be  kept  strictly  in  confidence,  and  shall  not be   */
/*   disclosed to or otherwise accessed by any third party.                     */
/*   c<2003> - <2010>                                                           */
/*   The Software is provided "AS IS" without any warranty of any kind,         */
/*   express, implied, statutory or otherwise.                                  */
/********************************************************************************/

//----------------------------------------------------------------------------------------------------
// ID Code      : Hdmi.c No.0000
// Update Note  :
//----------------------------------------------------------------------------------------------------

#define __HDMI__

#include "Common\Header\Include.h"

#if(_SCALER_SERIES_TYPE == _RTD2472D_SERIES)

#if((_HDMI_SUPPORT == _ON) || (_TMDS_SUPPORT == _ON))
//--------------------------------------------------
// Description  : Detect DVI/HDMI input format
// Input Value  : None
// Output Value : Return _FALSE if Input Format isn't HDMI, _TRUE while Input Format is HDMI
//--------------------------------------------------
bit CHdmiFormatDetect(void)
{
    CScalerPageSelect(_PAGE2);
    CScalerRead(_P2_HDMI_SR_CB, 1, pData, _NON_AUTOINC);

    if((pData[0] & 0x01) == 0x01)//Input source is the HDMI format.
    {
        return _TRUE;
    }
    else
    {
        return _FALSE;
    }
}

#endif // End of #if((_HDMI_SUPPORT == _ON) || (_TMDS_SUPPORT == _ON))

#if(_HDMI_SUPPORT == _ON)

//--------------------------------------------------
// Description  : HDMI Video Setting
// Input Value  : None
// Output Value : Return _FALSE if Set_AVMute is true, _TRUE while Video Setting is OK.
//--------------------------------------------------
bit CHdmiVideoSetting(void)
{
    if(GET_INPUTSOURCE_TYPE() == _SOURCE_HDMI)
    {
        SET_HDMIINPUT();

        CScalerPageSelect(_PAGE2);

        CScalerSetDataPortBit(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_ACRCR_51, ~_BIT2, _BIT2);//Enable Pixel Repetition down sampling auto mode

#if(_HDCP_SUPPORT == _ON)
        CScalerSetDataPortByte(_P2_HDCP_ADDR_PORT_C3, 0x40, 0x93);//Change to HDCP1.1 for HDMI
#endif

#if((_SCALER_TYPE == _RTD247xRD) || (_SCALER_TYPE == _RTD248xRD) || (_SCALER_TYPE == _RTD2545LR))
        CScalerSetBit(_P2_HDMI_SR_CB, ~_BIT5, _BIT5);
#else
        CScalerSetBit(_P2_HDMI_SR_CB, ~(_BIT5 | _BIT3 | _BIT2), _BIT5 | _BIT3 | _BIT2);
#endif

        CTimerDelayXms(10);

        // HDMI Video
        CScalerRead(_P2_HDMI_SR_CB, 1, pData, _NON_AUTOINC);

        if(!(bit)(pData[0] & 0x40))//For Clear_AVMute
        {

#if((_SCALER_TYPE == _RTD247xRD) || (_SCALER_TYPE == _RTD248xRD) || (_SCALER_TYPE == _RTD2545LR))

            CLR_AVMUTE();
#else
            SET_AVRESUME();  // Audio WD can't action, when Set_AVMute happen.
#endif

            CAdjustDisableHDMIWatchDog(_WD_SET_AVMUTE_ENABLE);//Disable Set_AVMute Watch Dog
            CScalerSetDataPortBit(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_AVMCR_30, ~_BIT3, _BIT3);//Enable DVI/HDMI video output
            //CAdjustEnableHDMIWatchDog(_WD_SET_AVMUTE_ENABLE);//Enable Set_AVMute Watch Dog
        }
        else
        {

#if((_SCALER_TYPE == _RTD247xRD) || (_SCALER_TYPE == _RTD248xRD) || (_SCALER_TYPE == _RTD2545LR))

            SET_AVMUTE();
#else
            CLR_AVRESUME();
#endif

            CLR_HDMIINPUT();
            return _FALSE;
        }

        //HDMI Video Part
        CScalerSetBit(_P2_HDMI_GPVS_CC, ~_BIT0, _BIT0);
        CScalerGetDataPortByte(_P2_HDMI_PSAP_CD, 0x00, 1, pData, _NON_AUTOINC);

            if((pData[0] & 0x23) == 0)//Check BCH data(Package error flag)
            {
                CScalerGetDataPortByte(_P2_HDMI_PSAP_CD, 0x04, 3, pData, _NON_AUTOINC);

                if((bit)(pData[0] & 0x40) != (bit)(pData[0] & 0x20))//For HDMI switch between RGB/YUV
                {

#if(_FRC_SUPPORT == _ON)
                    CLR_FRCRGBIN();//YUV Input
#endif
                    if((pData[0]&0x20) == 0x20) //YUV 422
                    {

#if((_SCALER_TYPE == _RTD2545LR) || (_SCALER_TYPE == _RTD247xRD) || (_SCALER_TYPE == _RTD248xRD))

#if(_FRC_YUV_MODE_SUPPORT == _ON)
                        CScalerSetByte(_V8_YUV_CONVERSION_1F, 0x10);//set audio pin share
                        SET_FRC16BITSIN();
                        CScalerPageSelect(_PAGE5);
                        CScalerSetByte(_P5_SDRF_MN_FIFO_422_SET_E3, 0x02); //Enable FRC 422->444
#else
            CScalerSetByte(_V8_YUV_CONVERSION_1F, 0x18);//enable and interpolation and set audio pin share
#endif // End of #if(_FRC_SUPPORT == _OFF)

#elif(_SCALER_TYPE == _RTD2472D)

#if(_FRC_YUV_MODE_SUPPORT == _OFF)
                        CScalerSetByte(_V8_YUV_CONVERSION_1F, 0x08);//enable 422->444 and interpolation
#else
                        SET_FRC16BITSIN();
#endif // End of #if(_FRC_YUV_MODE_SUPPORT == _OFF)

#else
    No Setting !!
#endif // End of #if((_SCALER_TYPE == _RTD2472D) || (_SCALER_TYPE == _RTD247xRD) || (_SCALER_TYPE == _RTD248xRD))

                    }
                    else // YUV 444
                    {

#if((_SCALER_TYPE == _RTD2472D) || (_SCALER_TYPE == _RTD247xRD) || (_SCALER_TYPE == _RTD248xRD))
                        CScalerSetByte(_V8_YUV_CONVERSION_1F, 0x10); //set audio pin share
#endif

                    }

                    if((pData[1] & 0xc0) != 0xc0)//For HDMI switch between ITU601/ITU709
                    {

                        if((bit)(pData[1] & 0x40))
                        {
                            CScalerCodeW(tHDMI_YPBPR_ITU601);
                        }
                        else
                        {
                            CScalerCodeW(tHDMI_YPBPR_ITU709);
                        }

                    }
                    else if((pData[1] & 0xc0) == 0xc0)  // Extended Colorimetry, xvYCC
                    {
                        if((pData[2] & 0x70) == 0x10) // xvYCC709
                        {
                            CScalerCodeW(tHDMI_YPBPR_ITU709);
                        }
                        else if((pData[2] & 0x70) == 0x00)// xvYCC601
                        {
                            CScalerCodeW(tHDMI_YPBPR_ITU601);
                        }
                    }
                }
                else if(!(bit)(pData[0] & 0x60))  // RGB
                {

#if(_FRC_YUV_MODE_SUPPORT == _ON)
                    SET_FRCRGBIN();//YUV Input
#endif


#if((_SCALER_TYPE == _RTD2545LR) || (_SCALER_TYPE == _RTD247xRD) || (_SCALER_TYPE == _RTD248xRD))

            CScalerPageSelect(_PAGE7);
            CScalerSetByte(_P7_YUV2RGB_CTRL_BF, 0x02);
            CScalerSetByte(_P7_YUV2RGB_ACCESS_C0, 0x00);
            CScalerSetByte(_P7_YUV2RGB_CTRL_BF, 0x00);
#endif


#if(_SCALER_TYPE == _RTD2472D)
                    CScalerSetByte(_YUV2RGB_CTRL_9C, 0x00);

                    CScalerSetByte(_YUV_RGB_ACCESS_9D, 0x00);
#elif((_SCALER_TYPE == _RTD2545LR) || (_SCALER_TYPE == _RTD247xRD) || (_SCALER_TYPE == _RTD248xRD))
                    CScalerSetByte(_YUV_RGB_CTRL_9C, 0x00);
                    CScalerSetByte(_YUV_RGB_COEF_DATA_9D, 0x00);
#else
    No Setting !!
#endif // End of #if(_SCALER_TYPE == _RTD2472D)

                }  // End of else if(!(bit)(pData[0] & 0x60))
            }  // End of if((pData[0] & 0x23) == 0)
    }
    else
    {
        CLR_HDMIINPUT();
        CScalerSetDataPortBit(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_ACRCR_51, ~_BIT2, 0x00);//Disable Pixel Repetition down sampling auto mode
        CScalerSetDataPortBit(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_VCR_50, ~(_BIT3 | _BIT2 | _BIT1 | _BIT0), 0x00);
        CAdjustDisableHDMIWatchDog(_WD_HDMI_ALL);
        CScalerSetDataPortBit(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_AVMCR_30, ~_BIT3, _BIT3);//Enable DVI/HDMI video output
        CScalerSetDataPortByte(_P2_HDCP_ADDR_PORT_C3, 0x40, 0x91);// Change to HDCP1.0 for DVI

#if(_SCALER_TYPE == _RTD2472D)
        CScalerSetByte(_YUV2RGB_CTRL_9C, 0x00);
        CScalerSetByte(_YUV_RGB_ACCESS_9D, 0x00);
#elif((_SCALER_TYPE == _RTD2545LR) || (_SCALER_TYPE == _RTD247xRD) || (_SCALER_TYPE == _RTD248xRD))
        CScalerSetByte(_YUV_RGB_CTRL_9C, 0x00);
        CScalerSetByte(_YUV_RGB_COEF_DATA_9D, 0x00);
#else
        No Setting !!
#endif
    }  // End of if(CHdmiFormatDetect())

    return _TRUE;
}

//--------------------------------------------------
// Description  : Detect AVMute to set WatchDog
// Input Value  : None
// Output Value : None
//--------------------------------------------------
void CHdmiAVMuteCheck(void)
{
    if((GET_INPUTPORT_TYPE(g_ucSearchIndex) == _D0_HDMI_PORT) || (GET_INPUTPORT_TYPE(g_ucSearchIndex) == _D1_HDMI_PORT)
    || (GET_INPUTPORT_TYPE(g_ucSearchIndex) == _D0_DVI_PORT) || (GET_INPUTPORT_TYPE(g_ucSearchIndex) == _D1_DVI_PORT)
    || (GET_INPUTPORT_TYPE(g_ucSearchIndex) == _D0_DVI_I_PORT) || (GET_INPUTPORT_TYPE(g_ucSearchIndex) == _D1_DVI_I_PORT))//sephinroth 20080606
    {
        if(GET_INPUTSOURCE_TYPE() == _SOURCE_HDMI)
        {
            CScalerPageSelect(_PAGE2);
            CScalerRead(_P2_HDMI_SR_CB, 1, pData, _NON_AUTOINC);  // Detect AVMute

            if((bit)(pData[0] & 0x40)) // AVMute detected!
            {
                CAdjustDisableHDMIWatchDog(_WD_PACKET_VARIATION | _WD_SET_AVMUTE_ENABLE);  // Disable packet variation Watch Dog
                CScalerSetBit(_VDISP_CTRL_28, ~_BIT5, _BIT5);  // Force to background
            }
            else   // No AVMute
            {
                 CAdjustEnableHDMIWatchDog(_WD_PACKET_VARIATION);   // Enable packet variation Watch Dog
                 CScalerSetBit(_VDISP_CTRL_28, ~_BIT5, 0x00);
            }
         }
    }
}


#if(_AUDIO_SUPPORT == _ON)

#if((_SCALER_TYPE == _RTD2472D))
//--------------------------------------------------
// Description  : Detect Audio Lock status
// Input Value  : None
// Output Value : Return _FALSE if Audio Lock is ok, _TRUE while Audio mislock, FIFO underflow/overflow
//--------------------------------------------------
bit CHdmiAudioFIFODetect(void)
{
    CScalerPageSelect(_PAGE2);
    CScalerRead(_P2_HDMI_SR_CB, 1, pData, _NON_AUTOINC);

    if((pData[0] & 0x06) == 0)
    {
        return _FALSE;
    }

    return _TRUE;
}
#endif

#if((_SCALER_TYPE == _RTD247xRD) || (_SCALER_TYPE == _RTD248xRD) || (_SCALER_TYPE == _RTD2545LR))
//--------------------------------------------------
// Description  : HDMI Audio Initial Setting
// Input Value  : None
// Output Value : None
//--------------------------------------------------
void CHdmiAudioInitialSetting(void)
{
    g_usHDMILockedFreq = 0;
    SET_HDMIAUDIOUNTRACKED();
}

//--------------------------------------------------
// Description  : Detect Audio Enable Check
// Input Value  : None
// Output Value : Return _TRUE if Audio detected, _FALSE if get AVMute or no Audio
//--------------------------------------------------
bit CHdmiAudioEnabledCheck(void)
{
    CScalerPageSelect(_PAGE2);
    CScalerSetDataPortBit(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_ACRCR_51, ~_BIT1, _BIT1); // pop up CTS&N result
    CScalerGetDataPortByte(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_ACRSR0_52, 5, pData, _NON_AUTOINC);

    if(GET_AVMUTE() || ((pData[0] | pData[1] | pData[2]) == 0x00))   // get AVMute or no Audio
    {
        return _FALSE;
    }
    else // detect Audio coming ( by CTS)
    {
        return _TRUE;
    }
}


//--------------------------------------------------
// Description  : Detect whether Audio is ready for output or not (LPCM, PLL, and overflow/underflow check)
// Input Value  : None
// Output Value : Return _TRUE if Audio is ready for output, _FALSE while Audio mislock, FIFO underflow/overflow
//--------------------------------------------------
bit CHdmiAudioReadyDetect(void)
{
    CScalerPageSelect(_PAGE2);
    CScalerRead(_P2_HDMI_SR_CB, 1, pData, _NON_AUTOINC);

    // LPCM, PLL, and overflow/underflow check ==> AudioReady
    if((pData[0] & 0x1E) == 0)
    {
        CScalerGetDataPortByte(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_DPCR3_3B + 1, 1, pData, _NON_AUTOINC);  //check sum c value
        if((pData[0] == 0x7f) || (pData[0] == 0x80))
        {
            CScalerSetDataPortByte(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_AOCR_62, 0x00);//Disable SPDIF/I2S Output
            g_usHDMILockedFreq = 0;
            return _FALSE;
        }
        else
        {
            CScalerSetBit(_P2_HDMI_SR_CB, ~(_BIT3 | _BIT2 | _BIT1), (_BIT3 | _BIT2 | _BIT1));   //Write 1 clear

            return _TRUE;
        }
    }
    else  //Check Fail ==> Audio not Ready
    {
        CScalerSetDataPortByte(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_AOCR_62, 0x00);  // Disable SPDIF/I2S Output
        g_usHDMILockedFreq = 0;

        return _FALSE;
    }
}
#endif

#if((_SCALER_TYPE == _RTD247xRD) || (_SCALER_TYPE == _RTD248xRD) || (_SCALER_TYPE == _RTD2545LR))

//--------------------------------------------------
// Description  : Setting Audio Frequence Mode
// Input Value  : None
// Output Value : None
//--------------------------------------------------
bit CHdmiAudioFirstTracking(void)
{
    BYTE ucCoeff = 0, ucS = 0;
    WORD usA = 1024, usB = 0, usM = 0;
    DWORD ulCTS = 0, ulN = 0, ulFreq = 0;


#if(_SCALER_TYPE == _RTD2472D)
    BYTE   ucO = 1;
#elif((_SCALER_TYPE == _RTD2545LR) || (_SCALER_TYPE == _RTD247xRD) || (_SCALER_TYPE == _RTD248xRD))
    BYTE   ucO = 2;
#else
    No Setting !!
#endif

    //Disable Audio Watch Dog
    CAdjustDisableHDMIWatchDog(_WD_AUDIO_FIFO);

    //HDMI Address Auto Increase Enable
    CScalerSetBit(_P2_HDMI_APC_C8, ~_BIT0, _BIT0);

    // Pop Up N&CTS Result
    CScalerSetDataPortBit(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_ACRCR_51, ~_BIT1, _BIT1);
    CScalerSetDataPortBit(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_NTX1024TR0_28, ~_BIT3, _BIT3);
    CTimerDelayXms(2);

    CScalerGetDataPortByte(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_ACRSR0_52, 5, pData, _NON_AUTOINC);


    ulCTS = ((DWORD)pData[0] << 12) | ((DWORD)pData[1] << 4) | (((DWORD)pData[2] >> 4) & 0x0f);
    ulN = (((DWORD)pData[2] & 0x0f) << 16) | ((DWORD)pData[3] << 8) | (DWORD)pData[4];

    CScalerGetDataPortByte(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_NTX1024TR0_28, 2, pData, _NON_AUTOINC);
    usB = (((WORD)pData[0] & 0x07) << 8) | (WORD)pData[1];

    // Fv(Video Freq) = Xtal * a / b ( a = 1024)
    // Fa(Audio Freq) = 128 * Fs(Sampling Freq) = (n/cts) * Fv
    // Getting :   Fs       = (n/cts) * ( Xtal * a / b ) / 128  (in 0.1kHz unit )
    //             Fs(freq) = (DWORD)8 * 2 * 10000 * _RTD_XTAL / cts * n / ((DWORD)b * 1000);

    // fixed due to the value of freq overflow !!
    ulFreq = (DWORD)8 * 2 * 1000 * _RTD_XTAL / ulCTS  * ulN / ((DWORD)usB * 100);
    ulFreq = (ulFreq >> 1) + (ulFreq & 0x01);

    if((ulFreq >= 318) && (ulFreq <= 322))
    {
        ucCoeff = _AUDIO_MCK_32000;
        ulFreq = 32000;
        ucAudioRate = 0;
        pData[6] = _AUDIO_FREQ_32000;
    }
    else if((ulFreq >= 438) && (ulFreq <= 444))
    {
        ucCoeff = _AUDIO_MCK_44100;
        ulFreq = 44100;
        ucAudioRate = 0;
        pData[6] = _AUDIO_FREQ_44100;
    }
    else if((ulFreq >= 476) && (ulFreq <= 484))
    {
        ucCoeff = _AUDIO_MCK_48000;
        ulFreq = 48000;
        ucAudioRate = 0;
        pData[6] = _AUDIO_FREQ_48000;
    }
    else if((ulFreq >= 877) && (ulFreq <= 887))
    {
        ucCoeff = _AUDIO_MCK_88200;
        ulFreq = 88200;
        ucAudioRate = 1;
        pData[6] = _AUDIO_FREQ_88200;
    }
    else if((ulFreq >= 955) && (ulFreq <= 965))
    {
        ucCoeff = _AUDIO_MCK_96000;
        ulFreq = 96000;
        ucAudioRate = 1;
        pData[6] = _AUDIO_FREQ_96000;
    }
    else if((ulFreq >= 1754) && (ulFreq <= 1774))
    {
        ucCoeff = _AUDIO_MCK_176400;
        ulFreq = 176400;
        ucAudioRate = 2;
        pData[6] = _AUDIO_FREQ_176400;
    }
    else if((ulFreq >= 1910) && (ulFreq <= 1930))
    {
        ucCoeff = _AUDIO_MCK_192000;
        ulFreq = 192000;
        ucAudioRate = 2;
        pData[6] = _AUDIO_FREQ_192000;
    }
    else
    {
        pData[6] = _AUDIO_FREQ_NONE;
        SET_HDMIAUDIOUNTRACKED();
        return _FALSE;
    }

    CScalerPageSelect(_PAGE2);
    CScalerSetDataPortBit(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_AFCR_03, ~(_BIT2 | _BIT1), 0x00); //Disable Audio FIFO

    do
    {
        ucS = ucS + 4;
        ((DWORD *)pData)[0] = (DWORD)128 * ulFreq * ucCoeff * ucS;
    }
    while (((DWORD *)pData)[0] < 180000000);

    // Fa(Audio Freq) = 128 * Fs(Sampling Freq)
    // Fa(Audio Freq) =  Xtal * (M/K/2^O/(2S))
    // Getting :   M  = 128 * Fs * 2^O * 2S * K / Xtal;
    // K=1 , O=1;
    // m = (DWORD)2 * 128 * freq * coeff * s / _RTD_XTAL / 1000;
    // m = (m >> 1) + (m & 0x01);
    usM = (((DWORD *)pData)[0] << ucO ) / ((DWORD)_RTD_XTAL * 1000);
    usM = usM+ 1;
    ucS = ucS / (ucO * 2);

    // Clear sum_r of SDM over/underflow flag
    CScalerPageSelect(_PAGE2);
    CScalerSetDataPortBit(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_AAPNR_2D, ~_BIT3, _BIT3);

    CScalerSetDataPortByte(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_MCAPR_11, (usM - 2));
    CScalerSetDataPortByte(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_SCAPR_12, (ucCoeff == _AUDIO_256_TIMES) ? ((ucS / 2) | 0x80) : (ucS / 2));

    // DPLL Deadlock
    CScalerSetDataPortByte(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_DCAPR0_13, 0x00);  // reset D Code
    CScalerSetDataPortByte(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_DCAPR1_14, 0x00);    // reset D Code

#if(_SCALER_TYPE == _RTD2472D)
    ucO = 1;
#elif((_SCALER_TYPE == _RTD2545LR) || (_SCALER_TYPE == _RTD247xRD) || (_SCALER_TYPE == _RTD248xRD))
    ucO = ucO + 1;
#else
    No Setting !!
#endif

    CScalerSetDataPortBit(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_DPCR0_38, ~(_BIT5 | _BIT4), (ucO << 4)); //DPLLO

#if(_SCALER_TYPE == _RTD2472D)
    // Calculate Ich for audio PLL
    pData[0] = (usM < 5) ? 0 : ((usM / 5) - 1);
    CScalerSetDataPortByte(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_DPCR1_39, pData[0] | 0x80);

#elif((_SCALER_TYPE == _RTD2545LR) || (_SCALER_TYPE == _RTD247xRD) || (_SCALER_TYPE == _RTD248xRD))

    // Calculate Ich for audio PLL
    pData[5] = ((DWORD) usM * 4 * 100 / 1067);    // 2bit fractional part
    pData[6] = 0x00;

    if (pData[5] >= 10)
    {
        pData[5] -= 10;
    }

    if(pData[5] >= 40)        // 2bit fractional part
    {
        pData[5] -= 40;
        pData[6] |= 0x04;
    }

    if(pData[5] >= 20)        // 2bit fractional part
    {
        pData[5] -= 20;
        pData[6] |= 0x02;
    }

    if(pData[5] >= 10)        // 2bit fractional part
    {
        pData[5] -= 10;
        pData[6] |= 0x01;
    }
    CScalerSetDataPortByte(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_DPCR1_39, pData[6] | 0x70);
#else
    No Setting !!
#endif     // End of #if(_SCALER_TYPE == _RTD2472D)

#if(_SCALER_TYPE == _RTD2472D)
    // Enable K and enable VCOSTART
    CScalerSetDataPortByte(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_DPCR3_3B, 0x03);
#endif

    // Enable Double Buffer for K/M/S/D/O
    CScalerPageSelect(_PAGE2);
    CScalerSetDataPortByte(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_CMCR_10, 0x50);

    // Calculate D code
    ((DWORD *)pData)[1] = (DWORD)1000 * _RTD_XTAL * usM / 2;  // PLL freq
    if (((DWORD *)pData)[0] > ((DWORD *)pData)[1])
    {
        usA = (((DWORD *)pData)[0] - ((DWORD *)pData)[1]) * 128 / (((DWORD *)pData)[1] / 2048);

        usA = 0xffff - usA;
    }
    else
    {
        usA = (((DWORD *)pData)[1] - ((DWORD *)pData)[0]) * 128 / (((DWORD *)pData)[1] / 2048);

        usA += 100; // MUST for compatibility
    }

    CScalerPageSelect(_PAGE2);
    CScalerSetDataPortByte(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_DCAPR0_13, usA >> 8);       // D Code
    CScalerSetDataPortByte(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_DCAPR1_14, usA & 0xff);    // D Code

    // Enable Double Buffer for D code
    CScalerSetDataPortByte(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_CMCR_10, 0x50);

    // Disable SDM
    CScalerSetDataPortBit(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_AAPNR_2D, ~_BIT1, 0x00);

    // Disable PLL
    CScalerSetDataPortBit(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_DPCR0_38, ~(_BIT7 | _BIT6), _BIT7);

    // Reg DPLL_CMPEN
    CScalerSetDataPortBit(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_DPCR0_38, ~_BIT0, 0x00);

    // Reg DPLL_CALLCH
    CScalerSetDataPortBit(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_DPCR0_38, ~_BIT1, 0x00);

    // Reg DPLL_CALSW
    CScalerSetDataPortBit(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_DPCR0_38, ~_BIT2, 0x00);

    // Enable PLL
    CScalerSetDataPortBit(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_DPCR0_38, ~(_BIT7 | _BIT6), 0x00);

// Add Calibration Setting
#if((_SCALER_TYPE == _RTD2545LR) || (_SCALER_TYPE == _RTD247xRD) || (_SCALER_TYPE == _RTD248xRD))

    //=================================================================================//
    //  Must follow the folowing sequence to Calibrate DPLL, including the delays !!   //
    //=================================================================================//

    // Set VCO default
    CScalerPageSelect(_PAGE2);
    CScalerSetDataPortBit(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_DPCR2_3A, ~(_BIT4 | _BIT3), _BIT4);
    CTimerDelayXms(1);

    // Reg DPLL_CMPEN
    CScalerSetDataPortBit(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_DPCR0_38, ~_BIT0, _BIT0);
    CTimerDelayXms(1);

    // Reg DPLL_CALLCH
    CScalerSetDataPortBit(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_DPCR0_38, ~_BIT1, _BIT1);
    CTimerDelayXms(1);

    // Reg DPLL_CALSW
    CScalerSetDataPortBit(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_DPCR0_38, ~_BIT2, _BIT2);

#endif

#if(_AUDIO_LOCK_MODE == _HARDWARE_TRACKING)

    CScalerSetDataPortByte(_P2_HDMI_ADDR_PORT_C9, 0x20, 0x00);  // Set I code of trend
    CScalerSetDataPortByte(_P2_HDMI_ADDR_PORT_C9, 0x21, 0x02);  // Set I code of trend
    CScalerSetDataPortByte(_P2_HDMI_ADDR_PORT_C9, 0x22, 0xff);  // Set P code of trend
    CScalerSetDataPortByte(_P2_HDMI_ADDR_PORT_C9, 0x23, 0xff);  // Set P code of trend
    CScalerSetDataPortByte(_P2_HDMI_ADDR_PORT_C9, 0x24, 0x00);  // Set I code of boundary
    CScalerSetDataPortByte(_P2_HDMI_ADDR_PORT_C9, 0x25, 0x30);  // Set I code of boundary
    CScalerSetDataPortByte(_P2_HDMI_ADDR_PORT_C9, 0x26, 0xff);  // Set P code of boundary
    CScalerSetDataPortByte(_P2_HDMI_ADDR_PORT_C9, 0x27, 0xff);  // Set P code of boundary
    CScalerSetDataPortByte(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_STBPR_2A, 0x80);     // Set Boundary Tracking Update Response Time
    CScalerSetDataPortByte(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_AAPNR_2D, 0xC2);     // Clear max value and enable SDM
    CScalerSetDataPortBit(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_CMCR_10, ~_BIT4, _BIT4);  // update double buffer

#else   // N/CTS tracking

    CScalerSetDataPortByte(_P2_HDMI_ADDR_PORT_C9, 0x1d, 0x02);  // Set I code//05
    CScalerSetDataPortByte(_P2_HDMI_ADDR_PORT_C9, 0x1e, 0xff);  // Set P code//00
    CScalerSetDataPortByte(_P2_HDMI_ADDR_PORT_C9, 0x1f, 0xFF);  // Set P code//9F

    CScalerSetDataPortByte(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_AAPNR_2D, 0x02);  // Enable SDM
    CScalerSetDataPortBit(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_CMCR_10, ~_BIT4, _BIT4);  // Update Double Buffer

#endif

    CScalerSetDataPortBit(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_WDCR0_31, ~_BIT5, _BIT5); // Enable Audio FIFO Tracking main control

    // Set FIFO Manual tracking Start
    CScalerSetDataPortByte(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_WDCR2_33, 0x05);     // set Y value
    CScalerSetDataPortBit(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_WDCR1_32, ~(_BIT3 | _BIT2 | _BIT1 | _BIT0), 0x05); // set X value
    CScalerSetDataPortByte(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_FBR_1B, 0x76);  // Target FIFO Depth
    CScalerSetDataPortBit(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_AFCR_03, ~_BIT2, _BIT2);  // Enable Audio FIFO Write

    CScalerGetDataPortByte(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_AFSR_04, 1, pData, _NON_AUTOINC);

    // prevent dead lock
    pData[1] = 0;
    while((pData[0] & _BIT1) != _BIT1)
    {
        pData[1] += 1;
        if(pData[1] > 10)
        {
            SET_HDMIAUDIOUNTRACKED();
            return _FALSE;
        }
        CScalerGetDataPortByte(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_AFSR_04, 1, pData, _NON_AUTOINC);
    }

#if(_AUDIO_LOCK_MODE == _HARDWARE_TRACKING)
        CScalerSetDataPortByte(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_PSCR_15, 0x4E);  // Enable FIFO Trend/ Boundary Tracking
#else
       CScalerSetDataPortByte(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_PSCR_15, 0x12);   // Enable N & CTS Tracking, phase error counted by fps/4,fdds
#endif

    CScalerSetDataPortByte(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_FTR_1A, 0x03);
    CScalerSetDataPortBit(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_CMCR_10, ~_BIT4, _BIT4);  // Update Double Buffer

    CScalerSetDataPortBit(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_AFCR_03, ~_BIT1, _BIT1);  // Enable Audio FIFO Read

    // Wait for PLL Stable
    pData[2] = 0;
    usB = 0;
    do
    {
        usA = usB;

        CScalerGetDataPortByte(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_DPCR3_3B + 1, 2, pData, _NON_AUTOINC);  // check sum c value
        usB = (pData[0] << 8 | pData[1]);
        pData[2] += 1;

        if((abs(usA-usB) < 0x20) || (pData[0] == 0x7f) || (pData[0] == 0x80))
        {
            break;
        }
    }
    while(pData[2] <= 100);

    if((pData[0] == 0x7f) || (pData[0] == 0x80) || (pData[2] > 100))
    {
        SET_HDMIAUDIOUNTRACKED();

        return _FALSE;
    }
    else
    {

        CScalerPageSelect(_PAGE2);
        CScalerSetBit(_P2_HDMI_SR_CB, ~(_BIT3 | _BIT2 | _BIT1), (_BIT3 | _BIT2 | _BIT1));   // Write 1 clear

        return _TRUE;
    }

}
#else

//--------------------------------------------------
// Description  : Setting Audio Frequence Mode
// Input Value  : None
// Output Value : None
//--------------------------------------------------
void CHdmiAudioFirstTracking(void)
{
    BYTE coeff = 0, s = 0;
    WORD a = 1024, b = 0, m = 0;
    DWORD cts = 0, n = 0, freq = 0;

#if(_SCALER_TYPE == _RTD2472D)
    BYTE   o = 1;
#elif((_SCALER_TYPE == _RTD2545LR) || (_SCALER_TYPE == _RTD247xRD) || (_SCALER_TYPE == _RTD248xRD))
    BYTE   o = 2;
#else
    No Setting !!
#endif // End of #if(_SCALER_TYPE == _RTD2472D)


    CScalerPageSelect(_PAGE2);
    CScalerRead(_P2_HDMI_SR_CB, 1, pData, _NON_AUTOINC);

    if((CHdmiAudioFIFODetect() || GET_AVRESUME()) && (!(bit)(pData[0] & 0x40)))//For HDMI audio pll setting
    {
        CLR_AVRESUME();

        CAdjustDisableHDMIWatchDog(_WD_AUDIO_FIFO);  // Disable Audio Watch Dog
        CScalerSetDataPortByte(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_PSCR_15, 0x00);  // Disable FIFO Trend
        CScalerSetDataPortByte(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_CMCR_10, 0x50);  // Update Double Buffer
        CScalerSetBit(_P2_HDMI_APC_C8, ~_BIT0, _BIT0);  // HDMI Address Auto Increase Enable

        CScalerSetDataPortBit(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_ACRCR_51, ~_BIT1, _BIT1);
        CScalerSetDataPortBit(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_NTX1024TR0_28, ~_BIT3, _BIT3);
        CTimerDelayXms(2);

        CScalerGetDataPortByte(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_ACRSR0_52, 5, pData, _NON_AUTOINC);


        cts = ((DWORD)pData[0] << 12) | ((DWORD)pData[1] << 4) | (((DWORD)pData[2] >> 4) & 0x0f);
        n = (((DWORD)pData[2] & 0x0f) << 16) | ((DWORD)pData[3] << 8) | (DWORD)pData[4];

        CScalerGetDataPortByte(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_NTX1024TR0_28, 2, pData, _NON_AUTOINC);
        b = (((WORD)pData[0] & 0x07) << 8) | (WORD)pData[1];

        // Fv(Video Freq) = Xtal * a / b ( a = 1024)
        // Fa(Audio Freq) = 128 * Fs(Sampling Freq) = (n/cts) * Fv
        // Getting :   Fs       = (n/cts) * ( Xtal * a / b ) / 128  (in 0.1kHz unit )
        //             Fs(freq) = (DWORD)8 * 2 * 10000 * _RTD_XTAL / cts * n / ((DWORD)b * 1000);

        freq = (DWORD)8 * 2 * 1000 * _RTD_XTAL / cts  * n / ((DWORD)b * 100);
        freq = (freq >> 1) + (freq & 0x01);

        if((freq >= 318) && (freq <= 322))
        {
            coeff = _AUDIO_MCK_32000;
            freq = 32000;
            ucAudioRate = 0;
        }
        else if((freq >= 438) && (freq <= 444))
        {
            coeff = _AUDIO_MCK_44100;
            freq = 44100;
            ucAudioRate = 0;
        }
        else if((freq >= 476) && (freq <= 484))
        {
            coeff = _AUDIO_MCK_48000;
            freq = 48000;
            ucAudioRate = 0;
        }
        else if((freq >= 877) && (freq <= 887))
        {
            coeff = _AUDIO_MCK_88200;
            freq = 88200;
            ucAudioRate = 1;
        }
        else if((freq >= 955) && (freq <= 965))
        {
            coeff = _AUDIO_MCK_96000;
            freq = 96000;
            ucAudioRate = 1;
        }
        else if((freq >= 1754) && (freq <= 1774))
        {
            coeff = _AUDIO_MCK_176400;
            freq = 176400;
            ucAudioRate = 2;
        }
        else if((freq >= 1910) && (freq <= 1930))
        {
            coeff = _AUDIO_MCK_192000;
            freq = 192000;
            ucAudioRate = 2;
        }
        else
        {
            SET_AVRESUME();
        }

        if(!GET_AVRESUME())
        {

            do
            {
                s = s + 4;
                ((DWORD *)pData)[0] = (DWORD)128 * freq * coeff * s;
            }
            while (((DWORD *)pData)[0] < 180000000);


            // Fa(Audio Freq) = 128 * Fs(Sampling Freq)
            // Fa(Audio Freq) =  Xtal * (M/K/2^O/(2S))
            // Getting :   M  = 128 * Fs * 2^O * 2S * K / Xtal;
            // K=1 , O=1;
            m = (((DWORD *)pData)[0] << o ) / ((DWORD)_RTD_XTAL * 1000);
            //m = (DWORD)2 * 128 * freq * coeff * s / _RTD_XTAL / 1000;
            //m = (m >> 1) + (m & 0x01);
            m = m + 1;
            s = s / (o * 2);

            CScalerSetDataPortByte(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_AAPNR_2D, 0x08);  // Disable SDM

            CScalerSetDataPortByte(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_MCAPR_11, (m - 2));
            CScalerSetDataPortByte(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_SCAPR_12, (coeff == _AUDIO_256_TIMES) ? ((s / 2) | 0x80) : (s / 2));

//#if(_AUDIO_LOCK_MODE == _HARDWARE_TRACKING)

            // Calculate D code
            ((DWORD *)pData)[1] = (DWORD)1000 * _RTD_XTAL * m / 2;  // PLL freq
            if (((DWORD *)pData)[0] > ((DWORD *)pData)[1])
            {
                a = (((DWORD *)pData)[0] - ((DWORD *)pData)[1]) * 128 / (((DWORD *)pData)[1] / 2048);

                a = 0xffff - a;
            }
            else
            {
                a = (((DWORD *)pData)[1] - ((DWORD *)pData)[0]) * 128 / (((DWORD *)pData)[1] / 2048);

                a += 100; // MUST for compatibility
            }

            CScalerSetDataPortByte(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_DCAPR0_13, a >> 8);
            CScalerSetDataPortByte(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_DCAPR1_14, a & 0xff);
//#endif

#if(_SCALER_TYPE == _RTD2472D)
      o = 1;
#elif((_SCALER_TYPE == _RTD2545LR) || (_SCALER_TYPE == _RTD247xRD) || (_SCALER_TYPE == _RTD248xRD))
      o = o + 1;
#else
    No Setting !!
#endif // End of #if(_SCALER_TYPE == _RTD2472D)

            CScalerSetDataPortBit(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_DPCR0_38, ~(_BIT5 | _BIT4), (o << 4));

#if(_SCALER_TYPE == _RTD2472D)
            // Calculate Ich for audio PLL
            pData[0] = (m < 5) ? 0 : ((m / 5) - 1);
            CScalerSetDataPortByte(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_DPCR1_39, pData[0] | 0x80);

#elif((_SCALER_TYPE == _RTD2545LR) || (_SCALER_TYPE == _RTD247xRD) || (_SCALER_TYPE == _RTD248xRD))

            // Calculate Ich for audio PLL
//            pData[0] = (m < 1067 / 100) ? 0 : ((m * 100/ 1067) - 1);

            pData[5] = ((DWORD) m * 4 * 100 / 1067);    // 2bit fractional part
            pData[6] = 0x00;

            if (pData[5] >= 10)
            {
                pData[5] -= 10;
            }

            if(pData[5] >= 40)        // 2bit fractional part
            {
                pData[5] -= 40;
                pData[6] |= 0x04;
            }

            if(pData[5] >= 20)        // 2bit fractional part
            {
                pData[5] -= 20;
                pData[6] |= 0x02;
            }

            if(pData[5] >= 10)        // 2bit fractional part
            {
                pData[5] -= 10;
                pData[6] |= 0x01;
            }
            CScalerSetDataPortByte(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_DPCR1_39, pData[6] | 0x70);
#else
            No Setting !!
#endif     // End of #if(_SCALER_TYPE == _RTD2472D)

#if(_SCALER_TYPE == _RTD2472D)
            CScalerSetDataPortByte(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_DPCR3_3B, 0x03); // Enable K and enable VCOSTART
#endif

            CScalerSetDataPortByte(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_CMCR_10, 0x50);  //Enable Double Buffer for K/M/S/D/O

            // Disable PLL
            CScalerSetDataPortBit(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_DPCR0_38, ~(_BIT7 | _BIT6), _BIT7);
            CScalerSetDataPortBit(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_DPCR0_38, ~_BIT0, 0x00);         // Reg DPLL_CMPEN
            CScalerSetDataPortBit(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_DPCR0_38, ~_BIT1, 0x00);         // Reg DPLL_CALLCH
            CScalerSetDataPortBit(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_DPCR0_38, ~_BIT2, 0x00);         // Reg DPLL_CALSW

            // Enable PLL
            CScalerSetDataPortBit(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_DPCR0_38, ~(_BIT7 | _BIT6), 0x00);

#if((_SCALER_TYPE == _RTD2545LR) || (_SCALER_TYPE == _RTD247xRD) || (_SCALER_TYPE == _RTD248xRD))

            // Must follow the folowing sequence to Calibrate DPLL, including the delays!!!!!!!
            CScalerPageSelect(_PAGE2);
            CScalerSetDataPortBit(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_DPCR2_3A, ~(_BIT4 | _BIT3), _BIT4);   // Set VCO default
            CTimerDelayXms(1);
            CScalerSetDataPortBit(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_DPCR0_38, ~_BIT0, _BIT0);             // Reg DPLL_CMPEN
            CTimerDelayXms(1);
            CScalerSetDataPortBit(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_DPCR0_38, ~_BIT1, _BIT1);             // Reg DPLL_CALLCH
            CTimerDelayXms(1);
            CScalerSetDataPortBit(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_DPCR0_38, ~_BIT2, _BIT2);             // Reg DPLL_CALSW
#endif

#if(_AUDIO_LOCK_MODE == _HARDWARE_TRACKING)
            do
            {
                CScalerSetDataPortByte(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_AAPNR_2D, 0x00);  // Disable SDM
                CScalerSetDataPortByte(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_AAPNR_2D, 0x02);  // Enable SDM
                CTimerDelayXms(1);
                CScalerGetDataPortByte(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_DPCR3_3B + 1, 2, pData, _NON_AUTOINC);
            }
            while((((a >> 8) & 0xff) != pData[0]) || (((a >> 0) & 0xff) != pData[1]));
#endif


            CScalerSetDataPortByte(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_AOCR_62, 0x00);//Disable SPDIF/I2S Output
            CAdjustDisableHDMIWatchDog(_WD_SET_AVMUTE_ENABLE);//Disable Set_AVMute Watch Dog
            CScalerSetDataPortBit(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_AVMCR_30, ~_BIT5, _BIT5);//Enable Audio Output
            //CAdjustEnableHDMIWatchDog(_WD_SET_AVMUTE_ENABLE);//Enable Set_AVMute Watch Dog,

#if(_AUDIO_LOCK_MODE == _HARDWARE_TRACKING)
            //H/W FIFO Tracking
            CScalerSetDataPortByte(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_PSCR_15, 0x04);//Enable boundary tracking
            CScalerSetDataPortByte(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_ICBPSR1_25, 0x01);//Set I code
            CScalerSetDataPortByte(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_PCBPSR1_27, 0x01);//Set P code
            CScalerSetDataPortByte(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_STBPR_2A, 0x80);//Set Boundary Tracking Update Response Time
            CScalerSetDataPortByte(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_AAPNR_2D, 0xC2);

            CScalerSetDataPortByte(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_FBR_1B, 0xe2);//0xe5 for DVR team ?
            CScalerSetDataPortBit(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_WDCR0_31, ~_BIT5, _BIT5);//Enable FIFO Tracking

            CScalerSetDataPortByte(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_CMCR_10, 0x50);//update double buffer

            CScalerSetByte(_P2_HDMI_SR_CB, 0x07);//Write 1 clear

            //Fine tune
            CScalerSetDataPortByte(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_PSCR_15, 0xEC);//Enable FIFO Trend
            CScalerSetDataPortByte(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_FTR_1A, 0x03);
            CScalerSetDataPortByte(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_ICTPSR1_21, 0x07);
            CScalerSetDataPortByte(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_CMCR_10, 0x50);//Update Double Buffer
#else
            CScalerSetDataPortByte(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_PSCR_15, 0xfe);//Enable N/CTS tracking
            CScalerSetDataPortByte(_P2_HDMI_ADDR_PORT_C9, 0x1d, 0x05);//Set I code
            CScalerSetDataPortByte(_P2_HDMI_ADDR_PORT_C9, 0x1f, 0x9F);//Set P code
            CScalerSetDataPortByte(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_AAPNR_2D, 0x02);
            CScalerSetDataPortByte(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_CMCR_10, 0x50);//update double buffer
#endif

#if((_SCALER_TYPE == _RTD2545LR) || (_SCALER_TYPE == _RTD247xRD) || (_SCALER_TYPE == _RTD248xRD))
            CScalerPageSelect(_PAGE2);
            CScalerSetDataPortBit(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_DPCR2_3A, ~(_BIT4 | _BIT3), _BIT4); // Set VCO default
            CTimerDelayXms(1);
            CScalerSetDataPortBit(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_DPCR0_38, ~_BIT0, _BIT0);         // Reg DPLL_CMPEN
            CTimerDelayXms(1);
            CScalerSetDataPortBit(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_DPCR0_38, ~_BIT1, _BIT1);         // Reg DPLL_CALLCH
            CTimerDelayXms(1);
            CScalerSetDataPortBit(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_DPCR0_38, ~_BIT2, _BIT2);         // Reg DPLL_CALSW
#endif

            SET_AUDIOWAITINGFLAG();
        }
        else
        {
            CLR_AUDIOWAITINGFLAG();
            CLR_AUDIOPLLLOCKREADY();
        }
    }
    else
    {
        CLR_AUDIOPLLLOCKREADY();
    }
}
#endif


#if((_SCALER_TYPE == _RTD247xRD) || (_SCALER_TYPE == _RTD248xRD) || (_SCALER_TYPE == _RTD2545LR))
//--------------------------------------------------
// Description  : Audio Control
// Input Value  : None
// Output Value : None
//--------------------------------------------------
void CHdmiAudioControl(void)
{
    if(CHdmiAudioEnabledCheck()) // Audio Detect
    {
        if(GET_HDMIAUDIOUNTRACKED())    // Do Audio Tracking
        {
            if (CHdmiAudioFirstTracking())
            {
                SET_HDMIAUDIOTRACKED();
                CLR_HDMIAUDIOUNTRACKED();
            }
            else
            {
                return;
            }
        }

        if(CHdmiAudioReadyDetect())     // Audio Stable
        {
            if(GET_HDMIAUDIOTRACKED())     // Audio Output
            {
                CHdmiEnableAudioOutput();
                CLR_HDMIAUDIOTRACKED();
            }
        }
        else         // Audio Unstable
        {
            SET_HDMIAUDIOUNTRACKED();
        }
    }
    else    // Get AV Mute or No Audio
    {
        CAdjustDisableAudio();
        SET_HDMIAUDIOUNTRACKED();
    }

}
#endif


//--------------------------------------------------
// Description  : Enable Audio Output
// Input Value  : None
// Output Value : None
//--------------------------------------------------
#if((_SCALER_TYPE == _RTD247xRD) || (_SCALER_TYPE == _RTD248xRD) || (_SCALER_TYPE == _RTD2545LR))
void CHdmiEnableAudioOutput(void)
{
    // Enable Audio Watch Dog
    CAdjustEnableHDMIWatchDog(_WD_AUDIO_FOR_TMDS_CLOCK | _WD_AUDIO_FIFO);

    // Audio Normal Output
    CScalerPageSelect(_PAGE2);
    CScalerSetDataPortBit(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_AVMCR_30, ~_BIT5, _BIT5);

#if(_AUDIO_OUTPUT_TYPE == _I2S)

    // Enable I2S Output
    CScalerSetDataPortByte(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_AOCR_62, 0x0f);

#else

    // Enable SPDIF Output
    CScalerSetDataPortByte(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_AOCR_62, 0xf0);

#endif


#if(_EXT_DAC_SUPPORT == _ON)
    CHdmiAdjustDACSampleRate(ucAudioRate);
#endif

    // Setting pin share and Power Control for Audio Dac
    CScalerCodeW(tHDMI_AUDIO_DAC);
}

#else  // Else of #if((_SCALER_TYPE == _RTD247xRD) || (_SCALER_TYPE == _RTD248xRD) || (_SCALER_TYPE == _RTD2545LR))

void CHdmiEnableAudioOutput(void)
{
    CLR_AUDIOWAITINGTIMEOUT();

    if(GET_AUDIOWAITINGFLAG())
    {
        CScalerPageSelect(_PAGE2);
        CScalerSetByte(_P2_HDMI_SR_CB, 0x07);//Write 1 clear
        CLR_AUDIOWAITINGFLAG();
        SET_AUDIOPLLLOCKREADY();
    }
    else
    {
        if (CHdmiAudioFIFODetect() || GET_AVRESUME())//For HDMI audio pll setting
        {
            CHdmiAudioFirstTracking();
            CLR_AUDIOPLLLOCKREADY();
        }
        else if (GET_AUDIOPLLLOCKREADY())
        {
            CLR_AUDIOPLLLOCKREADY();
            CScalerPageSelect(_PAGE2);
#if(_AUDIO_OUTPUT_TYPE == _I2S)
//            CScalerSetDataPortByte(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_AOCR_62, 0x0f);//Enable I2S Output
            CScalerRead(_P2_HDMI_SR_CB, 1, pData, _NON_AUTOINC);
            CScalerGetDataPortByte(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_AOCR_62, 1, &pData[1], _NON_AUTOINC); //Read I2S Output
            if((pData[0] & 0x16) == 0x00) //LPCM & no overflow/underflow in Audio FIFO
            {
                if((pData[1] & 0x0f) != 0x0f)
                {
                    CScalerSetDataPortByte(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_AOCR_62, 0x0f);//Enable I2S Output
                }
            }
            else
            {
                if((pData[1] & 0x0F) != 0x00)
                {
                    CScalerSetDataPortByte(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_AOCR_62, 0x00);//Disable I2S Output
                }
            }
#else
            CScalerSetDataPortByte(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_AOCR_62, 0xf0);//Enable SPDIF Output
#endif

#if(_AUDIO_LOCK_MODE == _HARDWARE_TRACKING)
            CScalerSetDataPortByte(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_AFCR_03, 0x26);//Enable Audio FIFO
#else
            CScalerSetDataPortByte(_P2_HDMI_ADDR_PORT_C9, _P2_HDMI_AFCR_03, 0x40);//Enable Audio FIFO
#endif
            CAdjustEnableHDMIWatchDog(_WD_AUDIO_FOR_TMDS_CLOCK | _WD_AUDIO_FIFO);

#if(_EXT_DAC_SUPPORT == _ON)
            CHdmiAdjustDACSampleRate(ucAudioRate);
#endif


        }
    }

    // Setting pin share and Power Control for Audio Dac

    CScalerCodeW(tHDMI_AUDIO_DAC);

     CTimerActiveTimerEvent(SEC(0), CHdmiAudioWaitingFlagReadyEven);
}
#endif

#if((_SCALER_TYPE == _RTD2472D))
//--------------------------------------------------
// Description  : Audio Waiting Time Flag Ready
// Input Value  : None
// Output Value : None
//--------------------------------------------------
void CHdmiAudioWaitingFlagReadyEven(void)
{
    SET_AUDIOWAITINGTIMEOUT();
}
#endif

#endif  // End of #if(_AUDIO_SUPPORT == _ON)


#if(_EXT_DAC_SUPPORT == _ON)
//--------------------------------------------------
// Description  : Adjust DAC Sample Rate
// Input Value  : None
// Output Value : None
//--------------------------------------------------
void CHdmiAdjustDACSampleRate(BYTE ucModeType)
{
    pData[0] = 0x20;
    pData[1] = (0x90 | ucModeType);
    CI2cWrite(NVRAM_LINE,0x20, 0x0c, 1, &pData[0]);
    CI2cWrite(NVRAM_LINE,0x20, 0x01, 1, &pData[1]);
}
#endif //End of #if(_EXT_DAC_SUPPORT == _ON)

#endif //End of #if(_HDMI_SUPPORT == _ON)

#endif  // End of #if(_SCALER_SERIES_TYPE == _RTD2472D_SERIES)

