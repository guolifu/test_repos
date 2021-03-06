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
// ID Code      : Memory.c No.0000
// Update Note  :
//
//----------------------------------------------------------------------------------------------------

#define __MEMORY__

#include "Common\Header\Include.h"

#if(_SCALER_SERIES_TYPE == _RTD2472D_SERIES)


#if((_OD_SUPPORT == _ON) || (_FRC_SUPPORT == _ON))
//--------------------------------------------------
// Description  : To Confirm SDRAM Size/Speed
// Input Value  : bControl --> OD/FRC Confrim Select
// Output Value : _TRUE/_FALSE for FRC, (Resolution * PixelChannel) for OD
//--------------------------------------------------
BYTE CMemorySDRAMConfirm(bit bControl)
{
#if(_FRC_SUPPORT == _ON)
    WORD usHWidth = 0; // Horizontal Width
    WORD usVHeight = 0; // Vertical Height
#endif

#if(_OD_SUPPORT == _ON)
    BYTE Resolution = 0; // OD Pixel Resolution
    BYTE ResolutionAfterSize = 0; // OD Pixel Resoultion After SDRAM Size Has Been Confirmed
    BYTE PixelChannel = 0; // OD Pixel Channel
    BYTE PixelChannelAfterSize = 0; // OD Pixel Channel After SDRAM Size Has Been Confirmed
    WORD usFrequency = 0; // OD Frequency Requirement
    DWORD ulODStartAddress = 0; // OD Memory Start Address
#endif


    if(bControl == _OD_CONFIRM)
    {
        CScalerPageSelect(_PAGE5);

        if(CScalerGetBit(_P5_SDRF_MN_DISP_CTRL_CF, (_BIT2 | _BIT1)) == 0x00)
        {
            SET_FRCENABLE();
        }
        else
        {
            CLR_FRCENABLE();
        }
    }

//--------------------FRC Memory Confirm-------------------//
#if(_FRC_SUPPORT == _ON)

    if(stModeInfo.IHWidth < stDisplayInfo.DHWidth)
    {
        usHWidth = stModeInfo.IHWidth;
    }
    else
    {
        usHWidth = stDisplayInfo.DHWidth;
    }

    if(stModeInfo.IVHeight < stDisplayInfo.DVHeight)
    {
        usVHeight = stModeInfo.IVHeight;
    }
    else
    {
        usVHeight = stDisplayInfo.DVHeight;
    }

    CLR_FRCDOUBLEFRAME();

#if(_FRC_YUV_MODE_SUPPORT == _ON)

    if(GET_FRC16BITSIN())
    {
        ((DWORD *)pData)[0] = (DWORD)usHWidth * usVHeight * 2 * 8 * 2 / 1048576;

        if(_MEMORY_SIZE < ((DWORD *)pData)[0])
        {
            CLR_FRC422STATUS();
            CLR_FRC16BITSIN();

            if(!GET_FRCENABLE() && bControl == _FRC_CONFIRM)
            {
                return _FALSE;
            }

        }

        ((DWORD *)pData)[1] = (DWORD)(usHWidth) * (DWORD)(usVHeight) * ((DWORD)stModeInfo.IVFreq + _PANEL_MAX_FRAME_RATE) / 10 / 85 * 100 / _MEMORY_BUS_WIDTH * 16;

        if(((DWORD *)pData)[1] > (DWORD)_MEMORY_SPEED * 1000000)
        {
            CLR_FRC422STATUS();
            CLR_FRC16BITSIN();

            if(!GET_FRCENABLE() && bControl == _FRC_CONFIRM)
            {
                return _FALSE;
            }
        }

        SET_FRC422STATUS();
        CScalerSetByte(_I_YUV444to422_26, 0x00); //Disable 444->422
    }
    else //444 In
    {
        ((DWORD *)pData)[0] = (DWORD)usHWidth * usVHeight * 3 * 8 * 2 / 1048576;

        if(_MEMORY_SIZE < ((DWORD *)pData)[0])
        {
            ((DWORD *)pData)[0] = (DWORD)usHWidth * usVHeight * 2 * 8 * 2 / 1048576;

            if(_MEMORY_SIZE < ((DWORD *)pData)[0])
            {
                CLR_FRC422STATUS();

                if(!GET_FRCENABLE() && bControl == _FRC_CONFIRM)
                {
                    return _FALSE;
                }

            }

            SET_FRC422STATUS();
        }

        ((DWORD *)pData)[1] = (DWORD)(usHWidth) * (DWORD)(usVHeight) * ((DWORD)stModeInfo.IVFreq * 2) / 10 / 85 * 100 / _MEMORY_BUS_WIDTH * 24;

        if(((DWORD *)pData)[1] > (DWORD)_MEMORY_SPEED * 1000000)
        {
            ((DWORD *)pData)[1] = (DWORD)(usHWidth) * (DWORD)(usVHeight) * ((DWORD)stModeInfo.IVFreq * 2) / 10 / 85 * 100 / _MEMORY_BUS_WIDTH * 16;

            if(((DWORD *)pData)[1] > (DWORD)_MEMORY_SPEED * 1000000)
            {
                CLR_FRC422STATUS();

                if(!GET_FRCENABLE() && bControl == _FRC_CONFIRM)
                {
                    return _FALSE;
                }
            }

            SET_FRC422STATUS();
        }

        if(GET_FRC422STATUS())
        {
            if(GET_FRCRGBIN())
            {
                CScalerCodeW(tFRC_TABLE_RGB2YUV); //I Domain RGB->YUV
                CScalerCodeW(tFRC_TABLE_YUV2RGB); //D Domain YUV->RGB
            }

            //CScalerSetByte(_I_YUV444to422_26, 0xCB); //Enable VSD Output as 444->422 Input, 444->422 Oute as FIFO Input, Mode 1
            CScalerSetByte(_I_YUV444to422_26, 0x0B); //Enable VSD Output as 444->422 Input, 444->422 Oute as FIFO Input, Mode 1
            SET_FRC16BITSIN();
        }
        else
        {
            CScalerSetByte(_I_YUV444to422_26, 0x00); //Disable 444->422
            CLR_FRC16BITSIN();
        }
    } // End of if(GET_FRC16BITSIN())

#else // #if(_FRC_YUV_MODE_SUPPORT != _ON)

    ((DWORD *)pData)[0] = (DWORD)usHWidth * usVHeight * 3 * 8 * 2 / 1048576; // Double Frame


    if(_MEMORY_SIZE < ((DWORD *)pData)[0])
    {
        CLR_FRCDOUBLEFRAME();
        ((DWORD *)pData)[0] = (DWORD)usHWidth * usVHeight * 3 * 8 * 1 / 1048576; //Single Frame


        if(_MEMORY_SIZE < ((DWORD *)pData)[0])
        {
            if(!GET_FRCENABLE() && bControl == _FRC_CONFIRM)
            {
                return _FALSE;
            }

        }
        else //1:1 mode ok
        {
            ((DWORD *)pData)[1] = (DWORD)(usHWidth) * (DWORD)(usVHeight) * ((DWORD)stModeInfo.IVFreq * 2) / 10 / 81 * 100 / _MEMORY_BUS_WIDTH * 24;


        }
    }
    else //Frame Rate Conversion ok
    {
        SET_FRCDOUBLEFRAME();
        ((DWORD *)pData)[1] = (DWORD)(usHWidth) * (DWORD)(usVHeight) * ((DWORD)stModeInfo.IVFreq + _PANEL_MAX_FRAME_RATE) / 10 / 81 * 100 / _MEMORY_BUS_WIDTH * 24;


    } // End of if(_MEMORY_SIZE < ((DWORD *)pData)[0])

    if(((DWORD *)pData)[1] > (DWORD)_MEMORY_SPEED * 1000000)
    {
        if(bControl == _FRC_CONFIRM)
        {
            return _FALSE;
        }

    }

#endif // End of #if(_FRC_YUV_MODE_SUPPORT == _ON)


    if(bControl == _FRC_CONFIRM)
    {
        return _TRUE;
    }

#endif // End of #if(_FRC_SUPPORT == _ON)


//---------------------OD Memory Confirm-------------------//
#if(_OD_SUPPORT == _ON)

    for(PixelChannel = 3; PixelChannel > 1; PixelChannel--)
    {
        for(Resolution = 6; Resolution > 3; Resolution--)
        {
            if(!GET_FRCENABLE())
            {
                ((DWORD *)pData)[2] = ((DWORD)_PANEL_DH_WIDTH * _PANEL_DV_HEIGHT) * PixelChannel * Resolution / 1048576;


                if((((DWORD *)pData)[2] < _MEMORY_SIZE) && (Resolution > 3))
                {
                    break;
                }
            }
#if(_FRC_SUPPORT == _ON)
            else //FRC ON
            {
#if(_OD_HW_MODE_SUPPORT == _ON)
                ((DWORD *)pData)[2] = ((DWORD)usHWidth * usVHeight) * PixelChannel * Resolution / 1048576;
#else
                ((DWORD *)pData)[2] = ((DWORD)_PANEL_DH_WIDTH * _PANEL_DV_HEIGHT) * PixelChannel * Resolution / 1048576;
#endif


                if((((DWORD *)pData)[2] < (_MEMORY_SIZE - ((DWORD *)pData)[0])) && (Resolution > 3))
                {
                    break;
                }
            }

#endif // End of #if(_FRC_SUPPORT == _ON)
        } // End of for(Resolution=6; Resolution>3; Resolution--)

        if(!GET_FRCENABLE())
        {

            if((((DWORD *)pData)[2] < _MEMORY_SIZE) && (Resolution > 3))
            {
                break;
            }

        }
        else
        {

            if((((DWORD *)pData)[2] < (_MEMORY_SIZE - ((DWORD *)pData)[0])) && (Resolution > 3))
            {
                break;
            }

        }
    } // End of for(PixelChannel=3; PixelChannel>1; PixelChannel--)

    if((Resolution < 4) && (PixelChannel <= 2))
    {
        return _FALSE;
    }

    ResolutionAfterSize = Resolution; //Record for OD Use
    PixelChannelAfterSize = PixelChannel; //Record for OD Use


#if(_OD_HW_MODE_SUPPORT == _ON)
    // Calculate and set display clock frequency
    //((DWORD *)pData)[3] = (DWORD)(stDisplayInfo.DHTotal) * (DWORD)stModeInfo.IHFreq * (DWORD)(stDisplayInfo.DVHeight) / stModeInfo.IVHeight / 10;
    ((DWORD *)pData)[3] = (DWORD)(stDisplayInfo.DHTotal) * (DWORD)stModeInfo.IHFreq * (DWORD)(_PANEL_DV_HEIGHT) / stModeInfo.IVHeight / 10; //Anderson V008_20080730 to Solve the Efficiency Issue
#else
    ((DWORD *)pData)[3] = (DWORD)(stDisplayInfo.DHTotal) * (DWORD)stModeInfo.IHFreq * (DWORD)(_PANEL_DV_HEIGHT) / stModeInfo.IVHeight / 10;
#endif


    for(PixelChannel = PixelChannelAfterSize; PixelChannel > 1; PixelChannel--)
    {
        for(Resolution = ResolutionAfterSize; Resolution > 3; Resolution--)
        {
            if(!GET_FRCENABLE())
            {
                usFrequency = (DWORD)_PANEL_DH_WIDTH * ((DWORD *)pData)[3] / _PANEL_DH_TOTAL * Resolution * PixelChannel * 2 / _MEMORY_BUS_WIDTH * 100 / 90 / 1000;


                if((usFrequency < _MEMORY_SPEED) && (Resolution > 3))
                {
                    break;
                }
            }
#if(_FRC_SUPPORT == _ON)
            else
            {
#if(_OD_HW_MODE_SUPPORT == _ON)
                //Frequency = (DWORD)HWidth * ((DWORD *)pData)[3] / _PANEL_DH_TOTAL * Resolution * PixelChannel * 2 / _MEMORY_BUS_WIDTH * 100 / 90 / 1000;
                usFrequency = (DWORD)_PANEL_DH_WIDTH * ((DWORD *)pData)[3] / _PANEL_DH_TOTAL * Resolution * PixelChannel * 2 / _MEMORY_BUS_WIDTH * 100 / 90 / 1000;
#else
                usFrequency = (DWORD)_PANEL_DH_WIDTH * ((DWORD *)pData)[3] / _PANEL_DH_TOTAL * Resolution * PixelChannel * 2 / _MEMORY_BUS_WIDTH * 100 / 90 / 1000;

#endif // End of #if(_OD_HW_MODE_SUPPORT == _ON)


                if((((DWORD)usFrequency * 1000000) < ((DWORD)_MEMORY_SPEED * 1000000 - ((DWORD *)pData)[1])) && (Resolution > 3))
                {
                    break;
                }

            } // End of if(!GET_FRCENABLE())

#endif  // End of #if(_FRC_SUPPORT == _ON)

        } // End of for(Resolution=ResolutionAfterSize; Resolution>3; Resolution--)

        if(!GET_FRCENABLE())
        {
            if((usFrequency < _MEMORY_SPEED) && (Resolution > 3))
            {
                break;
            }
        }
        else
        {
            if((((DWORD)usFrequency * 1000000) < ((DWORD)_MEMORY_SPEED * 1000000 - ((DWORD *)pData)[1])) && (Resolution > 3))
            {
                break;
            }
        }

    } // End of for(PixelChannel=PixelChannelAfterSize; PixelChannel>1; PixelChannel--)


    if((Resolution < 4) && (PixelChannel <= 2))
    {
        return _FALSE;
    }

    ulODStartAddress = _OD_START_ADDRESS;

#if(_FRC_SUPPORT == _ON)

#if(_FRC_YUV_MODE_SUPPORT == _ON)

    if(GET_FRC422STATUS())
    {
        ((DWORD *)pData)[0] = (DWORD)usHWidth * usVHeight * 2 * 8 * 2 / _MEMORY_BUS_WIDTH;
    }
    else
    {
        ((DWORD *)pData)[0] = (DWORD)usHWidth * usVHeight * 3 * 8 * 2 / _MEMORY_BUS_WIDTH;
    }

#else

    if(GET_FRCDOUBLEFRAME())
    {
        ((DWORD *)pData)[0] = (DWORD)usHWidth * usVHeight * 3 * 8 * 2 / _MEMORY_BUS_WIDTH;
    }
    else
    {
        ((DWORD *)pData)[0] = (DWORD)usHWidth * usVHeight * 3 * 8 * 1 / _MEMORY_BUS_WIDTH;
    }

#endif // End of #if(_FRC_YUV_MODE_SUPPORT == _ON)

#endif // End of #if(_FRC_SUPPORT == _ON)


#if((_OD_SUPPORT == _ON) && (_FRC_SUPPORT == _ON))
    if(GET_FRCENABLE())
    {
        ulODStartAddress += ((DWORD *)pData)[0];
    }
#endif


//---------------------OD Highlight Window-----------------//
#if(_OD_HW_MODE_SUPPORT == _ON)

    CScalerSetByte(_DISP_ACCESS_PORT_2A, (0x80 | _DISP_DH_BKGD_STA_H_03));
    CScalerRead(_DISP_DATA_PORT_2B, 4, pData, _NON_AUTOINC);//Read Display Horizontal Background Start
    ((WORD *)pData)[7] = ((WORD)pData[2] << 8 | pData[3]) - ((WORD)pData[0] << 8 | pData[1]);//Display Horizontal Background Start

    pData[0] = HIBYTE(((WORD *)pData)[7]);
    pData[1] = LOBYTE(((WORD *)pData)[7]);
    pData[2] = HIBYTE(((WORD *)pData)[7] + stDisplayInfo.DHWidth);
    pData[3] = LOBYTE(((WORD *)pData)[7] + stDisplayInfo.DHWidth);

    CScalerSetByte(_DISP_ACCESS_PORT_2A, (0x80 | _DISP_DV_BKGD_STA_H_0E));
    CScalerRead(_DISP_DATA_PORT_2B, 4, &pData[4], _NON_AUTOINC);//Read Display Vertical Background Start
    ((WORD *)pData)[7] = ((WORD)pData[6] << 8 | pData[7]) - ((WORD)pData[4] << 8 | pData[5]);//Display Vertical Background Start

    pData[4] = HIBYTE(((WORD *)pData)[7]);
    pData[5] = LOBYTE(((WORD *)pData)[7]);
    pData[6] = HIBYTE(((WORD *)pData)[7] + stDisplayInfo.DVHeight);
    pData[7] = LOBYTE(((WORD *)pData)[7] + stDisplayInfo.DVHeight);
    pData[8] = 0x00;//Border Width = 0

    CScalerSetBit(_HW_ACCESS_PORT_60, ~(_BIT7 | _BIT6 | _BIT3 | _BIT2 | _BIT1 | _BIT0), _BIT7);
    CTimerWaitForEvent(_EVENT_DEN_STOP);
    CScalerWrite(_HW_DATA_PORT_61, 9, pData, _NON_AUTOINC);
    CScalerSetBit(_HW_ACCESS_PORT_60, ~(_BIT7 | _BIT6), _BIT6);

#endif // End of #if(_OD_HW_MODE_SUPPORT == _ON)


    if(bControl == _OD_CONFIRM)
    {
        CScalerPageSelect(_PAGE3);
        pData[0] = (((DWORD)ulODStartAddress >> 16) & 0x3f);
        pData[1] = (((DWORD)ulODStartAddress >> 8) & 0xff);
        pData[2] = ((DWORD)ulODStartAddress & 0xff);
        CScalerWrite(_P3_LS_MEM_START_ADDR_H_C6, 3, pData, _AUTOINC);


        return (Resolution * PixelChannel);
    }

#endif// End of #if(_OD_SUPPORT == _ON)

    return _TRUE; //To Prevent No Return Value
}

//--------------------------------------------------
// Description  : Reset SDRAM
// Input Value  : NONE
// Output Value : NONE
//--------------------------------------------------
void CMemorySDRAMReset(void)
{
    CScalerPageSelect(_PAGE4);
    CScalerSetBit(_P4_SDR_PRCG_A4, ~_BIT4, _BIT4);
    CTimerDelayXms(5);
    CScalerSetBit(_P4_SDR_PRCG_A4, ~_BIT4, 0x00);
}

//--------------------------------------------------
// Description  : Calculate the number and remain
// Input Value  : ulTotalSize --> Image total size
//                ucLength --> Length of SDRAM access
// Output Value : Number and Remain
//--------------------------------------------------
DWORD CMemoryCalculateNumberAndRemain(DWORD ulTotalSize, BYTE ucLength, bit bDummy)
{
    BYTE Remain = 0;
    WORD usNumber = 0;

    if(bDummy)
    {
        if (ulTotalSize & 0x03)
        {
            (ulTotalSize += 4 - (ulTotalSize & 0x03));
        }
    }

    if((ulTotalSize % ucLength) != 0)
    {
        usNumber = ulTotalSize / ucLength;
        Remain = ulTotalSize % ucLength;
    }
    else
    {
        ulTotalSize = ulTotalSize - ucLength;
        usNumber = ulTotalSize / ucLength;
        Remain = ucLength;
    }

    return (((DWORD)Remain << 16) | usNumber);
}
#endif //End of #if((_OD_SUPPORT == _ON) || (_FRC_SUPPORT == _ON))

#if(_OD_SUPPORT == _ON)
//--------------------------------------------------
// Description  : Load OD Table to LUT
// Input Value  : ucColor --> LUT Channel Select
// Output Value : NONE
//--------------------------------------------------
void CMemoryLoadODLUT(BYTE ucColor)
{
    BYTE Repeat = 0; //Repeat LUT or Not

    CScalerPageSelect(_PAGE3);
    CScalerSetBit(_P3_LS_LUT_ROW_ADDR_AE, ~_BIT7, _BIT7);

    if((ucColor == _OD_TABLE_COMMON))
    {
        CScalerSetBit(_P3_LS_LUT_COL_ADDR_AF, ~(_BIT7 | _BIT6), _OD_TABLE_COMMON << 6);
        CScalerSetBit(_P3_LS_LUT_ROW_ADDR_AE, 0xc0, 0x00); // Hudson add 20100225 set LUT row selector = 0
        CScalerSetBit(_P3_LS_LUT_COL_ADDR_AF, 0xc0, 0x00); // Hudson add 20100225 set LUT column selector = 0
        CMemoryLoadODTable(tOD_TABLE[_OD_TABLE_MIDDLE]);
    }
    else
    {
        for(Repeat = 0; Repeat < 3; Repeat++)
        {
            CScalerSetBit(_P3_LS_LUT_COL_ADDR_AF, ~(_BIT7 | _BIT6), Repeat << 6);
            CScalerSetBit(_P3_LS_LUT_ROW_ADDR_AE, 0xc0, 0x00); // Hudson add 20100225 set LUT row selector = 0
            CScalerSetBit(_P3_LS_LUT_COL_ADDR_AF, 0xc0, 0x00); // Hudson add 20100225 set LUT column selector = 0
            CMemoryLoadODTable(tOD_TABLE[Repeat]);
        }
    }

    CScalerSetBit(_P3_LS_LUT_ROW_ADDR_AE, ~_BIT7, 0x00);
}

//--------------------------------------------------
// Description  : Load OD Table to LUT
// Input Value  : pChannelArray --> OD Table Start Address
// Output Value : NONE
//--------------------------------------------------
void CMemoryLoadODTable(BYTE *pChannelArray)
{
    CScalerPageSelect(_PAGE3);
    CScalerWrite(_P3_LS_LUT_DATA_B0, 1088, pChannelArray, _NON_AUTOINC);
}

//--------------------------------------------------
// Description  : OD On/Off Select
// Input Value  : ucSelect --> On or Off
// Output Value : NONE
//--------------------------------------------------
void CMemorySetODOnOff(BYTE ucSelect)
{
    CScalerPageSelect(_PAGE3);
    CScalerSetBit(_P3_LS_CTRL0_A1, ~(_BIT6 | _BIT5 | _BIT3), (_BIT6 | _BIT5 | _BIT3));

#if(_OD_HW_MODE_SUPPORT == _ON)
    CScalerSetBit(_P3_LS_CTRL1_A2, ~_BIT6, 0x00);//Disable Inside Highlight Window
    CScalerSetByte(_P3_LS_CTRL1_A2, ((BYTE)_OD_LBNRT_ENABLE << 4 | _OD_LBNRT_MODE << 3 | _OD_LBNRT_LEVEL) | _BIT7);
#else
    CScalerSetByte(_P3_LS_CTRL1_A2, ((BYTE)_OD_LBNRT_ENABLE << 4 | _OD_LBNRT_MODE << 3 | _OD_LBNRT_LEVEL) | _BIT7 | _BIT6);
#endif

    if((bit)CScalerGetBit(_P3_LS_CPRS_CTRL_A4, _BIT4))
    {
        CScalerSetByte(_P3_LS_FRAME0_CC, 0x19);
        CScalerSetByte(_P3_LS_FRAME1_CD, 0x99);
        CScalerSetBit(_P3_LS_FRAME2_CE, 0x00, (_YC_OD_PIXDIFFVAL & 0x3f));
    }
    else
    {
        CScalerSetByte(_P3_LS_FRAME0_CC, 0x00);
        CScalerSetByte(_P3_LS_FRAME1_CD, 0x00);
        CScalerSetBit(_P3_LS_FRAME2_CE, 0x00, (0 & 0x3f));
    }

    CScalerSetByte(_P3_LS_STATUS0_B6, 0x01);

    CAdjustODeltaGain(stSystemData.ODTable);

    if(ucSelect == _ON)
    {
        CScalerSetBit(_P3_LS_CTRL0_A1, ~(_BIT7 | _BIT6), (_BIT7 | _BIT6));
    }
    else
    {
        CScalerSetBit(_P3_LS_CTRL0_A1, ~(_BIT7 | _BIT6), 0x00);
    }

}

//--------------------------------------------------
// Description  : OD Compress
// Input Value  : NONE
// Output Value : NONE
//--------------------------------------------------
bit CMemorySetODCompress(void)
{
    BYTE Compress = 0; //Compression Enable or Disable
    BYTE Resolution = 0; //OD Resolution
    WORD usTarget = 0; //Target Size

    CScalerPageSelect(_PAGE3);

    Resolution = CMemorySDRAMConfirm(_OD_CONFIRM);

    if(Resolution == 18)
    {
        CScalerSetBit(_P3_LS_CTRL0_A1, ~(_BIT2 | _BIT1 | _BIT0), (((_DISP_BIT_MODE << 7) >> 5) | 0x00));

        CScalerSetBit(_P3_LS_CPRS_CTRL_A4, ~(_BIT6 | _BIT5 | _BIT4), _BIT6);

        Compress = 0;
    }
    else if(Resolution == 15)
    {
        CScalerSetBit(_P3_LS_CTRL0_A1, ~(_BIT2 | _BIT1 | _BIT0), (((_DISP_BIT_MODE << 7) >> 5) | 0x01));

        CScalerSetBit(_P3_LS_CPRS_CTRL_A4, ~(_BIT6 | _BIT5 | _BIT4), _BIT5);

        Compress = 0;
    }
    else if(Resolution == 12) // Use YUV 6bit OD
    {
        //CScalerSetBit(_P3_LS_CTRL0_A1, ~(_BIT2 | _BIT1 | _BIT0),(((_DISP_BIT_MODE << 7) >> 5) | 0x02));
        //CScalerSetBit(_P3_LS_CPRS_CTRL_A4, ~(_BIT6 | _BIT5), 0x00);
        CScalerSetBit(_P3_LS_CTRL0_A1, ~(_BIT2 | _BIT1 | _BIT0), (((_DISP_BIT_MODE << 7) >> 5) | 0x00));
        CScalerSetBit(_P3_LS_CPRS_CTRL_A4, ~(_BIT6 | _BIT5 | _BIT4), (_BIT6 | _BIT4));
        Compress = 0;
    }
    else if(Resolution == 10)
    {
        CScalerSetBit(_P3_LS_CTRL0_A1, ~(_BIT2 | _BIT1 | _BIT0), (((_DISP_BIT_MODE << 7) >> 5) | 0x01));
        CScalerSetBit(_P3_LS_CPRS_CTRL_A4, ~(_BIT6 | _BIT5 | _BIT4), (_BIT5 | _BIT4));
        Compress = 0;
    }
    else if(Resolution == 8)
    {
        CScalerSetBit(_P3_LS_CTRL0_A1, ~(_BIT2 | _BIT1 | _BIT0), (((_DISP_BIT_MODE << 7) >> 5) | 0x02));
        CScalerSetBit(_P3_LS_CPRS_CTRL_A4, ~(_BIT6 | _BIT5 | _BIT4), _BIT4);
        Compress = 0;
    }
    else
    {
        return _FALSE;
    }

    if((Resolution == 12) || (Resolution == 10) || (Resolution == 8))
    {
        Compress = _FALSE;
    }
    else
    {
        Compress = (Compress | _OD_COMPRESSION);
    }

    CScalerSetBit(_P3_LS_CTRL0_A1, ~_BIT4, (Compress << 4));

    if(Compress == _TRUE)
    {
        //Rounding to 5bit/4bit
        if(Resolution == 18)
        {
            CScalerSetBit(_P3_LS_CPRS_CTRL_A4, ~(_BIT6 | _BIT5), _BIT5);
            Resolution = 15;
        }
        else
        {
            CScalerSetBit(_P3_LS_CPRS_CTRL_A4, ~(_BIT6 | _BIT5), 0x00);
            Resolution = 12;
        }

        pData[0] = (HIBYTE(_COMPRESS_GROUP_NUMBER) & 0x07);
        pData[1] = (LOBYTE(_COMPRESS_GROUP_NUMBER));
        CScalerWrite(_P3_LS_GRP_NUM_H_A7, 2, pData, _AUTOINC);

        usTarget = (WORD)Resolution * _COMPRESS_GROUP_NUMBER / 64;

        pData[0] = (HIBYTE(usTarget) & 0x01);
        pData[1] = (LOBYTE(usTarget));
        CScalerWrite(_P3_LS_TG_SIZE_H_A5, 2, pData, _AUTOINC);
    }

    return _TRUE;
}

//--------------------------------------------------
// Description  : Set OD FIFO
// Input Value  : NONE
// Output Value : NONE
//--------------------------------------------------
void CMemorySetODFIFO(void)
{
    BYTE Resolution = 18; //OD Resolution
    BYTE Length = 128; //SDRAM Length
    DWORD ulValue = 0; //Number and Remain
    WORD usHWidth = 0; //Horizontal Width
    WORD usVHeight = 0; //Vertical Height

    if(stModeInfo.IHWidth < stDisplayInfo.DHWidth)
    {
        usHWidth = stModeInfo.IHWidth;
    }
    else
    {
        usHWidth = stDisplayInfo.DHWidth;
    }

    if(stModeInfo.IVHeight < stDisplayInfo.DVHeight)
    {
        usVHeight = stModeInfo.IVHeight;
    }
    else
    {
        usVHeight = stDisplayInfo.DVHeight;
    }

    CScalerPageSelect(_PAGE3);
    CScalerRead(_P3_LS_CPRS_CTRL_A4, 1, pData, _NON_AUTOINC);

    if((pData[0] & 0x10) == 0x10)
    {

        if((pData[0] & 0x40) == 0x40)
        {
            Resolution = 12;
        }
        else if((pData[0] & 0x20) == 0x20)
        {
            Resolution = 10;
        }
        else
        {
            Resolution = 8;
        }

    }
    else
    {
        if((pData[0] & 0x40) == 0x40)
        {
            Resolution = 18;
        }
        else if((pData[0] & 0x20) == 0x20)
        {
            Resolution = 15;
        }
        else
        {
            Resolution = 12;
        }

    }

#if(_OD_HW_MODE_SUPPORT == _ON)

    if(GET_FRCENABLE())
    {
        ((DWORD *)pData)[0] = (((DWORD)usHWidth * usVHeight) * Resolution % 64) ?
                              ((((DWORD)usHWidth * usVHeight) * Resolution / 64) + 1) :
                              (((DWORD)usHWidth * usVHeight) * Resolution / 64); //One frame total pixel for SDRAM (unit : 64 bits)
    }
    else
#endif // End of #if(_OD_HW_MODE_SUPPORT == _ON)
    {
        ((DWORD *)pData)[0] = (((DWORD)_PANEL_DH_WIDTH * _PANEL_DV_HEIGHT) * Resolution % 64) ?
                              ((((DWORD)_PANEL_DH_WIDTH * _PANEL_DV_HEIGHT) * Resolution / 64) + 1) :
                              (((DWORD)_PANEL_DH_WIDTH * _PANEL_DV_HEIGHT) * Resolution / 64);// One frame total pixel for SDRAM (unit : 64 bits)
    }

    ulValue = CMemoryCalculateNumberAndRemain(((DWORD *)pData)[0], Length, 1);

    CScalerPageSelect(_PAGE3);
    CScalerSetByte(_P3_LS_WTLVL_W_C0, (Length / 2));
    CScalerSetByte(_P3_LS_WTLVL_R_C1, (Length / 2));

    pData[0] = (HIBYTE(ulValue));
    pData[1] = (LOBYTE(ulValue));
    CScalerWrite(_P3_LS_MEM_FIFO_RW_NUM_H_C2, 2, pData, _AUTOINC);

    CScalerSetByte(_P3_LS_MEM_FIFO_RW_LEN_C4, Length);
    CScalerSetByte(_P3_LS_MEM_FIFO_RW_REMAIN_C5, ((ulValue >> 16) & 0xff));
}
#endif //End of #if(_OD_SUPPORT == _ON)

#if(_FRC_SUPPORT == _ON)
//--------------------------------------------------
// Description  : IN1&MAIN FIFO Setting for FRC Mode
// Input Value  : usHorSize --> Horizontal size
///               usVerSize --> Vertical Size
// Output Value : None
//--------------------------------------------------
void CMemoryControlForFRC(WORD usHorSize, WORD usVerSize)
{
    CScalerSetByte(_FIFO_FREQUENCY_22, 0x00);
    CMemoryCtrlForFRCInput(usHorSize, usVerSize);
    CMemoryCtrlForFRCDisplay(usHorSize, usVerSize);
}

//--------------------------------------------------
// Description  : IN1 FIFO Setting for FRC Mode
// Input Value  : ucHorSize --> Horizontal size
//                ucVerSize --> Vertical Size
// Output Value : None
//--------------------------------------------------
void CMemoryCtrlForFRCInput(WORD usHorSize, WORD usVerSize)
{
    BYTE Length = 160; //SDRAM Length
    DWORD ulValue = 0; //Number and Remain
    DWORD ulStep = 0; //Line Step

#if(_FRC_YUV_MODE_SUPPORT == _ON)

    if(GET_FRC422STATUS())
    {
        ((DWORD *)pData)[0] = ((DWORD)usHorSize * 2 * 8 % 64) ?
                              (((DWORD)usHorSize * 2 * 8 / 64) + 1) :
                              ((DWORD)usHorSize * 2 * 8 / 64);// One line total pixel for SDRAM (unit : 64 bits)
    }
    else
#endif
    {

#if(_FRC_MODE == _LINE_MODE)
        ((DWORD *)pData)[0] = ((DWORD)usHorSize * 3 * 8 % 64) ?
                              (((DWORD)usHorSize * 3 * 8 / 64) + 1) :
                              ((DWORD)usHorSize * 3 * 8 / 64);// One line total pixel for SDRAM (unit : 64 bits)
#else
        ((DWORD *)pData)[0] = ((DWORD)usHorSize * usVerSize * 3 * 8 % 64) ?
                              (((DWORD)usHorSize * usVerSize * 3 * 8 / 64) + 1) :
                              ((DWORD)usHorSize * usVerSize * 3 * 8 / 64);// One Frame total pixel for SDRAM (unit : 64 bits)
#endif // End of #if(_FRC_MODE == _LINE_MODE)

    }

    ulValue = CMemoryCalculateNumberAndRemain(((DWORD *)pData)[0], Length, 1);
    ulStep = CMemoryCalculateLineStepAndBlockStep(usHorSize, usVerSize);

    pData[0] = Length;
    pData[1] = HIBYTE(ulValue);
    pData[2] = LOBYTE(ulValue);
    pData[3] = Length;
    pData[4] = (ulValue >> 16) & 0xff;
    pData[5] = (BYTE)(_FRC_1ST_BLOCK_STA_ADDR >> 16);
    pData[6] = (BYTE)(_FRC_1ST_BLOCK_STA_ADDR >> 8);
    pData[7] = (BYTE)(_FRC_1ST_BLOCK_STA_ADDR >> 0);
    pData[8] = (BYTE)(ulStep >> 24); //LINE_STEP
    pData[9] = (BYTE)(ulStep >> 16); //LINE_STEP

#if(_FIELD_MERGE_SUPPORT == _ON)

    if(GET_FIELDMERGE_MODE())
    {
        pData[10] = (BYTE)(_FRC_2ND_BLOCK_STA_ADDR >> 20); //BLOCK_STEP
        pData[11] = (BYTE)(_FRC_2ND_BLOCK_STA_ADDR >> 12); //BLOCK_STEP
    }
    else
    {
        pData[10] = 0x00; //BLOCK_STEP
        pData[11] = 0x00; //BLOCK_STEP
    }

#else
    pData[10] = 0x00; //BLOCK_STEP
    pData[11] = 0x00; //BLOCK_STEP
#endif

    pData[12] = (BYTE)(_FRC_2ND_BLOCK_STA_ADDR >> 16);
    pData[13] = (BYTE)(_FRC_2ND_BLOCK_STA_ADDR >> 8);
    pData[14] = (BYTE)(_FRC_2ND_BLOCK_STA_ADDR >> 0);

    CScalerPageSelect(_PAGE5);
    CScalerWrite(_P5_SDRF_IN1_WATER_LEVEL_A8, 15, pData, _AUTOINC);

#if(_FRC_MODE == _LINE_MODE)
    pData[0] = (((usVerSize >> 8) & 0x0f) | 0x00);
#else
    pData[0] = (((usVerSize >> 8) & 0x0f) | 0x10);
#endif // End of #if(_FRC_MODE == _LINE_MODE

    pData[1] = LOBYTE(usVerSize);

#if(_FIELD_MERGE_SUPPORT == _ON)

    if(GET_FIELDMERGE_MODE())
    {
        pData[2] = 0xe2; //0xc2;
    }
    else
#if(_FRC_YUV_MODE_SUPPORT == _ON)
    {
        pData[2] = 0xa2; //Double Buffer
    }
#else
    {
        if(GET_FRCDOUBLEFRAME())
        {
            pData[2] = 0xa2; //Double Buffer
        }
        else
        {
            pData[2] = 0x82; //Single Buffer
        }
    }
#endif // End of #if(_FRC_YUV_MODE_SUPPORT == _ON)

#else // #if(_FIELD_MERGE_SUPPORT != _ON)

#if(_FRC_YUV_MODE_SUPPORT == _ON)
    pData[2] = 0xa2;//Double Buffer
#else

    if(GET_FRCDOUBLEFRAME())
    {
        pData[2] = 0xa2;//Double Buffer
    }
    else
    {
        pData[2] = 0x82;//Single Buffer
    }

#endif // End of #if(_FRC_YUV_MODE_SUPPORT == _ON)

#endif // End of #if(_FIELD_MERGE_SUPPORT == _ON)

    CScalerWrite(_P5_SDRF_IN1_LINE_NUM_H_B7, 3, pData, _AUTOINC);
}

//--------------------------------------------------
// Description  : MAIN FIFO Setting for FRC Mode
// Input Value  : usHorSize --> Horizontal size
//                usVerSize --> Vertical Size
// Output Value : None
//--------------------------------------------------
void CMemoryCtrlForFRCDisplay(WORD usHorSize, WORD usVerSize)
{

    BYTE Length = 160;
    DWORD ulValue = 0; //Number and Remain
    DWORD ulStep = 0; //Line Step

#if(_FRC_YUV_MODE_SUPPORT == _ON)
    if(GET_FRC422STATUS())
    {
        ((DWORD *)pData)[0] = ((DWORD)usHorSize * 2 * 8 % 64) ?
                              (((DWORD)usHorSize * 2 * 8 / 64) + 1) :
                              ((DWORD)usHorSize * 2 * 8 / 64);// One line total pixel for SDRAM (unit : 64 bits)
    }
    else
#endif
    {
#if(_FRC_MODE == _LINE_MODE)
        ((DWORD *)pData)[0] = ((DWORD)usHorSize * 3 * 8 % 64) ?
                              (((DWORD)usHorSize * 3 * 8 / 64) + 1) :
                              ((DWORD)usHorSize * 3 * 8 / 64);// One line total pixel for SDRAM (unit : 64 bits)
#else
        ((DWORD *)pData)[0] = ((DWORD)usHorSize * usVerSize * 3 * 8 % 64) ?
                              (((DWORD)usHorSize * usVerSize * 3 * 8 / 64) + 1) :
                              ((DWORD)usHorSize * usVerSize * 3 * 8 / 64);// One Frame total pixel for SDRAM (unit : 64 bits)
#endif // End of #if(_FRC_MODE == _LINE_MODE)
    }

    ulValue = CMemoryCalculateNumberAndRemain(((DWORD *)pData)[0], Length, 0);
    ulStep = CMemoryCalculateLineStepAndBlockStep(usHorSize, usVerSize);

    pData[0] = HIBYTE(stDisplayInfo.DVStartPos - 3);
    pData[1] = LOBYTE(stDisplayInfo.DVStartPos - 3);
    pData[2] = (HIBYTE(usHorSize) & 0x0F);
    pData[3] = LOBYTE(usHorSize);
    pData[4] = Length; //WTLVL
    pData[5] = 0x00; //Reserved
    pData[6] = HIBYTE(ulValue);
    pData[7] = LOBYTE(ulValue);
    pData[8] = Length;
    pData[9] = (BYTE)(ulValue >> 16);
    pData[10] = (BYTE)(_FRC_1ST_BLOCK_STA_ADDR >> 16);
    pData[11] = (BYTE)(_FRC_1ST_BLOCK_STA_ADDR >> 8);
    pData[12] = (BYTE)(_FRC_1ST_BLOCK_STA_ADDR >> 0);
    pData[13] = (BYTE)(ulStep >> 24); //LINE_STEP
    pData[14] = (BYTE)(ulStep >> 16); //LINE_STEP

    CScalerPageSelect(_PAGE5);
    CScalerWrite(_P5_SDRF_MN_PRERD_VST_H_BC, 15, pData, _AUTOINC);

#if(_FIELD_MERGE_SUPPORT == _ON)

    if(GET_FIELDMERGE_MODE())
    {
        pData[0] = (BYTE)(_FRC_2ND_BLOCK_STA_ADDR >> 20);//BLOCK_STEP
        pData[1] = (BYTE)(_FRC_2ND_BLOCK_STA_ADDR >> 12);//BLOCK_STEP
    }
    else
    {
        pData[0] = (BYTE)(_FRC_1ST_BLOCK_STA_ADDR >> 19);//BLOCK_STEP
        pData[1] = (BYTE)(_FRC_1ST_BLOCK_STA_ADDR >> 11);//BLOCK_STEP
    }

#else
    pData[0] = (BYTE)(_FRC_1ST_BLOCK_STA_ADDR >> 19);//BLOCK_STEP
    pData[1] = (BYTE)(_FRC_1ST_BLOCK_STA_ADDR >> 11);//BLOCK_STEP
#endif // End of #if(_FIELD_MERGE_SUPPORT == _ON)

#if(_FRC_MODE == _LINE_MODE)
    pData[2] = ((usVerSize >> 8) & 0x0f);//LINE_NUM
#else
   // pData[2] = (((usVerSize >> 8) & 0x0f) | 0x10);//LINE_NUM
    pData[2] = ((((usVerSize*2) >> 8) & 0x0f) | 0x10);//LINE_NUM
#endif // End of (_FRC_MODE == _LINE_MODE)

    pData[3] = LOBYTE((usVerSize*2));//LINE_NUM

#if(_FIELD_MERGE_SUPPORT == _ON)
    if(GET_FIELDMERGE_MODE())
    {
        pData[4] = 0x69;//DISP_CTRL
    }
    else
    {
#if(_FRC_YUV_MODE_SUPPORT == _ON)

        if(GET_FRC16BITSIN())//16 bits Input Format
        {
            pData[4] = 0xC9;//DISP_CTRL
            CLR_FRC16BITSIN();
        }
        else
#endif // End of #if(_FRC_YUV_MODE_SUPPORT == _ON)
        {

            if(GET_FRCDOUBLEFRAME())
            {
                pData[4] = 0x49;//DISP_CTRL Double Buffer
            }
            else
            {
                pData[4] = 0x09;//DISP_CTRL Single Buffer
            }
        }
    } // End of if(GET_FIELDMERGE_MODE())

#else // #if(_FIELD_MERGE_SUPPORT != _ON)

#if(_FRC_YUV_MODE_SUPPORT == _ON)
    if(GET_FRC16BITSIN())//16 bits Input Format
    {
        pData[4] = 0xC9;//DISP_CTRL
        CLR_FRC16BITSIN();
    }
    else
#endif // End of #if(_FRC_YUV_MODE_SUPPORT == _ON)
    {

        if(GET_FRCDOUBLEFRAME())
        {
            pData[4] = 0x49;//DISP_CTRL Double Buffer
        }
        else
        {
            pData[4] = 0x09;//DISP_CTRL Single Buffer
        }

    }
#endif // End of #if(_FIELD_MERGE_SUPPORT == _ON)

    pData[5] = 0x10;
    CScalerWrite(_P5_SDRF_MN_BLOCK_STEP_H_CB, 6, pData, _AUTOINC);
}

//--------------------------------------------------
// Description  : Adjust IVS2DVS Delay for Aspect Ratio Function
// Input Value  : ulFreq --> DCLK
// Output Value : IVS2DVS Delay Lines
//--------------------------------------------------
BYTE CMemoryAdjustIVS2DVSDelay(DWORD ulFreq)
{
    CScalerSetByte(_DISP_ACCESS_PORT_2A, (0x80 | _DISP_DV_ACT_END_H_12));
    CScalerRead(_DISP_DATA_PORT_2B, 2, &pData[4], _NON_AUTOINC); //Read DV Active End

    ((WORD *)pData)[4] = (WORD)pData[4] << 8 | pData[5];

    ((DWORD *)pData)[0] = ((DWORD)stDisplayInfo.DHTotal * ((WORD *)pData)[4]) + _PANEL_DH_START;
    ((DWORD *)pData)[0] = ((DWORD *)pData)[0] * ulFreq / (stModeInfo.IHTotal * stModeInfo.IHFreq * 100);

    ((DWORD *)pData)[1] = ((DWORD)stModeInfo.IHTotal * (stModeInfo.IVStartPos + stModeInfo.IVHeight - (g_ucVStartBias + _PROGRAM_VDELAY))) + stModeInfo.IHStartPos - (g_usHStartBias + _PROGRAM_HDELAY);

    ((DWORD *)pData)[1] += stModeInfo.IHTotal * 2;

    if (((DWORD *)pData)[0] > ((DWORD *)pData)[1])
    {
        pData[14] = 0;
        pData[15] = 0;

        CScalerSetBit(_FS_DELAY_FINE_TUNING_43, ~_BIT1, 0x00);
        CScalerSetByte(_IVS2DVS_DELAY_LINES_40, pData[14]);
        CScalerSetByte(_IV_DV_DELAY_CLK_ODD_41, pData[15]);

        return pData[14];
    }

    ((DWORD *)pData)[0] = ((DWORD *)pData)[1] - ((DWORD *)pData)[0];

    pData[14] = ((DWORD *)pData)[0] / stModeInfo.IHTotal;
    pData[15] = (((DWORD *)pData)[0] - ((DWORD)stModeInfo.IHTotal * pData[14])) / 16;

    if(pData[14] >= 0xFF)
    {
        pData[14] = 0xFE;
    }

    if(pData[15] >= 0xFF)
    {
        pData[15] = 0xFE;
    }
    CScalerSetBit(_FS_DELAY_FINE_TUNING_43, ~_BIT1, 0x00);
    CScalerSetByte(_IVS2DVS_DELAY_LINES_40, pData[14]);
    CScalerSetByte(_IV_DV_DELAY_CLK_ODD_41, pData[15]);

    return pData[14];
}

//--------------------------------------------------
// Description  : Transfer the Memory Size to Address
// Input Value  : ulTotalSize --> Total Memory size
// Output Value : Line Step Address or Block Step Address
//--------------------------------------------------
DWORD CMemoryAddressConversion(DWORD ulTotalSize)
{
    DWORD ulSize = 0; //Total Memory Soze

#if(_MEMORY_CONFIG >= _2M_32BIT_1PCE)
    ulSize = ((ulTotalSize & 0xffff00) << 1) | LOBYTE(ulTotalSize);
#else
    ulSize = ((ulTotalSize & 0x000100) << 1) | ((ulTotalSize & 0xfffe00) << 2) | LOBYTE(ulTotalSize);
#endif

    return ulSize;
}

//--------------------------------------------------
// Description  : Calculate the Line Step and Block Step
// Input Value  : usHorSize --> Horizontal size
//                usVerSize --> Vertical size
// Output Value : Line Step and Block Step
//--------------------------------------------------
DWORD CMemoryCalculateLineStepAndBlockStep(WORD usHorSize, WORD usVerSize)
{
    WORD usLine = 0; //Line Length
    DWORD ulBlock = 0; //Block Size

#if(_FRC_YUV_MODE_SUPPORT == _ON)

    if(GET_FRC422STATUS())
    {
        usLine = usHorSize * 16 / 64;
    }
    else
#endif
    {
        usLine = usHorSize * 24 / 64;
    }

    if(usLine & 0x03)
    {
        (usLine += 4 - (usLine & 0x03));
    }

    usLine = usLine * 64 / _MEMORY_BUS_WIDTH;
    ulBlock = (DWORD)usVerSize * usLine;
    usLine = CMemoryAddressConversion(usLine);
    ulBlock = (CMemoryAddressConversion(ulBlock) >> 11);

    return (((DWORD)usLine << 16) | (ulBlock & 0xffff));
}


//--------------------------------------------------
// Description  : Decide the Working Type
// Input Value  : None
// Output Value : None
//--------------------------------------------------
void CMemoryWorkingTypeDecide(void)
{

#if((_MEMORY_WORK_TYPE == _OD_ON_FRC_ON) || (_MEMORY_WORK_TYPE == _OD_OFF_FRC_ON))

#if(_FIELD_MERGE_SUPPORT == _ON)
    if((stModeInfo.IVFreq > _PANEL_MAX_FRAME_RATE) || GET_INTERLACE_MODE())
#else
    if(stModeInfo.IVFreq > _PANEL_MAX_FRAME_RATE)
#endif
    {
        if(CMemorySDRAMConfirm(_FRC_CONFIRM))
        {
            SET_FRCSTATUS();
        }
        else
        {
            ucCurrState = _NOSUPPORT_STATE;
 #if _DEBUG_MESSAGE_SUPPORT
				CDebugMessage("_NOSUPPORT_STATE7", 0);
#endif
        }

#if(_FRC_YUV_MODE_SUPPORT == _ON)
        if(GET_FRC422STATUS()) //Need FRC 422->444
        {
            CScalerPageSelect(_PAGE5);
            CScalerSetByte(_P5_SDRF_MN_FIFO_422_SET_E3, 0x02); //Enable FRC 422->444
        }
#endif //End of #if(_FRC_YUV_MODE_SUPPORT == _ON)

#if(_FIELD_MERGE_SUPPORT == _ON)
        {
            DWORD Imagesize = 0;

            if(GET_INTERLACE_MODE() && (ucCurrState != _NOSUPPORT_STATE))
            {
                if(GET_FRC422STATUS())
                {
                    Imagesize = (DWORD)stModeInfo.IHWidth * stModeInfo.IVHeight * 2 * 8 * 4 / 1000000;
                }
                else
                {
                    Imagesize = (DWORD)stModeInfo.IHWidth * stModeInfo.IVHeight * 3 * 8 * 4 / 1000000;
                }

                if(_MEMORY_SIZE > Imagesize)
                {
                    SET_FIELDMERGE_MODE();
                }
                else
                {
                    CLR_FIELDMERGE_MODE();
                }

            }
            else
            {
                CLR_FIELDMERGE_MODE();
            }
        } // End of if(GET_FRC422STATUS())
#endif
    }
    else // if(stModeInfo.IVFreq <= _PANEL_MAX_FRAME_RATE)
    {
        CLR_FRCSTATUS();

        if (stDisplayInfo.DVHeight < _PANEL_DV_HEIGHT)
        {

            if(CMemorySDRAMConfirm(_FRC_CONFIRM))
            {
                SET_FRCSTATUS();
            }

        }
    }
#else // #if((_MEMORY_WORK_TYPE != _OD_ON_FRC_ON) && (_MEMORY_WORK_TYPE != _OD_OFF_FRC_ON))

    CLR_FRCSTATUS();

#endif // End of #if((_MEMORY_WORK_TYPE == _OD_ON_FRC_ON) || (_MEMORY_WORK_TYPE == _OD_OFF_FRC_ON))

    // Calculate and set display clock frequency for frame sync mode
    ((DWORD *)pData)[0] = (DWORD)(stDisplayInfo.DHTotal) * (DWORD)stModeInfo.IHFreq * (DWORD)(stDisplayInfo.DVHeight) / stModeInfo.IVHeight / 10;

    if(!GET_FRCSTATUS() && (((DWORD *)pData)[0] >= (DWORD)_PANEL_PIXEL_CLOCK_MAX * 1000))
    {
        //ucCurrState = _NOSUPPORT_STATE;
        if(CMemorySDRAMConfirm(_FRC_CONFIRM))
        {
            SET_FRCSTATUS();
        }
        else
        {
            ucCurrState = _NOSUPPORT_STATE;
 #if _DEBUG_MESSAGE_SUPPORT
				CDebugMessage("_NOSUPPORT_STATE6", 0);
#endif
        }
    }

}
#endif //End of #if(_FRC_SUPPORT == _ON)


#endif  // End of #if(_SCALER_SERIES_TYPE == _RTD2472D_SERIES)

