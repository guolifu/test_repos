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
// ID Code      : OsdTable.h No.0000
// Update Note  :
//----------------------------------------------------------------------------------------------------

extern code void (*OperationTable[][4])(void);

//--------------------------------------------------
// RGB Settings for Colors
//--------------------------------------------------
#define _RGB_BLACK                                0,  0,  0
#define _RGB_WHITE                              255,255,255
#define _RGB_RED                                255,  0,  0
#define _RGB_GREEN                                0,255,  0
#define _RGB_BLUE                                 0,  0,255
#define _RGB_YELLOW                             255,255,  0
#define _RGB_GRAY                               128,128,128
#define _RGB_DARKBLUE                            64, 64,255
#define _RGB_LIGHTBLUE                          128,128,255
#define _RGB_SELECTBOTTOM                        58, 70, 80
#define _RGB_PINK                               255,128,255


#ifdef __ADJUST__

//--------------------------------------------------
// Color Palette Table
//--------------------------------------------------
BYTE code tPALETTE_0[] =
{
    _RGB_BLACK,
    _RGB_WHITE,
    _RGB_RED,
    _RGB_GREEN,
    _RGB_BLUE,
    _RGB_YELLOW,
    _RGB_GRAY,
    _RGB_DARKBLUE,
    _RGB_LIGHTBLUE,

    _RGB_SELECTBOTTOM,
    _RGB_PINK,

    _RGB_RED,
    _RGB_GREEN,
    _RGB_BLUE,
    _RGB_YELLOW,
    _RGB_GRAY,
};

//----------------------------------------------------------------------------------------------------
// Filter Coefficient Table (Sharpness)
//----------------------------------------------------------------------------------------------------
#if(_EXTEND_TABLE_SUPPORT == _ON)

BYTE xdata tSU_COEF_0[_SU_COEF_EXTEND]                  _at_ _SU_COEF_0_EXTEND_ADDRESS;
BYTE xdata tSU_COEF_1[_SU_COEF_EXTEND]                  _at_ _SU_COEF_1_EXTEND_ADDRESS;
BYTE xdata tSU_COEF_2[_SU_COEF_EXTEND]                  _at_ _SU_COEF_2_EXTEND_ADDRESS;
BYTE xdata tSU_COEF_3[_SU_COEF_EXTEND]                  _at_ _SU_COEF_3_EXTEND_ADDRESS;
BYTE xdata tSU_COEF_4[_SU_COEF_EXTEND]                  _at_ _SU_COEF_4_EXTEND_ADDRESS;

#else

BYTE code tSU_COEF_0[] =
{
    0x0B,0x00,0x0B,0x00,0x0A,0x00,0x08,0x00,0x06,0x00,0x05,0x00,0x02,0x00,0xFF,0x0F,
    0xFA,0x0F,0xF5,0x0F,0xF0,0x0F,0xE9,0x0F,0xE2,0x0F,0xDA,0x0F,0xD2,0x0F,0xC8,0x0F,
    0x74,0x0F,0x87,0x0F,0x9B,0x0F,0xB8,0x0F,0xD5,0x0F,0xFA,0x0F,0x1F,0x00,0x4C,0x00,
    0x7A,0x00,0xAF,0x00,0xE3,0x00,0x1F,0x01,0x59,0x01,0x9A,0x01,0xD8,0x01,0x1D,0x02,
    0x18,0x05,0x0E,0x05,0xFF,0x04,0xE6,0x04,0xCA,0x04,0xA3,0x04,0x7C,0x04,0x4B,0x04,
    0x19,0x04,0xDF,0x03,0xA6,0x03,0x66,0x03,0x27,0x03,0xE3,0x02,0xA2,0x02,0x5D,0x02,
    0x69,0x0F,0x60,0x0F,0x5C,0x0F,0x5A,0x0F,0x5B,0x0F,0x5E,0x0F,0x63,0x0F,0x6A,0x0F,
    0x73,0x0F,0x7D,0x0F,0x87,0x0F,0x92,0x0F,0x9E,0x0F,0xA9,0x0F,0xB4,0x0F,0xBE,0x0F,
};

BYTE code tSU_COEF_1[] =
{
    0x02,0x00,0x04,0x00,0x03,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0xFD,0x0F,0xFA,0x0F,
    0xF6,0x0F,0xF1,0x0F,0xED,0x0F,0xE7,0x0F,0xE0,0x0F,0xDA,0x0F,0xD3,0x0F,0xCB,0x0F,
    0x9E,0x0F,0xB1,0x0F,0xC6,0x0F,0xE2,0x0F,0xFF,0x0F,0x21,0x00,0x45,0x00,0x6F,0x00,
    0x9B,0x00,0xCC,0x00,0xFC,0x00,0x34,0x01,0x69,0x01,0xA5,0x01,0xDD,0x01,0x1C,0x02,
    0xCE,0x04,0xC3,0x04,0xB5,0x04,0x9E,0x04,0x85,0x04,0x63,0x04,0x3F,0x04,0x14,0x04,
    0xE6,0x03,0xB3,0x03,0x7F,0x03,0x45,0x03,0x0E,0x03,0xCF,0x02,0x95,0x02,0x56,0x02,
    0x92,0x0F,0x88,0x0F,0x82,0x0F,0x7E,0x0F,0x7C,0x0F,0x7C,0x0F,0x7F,0x0F,0x83,0x0F,
    0x89,0x0F,0x90,0x0F,0x98,0x0F,0xA0,0x0F,0xA9,0x0F,0xB2,0x0F,0xBB,0x0F,0xC3,0x0F,
    0x99,0x0F,0x9A,0x0F,0x9B,0x0F,0x9D,0x0F,0xA1,0x0F,0xA5,0x0F,0xAB,0x0F,0xB1,0x0F,
};

BYTE code tSU_COEF_2[] =
{
    0xFB,0x0F,0xFE,0x0F,0xFD,0x0F,0xFC,0x0F,0xFB,0x0F,0xFA,0x0F,0xF7,0x0F,0xF5,0x0F,
    0xF1,0x0F,0xED,0x0F,0xE9,0x0F,0xE4,0x0F,0xDE,0x0F,0xD9,0x0F,0xD3,0x0F,0xCD,0x0F,
    0xC7,0x0F,0xDB,0x0F,0xF0,0x0F,0x0C,0x00,0x27,0x00,0x49,0x00,0x6B,0x00,0x93,0x00,
    0xBC,0x00,0xEA,0x00,0x16,0x01,0x49,0x01,0x7A,0x01,0xB0,0x01,0xE3,0x01,0x1C,0x02,
    0x84,0x04,0x78,0x04,0x6C,0x04,0x57,0x04,0x42,0x04,0x23,0x04,0x04,0x04,0xDC,0x03,
    0xB4,0x03,0x86,0x03,0x59,0x03,0x26,0x03,0xF4,0x02,0xBD,0x02,0x89,0x02,0x50,0x02,
    0xBA,0x0F,0xAF,0x0F,0xA7,0x0F,0xA1,0x0F,0x9C,0x0F,0x9A,0x0F,0x9A,0x0F,0x9C,0x0F,
    0x9F,0x0F,0xA3,0x0F,0xA8,0x0F,0xAD,0x0F,0xB4,0x0F,0xBA,0x0F,0xC1,0x0F,0xC7,0x0F,
};

BYTE code tSU_COEF_3[] =
{
    0xF7,0x0F,0xFB,0x0F,0xFA,0x0F,0xF9,0x0F,0xF8,0x0F,0xF7,0x0F,0xF4,0x0F,0xF2,0x0F,
    0xEF,0x0F,0xEB,0x0F,0xE7,0x0F,0xE3,0x0F,0xDD,0x0F,0xD9,0x0F,0xD4,0x0F,0xCF,0x0F,
    0xDC,0x0F,0xF0,0x0F,0x05,0x00,0x21,0x00,0x3C,0x00,0x5D,0x00,0x7E,0x00,0xA5,0x00,
    0xCC,0x00,0xF8,0x00,0x23,0x01,0x54,0x01,0x82,0x01,0xB6,0x01,0xE6,0x01,0x1B,0x02,
    0x5F,0x04,0x52,0x04,0x47,0x04,0x33,0x04,0x1F,0x04,0x03,0x04,0xE6,0x03,0xC1,0x03,
    0x9B,0x03,0x71,0x03,0x46,0x03,0x15,0x03,0xE8,0x02,0xB3,0x02,0x82,0x02,0x4D,0x02,
    0xCE,0x0F,0xC3,0x0F,0xBA,0x0F,0xB3,0x0F,0xAD,0x0F,0xA9,0x0F,0xA8,0x0F,0xA8,0x0F,
    0xAA,0x0F,0xAC,0x0F,0xB0,0x0F,0xB4,0x0F,0xB9,0x0F,0xBE,0x0F,0xC4,0x0F,0xC9,0x0F,
};

BYTE code tSU_COEF_4[] =
{
    0xE0,0x0F,0xE9,0x0F,0xE8,0x0F,0xE9,0x0F,0xE8,0x0F,0xE7,0x0F,0xE4,0x0F,0xE3,0x0F,
    0xE1,0x0F,0xDF,0x0F,0xDC,0x0F,0xDB,0x0F,0xD8,0x0F,0xD7,0x0F,0xD5,0x0F,0xD5,0x0F,
    0x4C,0x00,0x61,0x00,0x78,0x00,0x92,0x00,0xAC,0x00,0xC9,0x00,0xE6,0x00,0x06,0x01,
    0x26,0x01,0x48,0x01,0x69,0x01,0x8D,0x01,0xAF,0x01,0xD4,0x01,0xF6,0x01,0x1B,0x02,
    0x9A,0x03,0x8B,0x03,0x84,0x03,0x75,0x03,0x68,0x03,0x56,0x03,0x44,0x03,0x2D,0x03,
    0x15,0x03,0xFA,0x02,0xE0,0x02,0xC0,0x02,0xA3,0x02,0x81,0x02,0x61,0x02,0x3C,0x02,
    0x3A,0x00,0x2B,0x00,0x1C,0x00,0x10,0x00,0x04,0x00,0xFA,0x0F,0xF2,0x0F,0xEA,0x0F,
    0xE4,0x0F,0xDF,0x0F,0xDB,0x0F,0xD8,0x0F,0xD6,0x0F,0xD4,0x0F,0xD4,0x0F,0xD4,0x0F,
};

#endif // End of #if(_EXTEND_TABLE_SUPPORT == _ON)

code BYTE *tSCALE_COEF_TABLE[] =
{
    tSU_COEF_0,     tSU_COEF_1,     tSU_COEF_2,     tSU_COEF_3,     tSU_COEF_4,
};

#else

extern BYTE code tPALETTE_0[];
extern code BYTE *tSCALE_COEF_TABLE[];

#endif // End of #ifdef __ADJUST__




#ifdef __NONE_OSD__

//--------------------------------------------------
// Text Width Table Used by COsdFxLoadText()
//--------------------------------------------------
BYTE code tOSD_CHARWIDTH_TABLE_GLOBAL[][2] =
{
    {_A_, (8 << 4) | 12},
    {_B_, (8 << 4) | 12},
    {_C_, (8 << 4) | 12},
    {_D_, (8 << 4) | 12},
    {_E_, (8 << 4) | 11},
    {_F_, (8 << 4) | 10},
    {_G_, (8 << 4) | 12},
    {_H_, (8 << 4) | 12},
    {_I_, (8 << 4) | 4},
    {_J_, (8 << 4) | 10},
    {_K_, (8 << 4) | 12},
    {_L_, (8 << 4) | 12},
    {_M_, (8 << 4) | 12},
    {_N_, (8 << 4) | 12},
    {_O_, (8 << 4) | 12},
    {_P_, (8 << 4) | 12},
    {_Q_, (8 << 4) | 12},
    {_R_, (8 << 4) | 12},
    {_S_, (8 << 4) | 10},
    {_T_, (8 << 4) | 12},
    {_U_, (8 << 4) | 12},
    {_V_, (8 << 4) | 12},
    {_W_, (8 << 4) | 12},
    {_X_, (8 << 4) | 12},
    {_Y_, (8 << 4) | 12},
    {_Z_, (8 << 4) | 9},

    {_a_, (8 << 4) | 10},
    {_b_, (8 << 4) | 10},
    {_c_, (8 << 4) | 9},
    {_d_, (8 << 4) | 10},
    {_e_, (8 << 4) | 9},
    {_f_, (8 << 4) | 8},
    {_g_, (8 << 4) | 10},
    {_h_, (8 << 4) | 10},
    {_i_, (8 << 4) | 4},
    {_j_, (8 << 4) | 5},
    {_k_, (8 << 4) | 9},
    {_l_, (8 << 4) | 4},
    {_m_, (8 << 4) | 12},
    {_n_, (8 << 4) | 10},
    {_o_, (8 << 4) | 10},
    {_p_, (8 << 4) | 10},
    {_q_, (8 << 4) | 10},
    {_r_, (8 << 4) | 7},
    {_s_, (8 << 4) | 9},
    {_t_, (8 << 4) | 7},
    {_u_, (8 << 4) | 10},
    {_v_, (8 << 4) | 9},
    {_w_, (8 << 4) | 12},
    {_x_, (8 << 4) | 9},
    {_y_, (8 << 4) | 11},
    {_z_, (8 << 4) | 9},

    {___, (8 << 4) | 12},
    {_0_, (8 << 4) | 10},
    {_1_, (8 << 4) | 10},
    {_2_, (8 << 4) | 10},
    {_3_, (8 << 4) | 10},
    {_4_, (8 << 4) | 10},
    {_5_, (8 << 4) | 10},
    {_6_, (8 << 4) | 10},
    {_7_, (8 << 4) | 10},
    {_8_, (8 << 4) | 10},
    {_9_, (8 << 4) | 10},

    {_END_, _END_},
};

BYTE code tOSD_CHARWIDTH_TABLE_0[][2] =
{
    {_END_, _END_},
};

BYTE code tOSD_CHARWIDTH_TABLE_1[][2] =
{
    {_END_, _END_},
};

BYTE code tOSD_CHARWIDTH_TABLE_2[][2] =
{
    {_END_, _END_},
};

BYTE code tOSD_CHARWIDTH_TABLE_3[][2] =
{
    {_END_, _END_},
};

BYTE code tOSD_CHARWIDTH_TABLE_4[][2] =
{
    {_END_, _END_},
};

BYTE code tOSD_CHARWIDTH_TABLE_5[][2] =
{
    {_END_, _END_},
};

BYTE code tOSD_CHARWIDTH_TABLE_6[][2] =
{
    {_END_, _END_},
};

BYTE code tOSD_CHARWIDTH_TABLE_7[][2] =
{
    {_END_, _END_},
};

code BYTE *tOSD_CHARWIDTH_TABLE[9] =
{
    tOSD_CHARWIDTH_TABLE_GLOBAL,
    tOSD_CHARWIDTH_TABLE_0, tOSD_CHARWIDTH_TABLE_1, tOSD_CHARWIDTH_TABLE_2, tOSD_CHARWIDTH_TABLE_3,
    tOSD_CHARWIDTH_TABLE_4, tOSD_CHARWIDTH_TABLE_5, tOSD_CHARWIDTH_TABLE_6, tOSD_CHARWIDTH_TABLE_7,
};

//--------------------------------------------------
// OSD Char Commands Map Information (Rows, Cols)
//--------------------------------------------------
WORD code tOSD_OSDMAP_INF[][2] =
{
    {15, 25},
};

//--------------------------------------------------
// OSD Row Command Table
//--------------------------------------------------
BYTE code tOSD_ROW_COMMAND0[] =
{
    FONTBYTE0(0x000),
    0x80, _REPEAT_, 15, _NEXT_,

    FONTBYTE1(0x000),
    0x88, _REPEAT_, 15, _NEXT_,

    FONTBYTE2(0x000),
    25, _REPEAT_, 15, _NEXT_,

    FONTBYTE0(15),
    0x00,

    _END_
};


//--------------------------------------------------
// Window Command Table Used by COsdFxDrawWindow()
//--------------------------------------------------
BYTE code tOSD_WINDOW0_STYLE[] =
{
    WINNO(0),

    0x00,                               // Byte 0 of 110h
    (_CP_BLACK << 4) | _CP_BLACK,       // Byte 1 of 110h
    0x00,                               // Byte 2 of 110h
    0x00,                               // Byte 0 of 113h
    0x00 | _CP_BLACK,                   // Byte 1 of 113h
    0x01                                // Byte 2 of 113h
};


//--------------------------------------------------
// Main Menu Text Table
//--------------------------------------------------
BYTE code tMain_Menu_TEXT[] =
{
    _M_, _e_, _n_, _u_, _END_,
};

code BYTE *tMain_Menu_TABLE[1] =
{
    tMain_Menu_TEXT,
};

#else

extern code BYTE *tOSD_CHARWIDTH_TABLE[9];
extern WORD code tOSD_OSDMAP_INF[][2];
extern BYTE code tOSD_ROW_COMMAND0[];
extern BYTE code tOSD_WINDOW0_STYLE[];
extern code BYTE *tMain_Menu_TABLE[1];

#endif


