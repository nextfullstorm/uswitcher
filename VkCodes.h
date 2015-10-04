#pragma once

static const wchar_t* vkcodes[] = 
{
	L"none",		//							0x00
	L"LButton",		//#define VK_LBUTTON        0x01
	L"RButton",		//#define VK_RBUTTON        0x02
	L"Cancel",		//#define VK_CANCEL         0x03
	L"MButton",		//#define VK_MBUTTON        0x04    /* NOT contiguous with L & RBUTTON */
	L"XButton1",	//#define VK_XBUTTON1       0x05    /* NOT contiguous with L & RBUTTON */
	L"XButton2",	//#define VK_XBUTTON2       0x06    /* NOT contiguous with L & RBUTTON */
	L"none",		//							0x07 : unassigned
	L"Back",		//#define VK_BACK           0x08
	L"Tab",			//#define VK_TAB            0x09
	L"none",		//							0x0A - 0x0B : reserved
	L"none",		//		
	L"Clear",		//#define VK_CLEAR          0x0C
	L"Enter",		//#define VK_RETURN         0x0D
	L"none",		//							0x0E
	L"none",		//							0x0F
	L"Shift",		//#define VK_SHIFT          0x10
	L"Control",		//#define VK_CONTROL        0x11
	L"Menu",		//#define VK_MENU           0x12
	L"Pause",		//#define VK_PAUSE          0x13
	L"Caps",		//#define VK_CAPITAL        0x14
	L"Kana",		//#define VK_KANA           0x15
	L"none",		//							0x16
	L"Junja",		//#define VK_JUNJA          0x17
	L"Final",		//#define VK_FINAL          0x18
	L"Hanja",		//#define VK_HANJA          0x19
	L"none",		//							0x1A
	L"Escape",		//#define VK_ESCAPE         0x1B
	L"none",		//#define VK_CONVERT        0x1C
	L"none",		//#define VK_NONCONVERT     0x1D
	L"none",		//#define VK_ACCEPT			0x1E
	L"none",		//#define VK_MODECHANGE		0x1F
	L"Space",		//#define VK_SPACE          0x20
	L"none",		//#define VK_PRIOR          0x21
	L"none",		//#define VK_NEXT           0x22
	L"End",			//#define VK_END            0x23
	L"Home",		//#define VK_HOME           0x24
	L"Left",		//#define VK_LEFT           0x25
	L"Up",			//#define VK_UP             0x26
	L"Right",		//#define VK_RIGHT          0x27
	L"Down",		//#define VK_DOWN           0x28
	L"Select",		//#define VK_SELECT         0x29
	L"Print",		//#define VK_PRINT          0x2A
	L"none",		//#define VK_EXECUTE        0x2B
	L"none",		//#define VK_SNAPSHOT       0x2C
	L"Insert",		//#define VK_INSERT         0x2D
	L"Delete",		//#define VK_DELETE         0x2E
	L"Help",		//#define VK_HELP           0x2F
	L"0",			//							0x30
	L"1",			//							0x31
	L"2",			//							0x32
	L"3",			//							0x33
	L"4",			//							0x34
	L"5",			//							0x35
	L"6",			//							0x36
	L"7",			//							0x37
	L"8",			//							0x38
	L"9",			//							0x39
	L"none",		//							0x3A
	L"none",		//							0x3B
	L"none",		//							0x3C
	L"none",		//							0x3D
	L"none",		//							0x3E
	L"none",		//							0x3F
	L"none",		//							0x40
	L"A",			//							0x41
	L"B",			//							0x42
	L"C",			//							0x43
	L"D",			//							0x44
	L"E",			//							0x45
	L"F",			//							0x46
	L"G",			//							0x47
	L"H",			//							0x48
	L"I",			//							0x49
	L"J",			//							0x4A
	L"K",			//							0x4B
	L"L",			//							0x4C
	L"M",			//							0x4D
	L"N",			//							0x4E
	L"O",			//							0x4F
	L"P",			//							0x50
	L"Q",			//							0x51
	L"R",			//							0x52
	L"S",			//							0x53
	L"T",			//							0x54
	L"U",			//							0x55
	L"V",			//							0x56
	L"W",			//							0x57
	L"X",			//							0x58
	L"Y",			//							0x59
	L"Z",			//							0x5A
	L"LWin",		//#define VK_LWIN           0x5B
	L"RWin",		//#define VK_RWIN           0x5C
	L"Menu",		//#define VK_APPS           0x5D
	L"none",		//							0x5E
	L"Sleep",		//#define VK_SLEEP          0x5F
	L"Num 0",		//#define VK_NUMPAD0        0x60
	L"Num 1",		//#define VK_NUMPAD0        0x61
	L"Num 2",		//#define VK_NUMPAD0        0x62
	L"Num 3",		//#define VK_NUMPAD0        0x63
	L"Num 4",		//#define VK_NUMPAD0        0x64
	L"Num 5",		//#define VK_NUMPAD0        0x65
	L"Num 6",		//#define VK_NUMPAD0        0x66
	L"Num 7",		//#define VK_NUMPAD0        0x67
	L"Num 8",		//#define VK_NUMPAD0        0x68
	L"Num 9",		//#define VK_NUMPAD0        0x69
	L"Num *",		//#define VK_MULTIPLY       0x6A
	L"Num +",		//#define VK_ADD            0x6B
	L"Separator",	//#define VK_SEPARATOR      0x6C
	L"Substract",	//#define VK_SUBTRACT       0x6D
	L"Decimal",		//#define VK_DECIMAL        0x6E
	L"Divide",		//#define VK_DIVIDE         0x6F
	L"F1",			//#define VK_F1             0x70
	L"F2",			//#define VK_F2             0x71
	L"F3",			//#define VK_F3             0x72
	L"F4",			//#define VK_F4             0x73
	L"F5",			//#define VK_F5             0x74
	L"F6",			//#define VK_F6             0x75
	L"F7",			//#define VK_F7             0x76
	L"F8",			//#define VK_F8             0x77
	L"F9",			//#define VK_F9             0x78
	L"F10",			//#define VK_F10            0x79
	L"F11",			//#define VK_F11            0x7A
	L"F12",			//#define VK_F12            0x7B
	L"F13",			//#define VK_F13            0x7C
	L"F14",			//#define VK_F14            0x7D
	L"F15",			//#define VK_F15            0x7E
	L"F16",			//#define VK_F16            0x7F
	L"F17",			//#define VK_F17            0x80
	L"F18",			//#define VK_F18            0x81
	L"F19",			//#define VK_F19            0x82
	L"F20",			//#define VK_F20            0x83
	L"F21",			//#define VK_F21            0x84
	L"F22",			//#define VK_F22            0x85
	L"F23",			//#define VK_F23            0x86
	L"F24",			//#define VK_F24            0x87
	L"none",		//							0x88
	L"none",		//							0x89
	L"none",		//							0x8A
	L"none",		//							0x8B
	L"none",		//							0x8C
	L"none",		//							0x8D
	L"none",		//							0x8E
	L"none",		//							0x8F
	L"Num Lock",	//#define VK_NUMLOCK        0x90
	L"Scroll Lock",	//#define VK_SCROLL         0x91
	L"none",		//							0x92
	L"none",		//							0x93
	L"none",		//							0x94
	L"none",		//							0x95
	L"none",		//							0x96
	L"none",		//							0x97
	L"none",		//							0x98
	L"none",		//							0x99
	L"none",		//							0x9A
	L"none",		//							0x9B
	L"none",		//							0x9C
	L"none",		//							0x9D
	L"none",		//							0x9E
	L"none",		//							0x9F
	L"LShift",		//#define VK_LSHIFT         0xA0
	L"RShift",		//#define VK_RSHIFT         0xA1
	L"LControl",	//#define VK_LCONTROL       0xA2
	L"RControl",	//#define VK_RCONTROL       0xA3
	L"LAlt",		//#define VK_LMENU          0xA4
	L"RAlt",		//#define VK_RMENU          0xA5
	L"Browser back",		//#define VK_BROWSER_BACK		0xA6
	L"Browser forward",		//#define VK_BROWSER_FORWARD	0xA7
	L"Browser refresh",		//#define VK_BROWSER_REFRESH	0xA8
	L"Browser stop",		//#define VK_BROWSER_STOP		0xA9
	L"Browser search",		//#define VK_BROWSER_SEARCH		0xAA
	L"Browser favorites",	//#define VK_BROWSER_FAVORITES	0xAB
	L"Browser home",		//#define VK_BROWSER_HOME		0xAC

	L"Volume mute",			//#define VK_VOLUME_MUTE		0xAD
	L"Volume down",			//#define VK_VOLUME_DOWN		0xAE
	L"Volume up",			//#define VK_VOLUME_UP			0xAF
	L"Media next track",	//#define VK_MEDIA_NEXT_TRACK	0xB0
	L"Media prev track",	//#define VK_MEDIA_PREV_TRACK	0xB1
	L"Media stop",			//#define VK_MEDIA_STOP			0xB2
	L"Media play pause",	//#define VK_MEDIA_PLAY_PAUSE	0xB3
	L"Launch mail",			//#define VK_LAUNCH_MAIL		0xB4
	L"Launch media select",	//#define VK_LAUNCH_MEDIA_SELECT 0xB5
	L"Launch app1",			//#define VK_LAUNCH_APP1		0xB6
	L"Launch app2",			//#define VK_LAUNCH_APP2		0xB7

	L"none",				//								0xB8
	L"none",				//								0xB9

	L":;",		// #define VK_OEM_1          0xBA   // ';:' for US
	L"+",		// #define VK_OEM_PLUS       0xBB   // '+' any country
	L",",		// #define VK_OEM_COMMA      0xBC   // ',' any country
	L"-",		// #define VK_OEM_MINUS      0xBD   // '-' any country
	L".",		// #define VK_OEM_PERIOD     0xBE   // '.' any country
	L"/?",		// #define VK_OEM_2          0xBF   // '/?' for US
	L"~",		// #define VK_OEM_3          0xC0   // '`~' for US

	L"none",				//								0xC1
	L"none",				//								0xC2
	L"none",				//								0xC3
	L"none",				//								0xC4
	L"none",				//								0xC5
	L"none",				//								0xC6
	L"none",				//								0xC7
	L"none",				//								0xC8
	L"none",				//								0xC9
	L"none",				//								0xCA
	L"none",				//								0xCB
	L"none",				//								0xCC
	L"none",				//								0xCD
	L"none",				//								0xCE
	L"none",				//								0xCF
	L"none",				//								0xD0
	L"none",				//								0xD1
	L"none",				//								0xD2
	L"none",				//								0xD3
	L"none",				//								0xD4
	L"none",				//								0xD5
	L"none",				//								0xD6
	L"none",				//								0xD7

	L"none",				//								0xD8
	L"none",				//								0xD9
	L"none",				//								0xDA

	L"[{",			// #define VK_OEM_4			 0xDB  //  '[{' for US
	L"\|",			// #define VK_OEM_5          0xDC  //  '\|' for US
	L"]}",			// #define VK_OEM_6          0xDD  //  ']}' for US
	L"\'\"",		// #define VK_OEM_7          0xDE  //  ''"' for US
	L"none",		// #define VK_OEM_8          0xDF
	L"none"			//					         0xE0
};


// /*
// * Various extended or enhanced keyboards
// */
// #define VK_OEM_AX         0xE1  //  'AX' key on Japanese AX kbd
// #define VK_OEM_102        0xE2  //  "<>" or "\|" on RT 102-key kbd.
// #define VK_ICO_HELP       0xE3  //  Help key on ICO
// #define VK_ICO_00         0xE4  //  00 key on ICO
// 
// #if(WINVER >= 0x0400)
// #define VK_PROCESSKEY     0xE5
// #endif /* WINVER >= 0x0400 */
// 
// #define VK_ICO_CLEAR      0xE6
// 
// 
// #if(_WIN32_WINNT >= 0x0500)
// #define VK_PACKET         0xE7
// #endif /* _WIN32_WINNT >= 0x0500 */
// 
// /*
// * 0xE8 : unassigned
// */
// 
// /*
// * Nokia/Ericsson definitions
// */
// #define VK_OEM_RESET      0xE9
// #define VK_OEM_JUMP       0xEA
// #define VK_OEM_PA1        0xEB
// #define VK_OEM_PA2        0xEC
// #define VK_OEM_PA3        0xED
// #define VK_OEM_WSCTRL     0xEE
// #define VK_OEM_CUSEL      0xEF
// #define VK_OEM_ATTN       0xF0
// #define VK_OEM_FINISH     0xF1
// #define VK_OEM_COPY       0xF2
// #define VK_OEM_AUTO       0xF3
// #define VK_OEM_ENLW       0xF4
// #define VK_OEM_BACKTAB    0xF5
// 
// #define VK_ATTN           0xF6
// #define VK_CRSEL          0xF7
// #define VK_EXSEL          0xF8
// #define VK_EREOF          0xF9
// #define VK_PLAY           0xFA
// #define VK_ZOOM           0xFB
// #define VK_NONAME         0xFC
// #define VK_PA1            0xFD
// #define VK_OEM_CLEAR      0xFE
// 
// */