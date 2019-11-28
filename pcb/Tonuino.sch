EESchema Schematic File Version 4
LIBS:Tonuino-cache
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L MCU_Module:Arduino_Nano_v2.x A1
U 1 1 5DBEB121
P 5500 3600
F 0 "A1" H 5500 2511 50  0000 C CNN
F 1 "Arduino_Nano_v2.x" H 5500 2420 50  0000 C CNN
F 2 "Module:Arduino_Nano" H 5650 2650 50  0001 L CNN
F 3 "https://www.arduino.cc/en/uploads/Main/ArduinoNanoManual23.pdf" H 5500 2600 50  0001 C CNN
	1    5500 3600
	0    -1   -1   0   
$EndComp
Wire Wire Line
	6500 3600 6500 3500
Wire Wire Line
	4500 3400 4350 3400
$Comp
L rur:DFPLAYER_MINI U1
U 1 1 5DBEB5A8
P 6650 5650
F 0 "U1" H 6650 6443 60  0000 C CNN
F 1 "DFPLAYER_MINI" H 6650 6337 60  0000 C CNN
F 2 "Arduino_Nano:DFPlayer" H 6650 6231 60  0000 C CNN
F 3 "" H 6650 5650 60  0000 C CNN
	1    6650 5650
	1    0    0    -1  
$EndComp
Wire Wire Line
	5100 5500 6050 5500
Wire Wire Line
	5100 4100 5100 5500
Wire Wire Line
	5700 5400 5600 5400
Wire Wire Line
	6000 5400 6050 5400
$Comp
L power:+5V #PWR0101
U 1 1 5DC01317
P 4950 2600
F 0 "#PWR0101" H 4950 2450 50  0001 C CNN
F 1 "+5V" H 4965 2773 50  0000 C CNN
F 2 "" H 4950 2600 50  0001 C CNN
F 3 "" H 4950 2600 50  0001 C CNN
	1    4950 2600
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR0102
U 1 1 5DC01BAD
P 5900 5100
F 0 "#PWR0102" H 5900 4950 50  0001 C CNN
F 1 "+5V" H 5915 5273 50  0000 C CNN
F 2 "" H 5900 5100 50  0001 C CNN
F 3 "" H 5900 5100 50  0001 C CNN
	1    5900 5100
	1    0    0    -1  
$EndComp
Wire Wire Line
	4950 2600 4950 2950
Wire Wire Line
	5900 5300 6050 5300
Wire Wire Line
	5900 5100 5900 5300
$Comp
L power:GND #PWR0103
U 1 1 5DC024A1
P 6500 3950
F 0 "#PWR0103" H 6500 3700 50  0001 C CNN
F 1 "GND" H 6505 3777 50  0000 C CNN
F 2 "" H 6500 3950 50  0001 C CNN
F 3 "" H 6500 3950 50  0001 C CNN
	1    6500 3950
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0104
U 1 1 5DC03171
P 7400 5900
F 0 "#PWR0104" H 7400 5650 50  0001 C CNN
F 1 "GND" V 7405 5772 50  0000 R CNN
F 2 "" H 7400 5900 50  0001 C CNN
F 3 "" H 7400 5900 50  0001 C CNN
	1    7400 5900
	0    -1   -1   0   
$EndComp
Wire Wire Line
	7400 5900 7250 5900
$Comp
L Connector:Conn_01x08_Female J1
U 1 1 5DC040CD
P 8650 4650
F 0 "J1" H 8542 4025 50  0000 C CNN
F 1 "ConRFID" H 8542 4116 50  0000 C CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_1x08_P2.54mm_Vertical" H 8650 4650 50  0001 C CNN
F 3 "~" H 8650 4650 50  0001 C CNN
	1    8650 4650
	1    0    0    1   
$EndComp
Wire Wire Line
	5300 4100 5300 4800
Wire Wire Line
	5300 4800 7500 4800
Wire Wire Line
	7500 4800 7500 5300
Wire Wire Line
	7500 5300 7250 5300
$Comp
L Connector:AudioJack3 J2
U 1 1 5DC071A7
P 4100 5700
F 0 "J2" H 4082 6025 50  0000 C CNN
F 1 "AudioJack3" H 4082 5934 50  0000 C CNN
F 2 "Connector_Audio:Jack_3.5mm_CUI_SJ1-3533NG_Horizontal" H 4100 5700 50  0001 C CNN
F 3 "~" H 4100 5700 50  0001 C CNN
	1    4100 5700
	1    0    0    1   
$EndComp
$Comp
L power:GND #PWR0105
U 1 1 5DC0B691
P 4500 5900
F 0 "#PWR0105" H 4500 5650 50  0001 C CNN
F 1 "GND" H 4505 5727 50  0000 C CNN
F 2 "" H 4500 5900 50  0001 C CNN
F 3 "" H 4500 5900 50  0001 C CNN
	1    4500 5900
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR0106
U 1 1 5DC10C7D
P 4150 3350
F 0 "#PWR0106" H 4150 3200 50  0001 C CNN
F 1 "+3.3V" H 4165 3523 50  0000 C CNN
F 2 "" H 4150 3350 50  0001 C CNN
F 3 "" H 4150 3350 50  0001 C CNN
	1    4150 3350
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR0107
U 1 1 5DC1120B
P 8050 5100
F 0 "#PWR0107" H 8050 4950 50  0001 C CNN
F 1 "+3.3V" H 8065 5273 50  0000 C CNN
F 2 "" H 8050 5100 50  0001 C CNN
F 3 "" H 8050 5100 50  0001 C CNN
	1    8050 5100
	0    -1   -1   0   
$EndComp
Wire Wire Line
	8450 4950 8200 4950
Wire Wire Line
	8200 5100 8050 5100
Wire Wire Line
	4500 3500 4150 3500
Wire Wire Line
	4150 3500 4150 3350
Wire Wire Line
	6500 3950 6500 3600
$Comp
L power:GND #PWR0108
U 1 1 5DC13CBF
P 8050 4850
F 0 "#PWR0108" H 8050 4600 50  0001 C CNN
F 1 "GND" H 8055 4677 50  0000 C CNN
F 2 "" H 8050 4850 50  0001 C CNN
F 3 "" H 8050 4850 50  0001 C CNN
	1    8050 4850
	0    1    1    0   
$EndComp
Wire Wire Line
	8450 4850 8050 4850
Wire Wire Line
	8450 4650 5800 4650
Wire Wire Line
	5800 4650 5800 4100
Wire Wire Line
	5900 4100 5900 4550
Wire Wire Line
	5900 4550 8450 4550
Wire Wire Line
	8450 4450 6000 4450
Wire Wire Line
	6000 4450 6000 4100
Wire Wire Line
	6100 4100 6100 4350
Wire Wire Line
	6100 4350 8450 4350
Wire Wire Line
	8450 4250 6200 4250
Wire Wire Line
	6200 4250 6200 4100
Text Label 5800 4650 0    50   ~ 0
DFP_RST
Text Label 5900 4550 0    50   ~ 0
DFP_SDA(SS)
Text Label 6000 4450 0    50   ~ 0
DFP_MOSI
Text Label 6100 4350 0    50   ~ 0
DFP_MISO
Text Label 6200 4250 0    50   ~ 0
DFP_SCK
$Comp
L Connector:Conn_01x08_Female J4
U 1 1 5DC300A7
P 5900 1900
F 0 "J4" V 6065 1830 50  0000 C CNN
F 1 "ConButtons" V 5974 1830 50  0000 C CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_1x08_P2.54mm_Vertical" H 5900 1900 50  0001 C CNN
F 3 "~" H 5900 1900 50  0001 C CNN
	1    5900 1900
	0    1    -1   0   
$EndComp
$Comp
L power:GND #PWR0109
U 1 1 5DC3220A
P 6100 2800
F 0 "#PWR0109" H 6100 2550 50  0001 C CNN
F 1 "GND" H 6105 2627 50  0000 C CNN
F 2 "" H 6100 2800 50  0001 C CNN
F 3 "" H 6100 2800 50  0001 C CNN
	1    6100 2800
	1    0    0    -1  
$EndComp
Wire Wire Line
	5500 2100 5500 3100
Wire Wire Line
	5600 3100 5600 2100
Wire Wire Line
	5700 2100 5700 3100
Wire Wire Line
	5800 3100 5800 2100
Wire Wire Line
	5900 2100 5900 3100
Wire Wire Line
	6000 3100 6000 2100
Text Label 5500 2600 1    50   ~ 0
Btn1
Text Label 5600 2600 1    50   ~ 0
Btn2
Text Label 5700 2600 1    50   ~ 0
Btn3
Text Label 5800 2600 1    50   ~ 0
Btn4
Text Label 5900 2600 1    50   ~ 0
Btn5
Text Label 6000 2600 1    50   ~ 0
Btn6
Text Label 6200 2650 1    50   ~ 0
BtnLigth_5V0
$Comp
L Connector:USB_A J3
U 1 1 5DC410F9
P 7150 1800
F 0 "J3" H 6920 1789 50  0000 R CNN
F 1 "USB_A" H 6920 1698 50  0000 R CNN
F 2 "Connector_USB:USB_A_Stewart_SS-52100-001_Horizontal" H 7300 1750 50  0001 C CNN
F 3 " ~" H 7300 1750 50  0001 C CNN
	1    7150 1800
	-1   0    0    -1  
$EndComp
$Comp
L power:GND #PWR0110
U 1 1 5DC42FEC
P 7150 2300
F 0 "#PWR0110" H 7150 2050 50  0001 C CNN
F 1 "GND" H 7155 2127 50  0000 C CNN
F 2 "" H 7150 2300 50  0001 C CNN
F 3 "" H 7150 2300 50  0001 C CNN
	1    7150 2300
	1    0    0    -1  
$EndComp
Wire Wire Line
	7250 2200 7150 2200
$Comp
L Transistor_FET:IRLML6402 Q2
U 1 1 5DC46A96
P 3150 1700
F 0 "Q2" V 3493 1700 50  0000 C CNN
F 1 "IRLML6402" V 3402 1700 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 3350 1625 50  0001 L CIN
F 3 "https://www.infineon.com/dgdl/irlml6402pbf.pdf?fileId=5546d462533600a401535668d5c2263c" H 3150 1700 50  0001 L CNN
	1    3150 1700
	0    1    -1   0   
$EndComp
Wire Wire Line
	2550 1600 2750 1600
$Comp
L Connector:Conn_01x04_Female J5
U 1 1 5DC5F801
P 1850 1700
F 0 "J5" H 1742 1985 50  0000 C CNN
F 1 "ConInput" H 1742 1894 50  0000 C CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_1x04_P2.54mm_Vertical" H 1850 1700 50  0001 C CNN
F 3 "~" H 1850 1700 50  0001 C CNN
	1    1850 1700
	-1   0    0    -1  
$EndComp
$Comp
L power:GND #PWR0111
U 1 1 5DC609F8
P 2150 1950
F 0 "#PWR0111" H 2150 1700 50  0001 C CNN
F 1 "GND" H 2155 1777 50  0000 C CNN
F 2 "" H 2150 1950 50  0001 C CNN
F 3 "" H 2150 1950 50  0001 C CNN
	1    2150 1950
	1    0    0    -1  
$EndComp
$Comp
L power:+BATT #PWR0112
U 1 1 5DC61238
P 2550 1600
F 0 "#PWR0112" H 2550 1450 50  0001 C CNN
F 1 "+BATT" H 2565 1773 50  0000 C CNN
F 2 "" H 2550 1600 50  0001 C CNN
F 3 "" H 2550 1600 50  0001 C CNN
	1    2550 1600
	1    0    0    -1  
$EndComp
Connection ~ 2550 1600
Wire Wire Line
	2050 1600 2550 1600
Wire Wire Line
	2050 1900 2150 1900
Wire Wire Line
	2150 1900 2150 1950
$Comp
L Device:R R2
U 1 1 5DC68E28
P 2750 1850
F 0 "R2" H 2820 1896 50  0000 L CNN
F 1 "650k" H 2820 1805 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P15.24mm_Horizontal" V 2680 1850 50  0001 C CNN
F 3 "~" H 2750 1850 50  0001 C CNN
	1    2750 1850
	1    0    0    -1  
$EndComp
Wire Wire Line
	2750 1700 2750 1600
Connection ~ 2750 1600
Wire Wire Line
	2750 1600 2950 1600
Wire Wire Line
	2750 2000 3150 2000
Wire Wire Line
	3150 2000 3150 1900
$Comp
L Transistor_FET:IRLML6402 Q3
U 1 1 5DC6FDBB
P 3150 2850
F 0 "Q3" V 3401 2850 50  0000 C CNN
F 1 "IRLML6402" V 3492 2850 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 3350 2775 50  0001 L CIN
F 3 "https://www.infineon.com/dgdl/irlml6402pbf.pdf?fileId=5546d462533600a401535668d5c2263c" H 3150 2850 50  0001 L CNN
	1    3150 2850
	0    1    1    0   
$EndComp
Wire Wire Line
	2950 2950 2550 2950
Wire Wire Line
	2550 2950 2550 1600
Wire Wire Line
	3150 2000 3150 2650
Connection ~ 3150 2000
Connection ~ 7150 2200
Wire Wire Line
	7150 2200 7150 2300
$Comp
L Transistor_FET:IRLML2060 Q4
U 1 1 5DC81F34
P 4250 2200
F 0 "Q4" H 4456 2246 50  0000 L CNN
F 1 "IRLML2060" H 4456 2155 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 4450 2125 50  0001 L CIN
F 3 "https://www.infineon.com/dgdl/irlml2060pbf.pdf?fileId=5546d462533600a401535664b7fb25ee" H 4250 2200 50  0001 L CNN
	1    4250 2200
	-1   0    0    -1  
$EndComp
Wire Wire Line
	4350 3400 4350 2950
Wire Wire Line
	4350 2950 4950 2950
$Comp
L power:GND #PWR0113
U 1 1 5DCC51C4
P 4150 2650
F 0 "#PWR0113" H 4150 2400 50  0001 C CNN
F 1 "GND" H 4155 2477 50  0000 C CNN
F 2 "" H 4150 2650 50  0001 C CNN
F 3 "" H 4150 2650 50  0001 C CNN
	1    4150 2650
	1    0    0    -1  
$EndComp
Wire Wire Line
	4150 2650 4150 2550
$Comp
L Device:R R3
U 1 1 5DCC70E7
P 4450 2400
F 0 "R3" H 4520 2446 50  0000 L CNN
F 1 "650k" H 4520 2355 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal" V 4380 2400 50  0001 C CNN
F 3 "~" H 4450 2400 50  0001 C CNN
	1    4450 2400
	1    0    0    -1  
$EndComp
Wire Wire Line
	4450 2250 4450 2200
Wire Wire Line
	4450 2550 4150 2550
Connection ~ 4150 2550
Wire Wire Line
	4150 2550 4150 2400
Wire Wire Line
	5600 4100 5600 4200
Text GLabel 5600 4200 3    50   Input ~ 0
PIN_PWR_ON
Text GLabel 4450 2150 1    50   Input ~ 0
PIN_PWR_ON
Wire Wire Line
	4450 2150 4450 2200
Connection ~ 4450 2200
$Comp
L Transistor_FET:IRLML6402 Q5
U 1 1 5DCD5991
P 6450 2750
F 0 "Q5" V 6793 2750 50  0000 C CNN
F 1 "IRLML6402" V 6702 2750 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 6650 2675 50  0001 L CIN
F 3 "https://www.infineon.com/dgdl/irlml6402pbf.pdf?fileId=5546d462533600a401535668d5c2263c" H 6450 2750 50  0001 L CNN
	1    6450 2750
	0    -1   -1   0   
$EndComp
$Comp
L power:+BATT #PWR0114
U 1 1 5DCD9E88
P 6700 2300
F 0 "#PWR0114" H 6700 2150 50  0001 C CNN
F 1 "+BATT" H 6715 2473 50  0000 C CNN
F 2 "" H 6700 2300 50  0001 C CNN
F 3 "" H 6700 2300 50  0001 C CNN
	1    6700 2300
	1    0    0    -1  
$EndComp
Wire Wire Line
	6700 2300 6700 2650
Wire Wire Line
	6700 2650 6650 2650
Wire Wire Line
	6200 2100 6200 2650
Wire Wire Line
	6200 2650 6250 2650
$Comp
L Transistor_FET:IRLML2060 Q1
U 1 1 5DCF5D06
P 8650 2450
F 0 "Q1" H 8856 2404 50  0000 L CNN
F 1 "IRLML2060" H 8856 2495 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 8850 2375 50  0001 L CIN
F 3 "https://www.infineon.com/dgdl/irlml2060pbf.pdf?fileId=5546d462533600a401535664b7fb25ee" H 8650 2450 50  0001 L CNN
	1    8650 2450
	-1   0    0    -1  
$EndComp
$Comp
L power:GND #PWR0115
U 1 1 5DCFD1E3
P 8550 2800
F 0 "#PWR0115" H 8550 2550 50  0001 C CNN
F 1 "GND" H 8555 2627 50  0000 C CNN
F 2 "" H 8550 2800 50  0001 C CNN
F 3 "" H 8550 2800 50  0001 C CNN
	1    8550 2800
	1    0    0    -1  
$EndComp
$Comp
L Device:R R4
U 1 1 5DCFD742
P 8550 2000
F 0 "R4" H 8620 2046 50  0000 L CNN
F 1 "33R" H 8620 1955 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 8480 2000 50  0001 C CNN
F 3 "~" H 8550 2000 50  0001 C CNN
	1    8550 2000
	1    0    0    -1  
$EndComp
Wire Wire Line
	8550 2150 8550 2200
$Comp
L Device:R R5
U 1 1 5DD026D7
P 8900 2650
F 0 "R5" H 8970 2696 50  0000 L CNN
F 1 "650k" H 8970 2605 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 8830 2650 50  0001 C CNN
F 3 "~" H 8900 2650 50  0001 C CNN
	1    8900 2650
	1    0    0    -1  
$EndComp
Wire Wire Line
	8550 2800 8900 2800
Wire Wire Line
	8550 2800 8550 2650
Connection ~ 8550 2800
Wire Wire Line
	8850 2450 8900 2450
Wire Wire Line
	8900 2450 8900 2500
Wire Wire Line
	8900 2450 9050 2450
Connection ~ 8900 2450
Text GLabel 9050 2450 2    50   Input ~ 0
PIN_ALIVE
Text GLabel 5400 4200 3    50   Input ~ 0
PIN_ALIVE
Wire Wire Line
	5400 4200 5400 4100
Connection ~ 6500 3600
Text GLabel 7500 2650 2    50   Input ~ 0
PIN_BTN_PWM
Wire Wire Line
	6450 3050 6450 2950
Wire Wire Line
	8200 5100 8200 4950
Wire Wire Line
	6100 2800 6100 2100
$Comp
L Connector:Conn_01x03_Female J6
U 1 1 5DC4BC0B
P 1650 2600
F 0 "J6" H 1542 2885 50  0000 C CNN
F 1 "Conn_01x03_Female" H 1542 2794 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x03_P2.54mm_Vertical" H 1650 2600 50  0001 C CNN
F 3 "~" H 1650 2600 50  0001 C CNN
	1    1650 2600
	-1   0    0    -1  
$EndComp
$Comp
L power:GND #PWR0117
U 1 1 5DC4C88D
P 2000 2600
F 0 "#PWR0117" H 2000 2350 50  0001 C CNN
F 1 "GND" H 2005 2427 50  0000 C CNN
F 2 "" H 2000 2600 50  0001 C CNN
F 3 "" H 2000 2600 50  0001 C CNN
	1    2000 2600
	0    -1   -1   0   
$EndComp
Wire Wire Line
	2000 2600 1850 2600
Text GLabel 2350 2400 1    50   Input ~ 0
SW_PWR_ON
Text GLabel 2350 2800 3    50   Input ~ 0
SW_ALIVE
Wire Wire Line
	1850 2500 2350 2500
Wire Wire Line
	2350 2500 2350 2400
Wire Wire Line
	1850 2700 2350 2700
Wire Wire Line
	2350 2700 2350 2800
Text GLabel 8400 2200 0    50   Input ~ 0
SW_ALIVE
Text GLabel 2750 2150 3    50   Input ~ 0
SW_PWR_ON
Wire Wire Line
	8400 2200 8550 2200
Connection ~ 8550 2200
Wire Wire Line
	8550 2200 8550 2250
Wire Wire Line
	2750 2150 2750 2000
Connection ~ 2750 2000
Text GLabel 5200 4200 3    50   Input ~ 0
PIN_BTN_PWM
Wire Wire Line
	5200 4200 5200 4100
$Comp
L Device:R R1
U 1 1 5DBF700B
P 5850 5400
F 0 "R1" V 6057 5400 50  0000 C CNN
F 1 "1k" V 5966 5400 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 5780 5400 50  0001 C CNN
F 3 "~" H 5850 5400 50  0001 C CNN
	1    5850 5400
	0    -1   -1   0   
$EndComp
Text GLabel 5600 5400 0    50   Input ~ 0
PIN_DF_RX
Text GLabel 5700 4200 3    50   Input ~ 0
PIN_DF_RX
$Comp
L Device:R R6
U 1 1 5DDC0CD0
P 6700 2800
F 0 "R6" H 6770 2846 50  0000 L CNN
F 1 "6k34" H 6770 2755 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal" V 6630 2800 50  0001 C CNN
F 3 "~" H 6700 2800 50  0001 C CNN
	1    6700 2800
	1    0    0    -1  
$EndComp
Connection ~ 6700 2650
Wire Wire Line
	6450 3050 6700 3050
Wire Wire Line
	6700 2950 6700 3050
Connection ~ 6700 3050
$Comp
L Diode:1N5817 D2
U 1 1 5DCE1235
P 3850 2950
F 0 "D2" H 3850 2734 50  0000 C CNN
F 1 "1N5817" H 3850 2825 50  0000 C CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 3850 2775 50  0001 C CNN
F 3 "http://www.vishay.com/docs/88525/1n5817.pdf" H 3850 2950 50  0001 C CNN
	1    3850 2950
	-1   0    0    1   
$EndComp
Wire Wire Line
	4000 2950 4350 2950
Connection ~ 4350 2950
$Comp
L Device:LED D1
U 1 1 5DCE8D68
P 3550 3200
F 0 "D1" V 3589 3083 50  0000 R CNN
F 1 "LED" V 3498 3083 50  0000 R CNN
F 2 "LED_THT:LED_D5.0mm" H 3550 3200 50  0001 C CNN
F 3 "~" H 3550 3200 50  0001 C CNN
	1    3550 3200
	0    -1   -1   0   
$EndComp
$Comp
L power:+BATT #PWR0116
U 1 1 5DD0F839
P 8550 1750
F 0 "#PWR0116" H 8550 1600 50  0001 C CNN
F 1 "+BATT" H 8565 1923 50  0000 C CNN
F 2 "" H 8550 1750 50  0001 C CNN
F 3 "" H 8550 1750 50  0001 C CNN
	1    8550 1750
	1    0    0    -1  
$EndComp
Wire Wire Line
	8550 1850 8550 1750
Wire Wire Line
	3350 1600 6850 1600
Wire Wire Line
	3150 2000 4150 2000
Wire Wire Line
	3350 2950 3550 2950
Wire Wire Line
	3550 3050 3550 2950
Connection ~ 3550 2950
Wire Wire Line
	3550 2950 3700 2950
$Comp
L Device:R R7
U 1 1 5DD38654
P 3550 3600
F 0 "R7" H 3620 3646 50  0000 L CNN
F 1 "230" H 3620 3555 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal" V 3480 3600 50  0001 C CNN
F 3 "~" H 3550 3600 50  0001 C CNN
	1    3550 3600
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0118
U 1 1 5DD38D51
P 3550 3850
F 0 "#PWR0118" H 3550 3600 50  0001 C CNN
F 1 "GND" H 3555 3677 50  0000 C CNN
F 2 "" H 3550 3850 50  0001 C CNN
F 3 "" H 3550 3850 50  0001 C CNN
	1    3550 3850
	1    0    0    -1  
$EndComp
Wire Wire Line
	3550 3850 3550 3750
Wire Wire Line
	3550 3450 3550 3350
$Comp
L Transistor_FET:IRLML2060 Q6
U 1 1 5DD16080
P 7100 2950
F 0 "Q6" H 7306 2996 50  0000 L CNN
F 1 "IRLML2060" H 7306 2905 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 7300 2875 50  0001 L CIN
F 3 "https://www.infineon.com/dgdl/irlml2060pbf.pdf?fileId=5546d462533600a401535664b7fb25ee" H 7100 2950 50  0001 L CNN
	1    7100 2950
	0    -1   1    0   
$EndComp
$Comp
L power:GND #PWR0119
U 1 1 5DD1A09D
P 7400 3150
F 0 "#PWR0119" H 7400 2900 50  0001 C CNN
F 1 "GND" H 7405 2977 50  0000 C CNN
F 2 "" H 7400 3150 50  0001 C CNN
F 3 "" H 7400 3150 50  0001 C CNN
	1    7400 3150
	1    0    0    -1  
$EndComp
$Comp
L Device:R R8
U 1 1 5DD1A632
P 7400 2900
F 0 "R8" H 7470 2946 50  0000 L CNN
F 1 "6k34" H 7470 2855 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal" V 7330 2900 50  0001 C CNN
F 3 "~" H 7400 2900 50  0001 C CNN
	1    7400 2900
	1    0    0    -1  
$EndComp
Wire Wire Line
	6700 3050 6900 3050
Wire Wire Line
	7300 3050 7400 3050
Wire Wire Line
	7400 3050 7400 3150
Connection ~ 7400 3050
Wire Wire Line
	7100 2750 7100 2650
Wire Wire Line
	7100 2650 7400 2650
Wire Wire Line
	7400 2750 7400 2650
Connection ~ 7400 2650
Wire Wire Line
	7400 2650 7500 2650
Wire Wire Line
	5700 4200 5700 4100
Wire Wire Line
	4300 5800 4500 5800
Wire Wire Line
	4500 5800 4500 5900
Text GLabel 6050 5600 0    50   Input ~ 0
PIN_AUDIO_RIGHT
Text GLabel 6050 5700 0    50   Input ~ 0
PIN_AUDIO_LEFT
Text GLabel 4500 5700 2    50   Input ~ 0
PIN_AUDIO_RIGHT
Text GLabel 4500 5600 2    50   Input ~ 0
PIN_AUDIO_LEFT
Wire Wire Line
	4300 5700 4500 5700
Wire Wire Line
	4300 5600 4500 5600
$Comp
L power:GND #PWR?
U 1 1 5DDCE12C
P 5800 5900
F 0 "#PWR?" H 5800 5650 50  0001 C CNN
F 1 "GND" V 5805 5772 50  0000 R CNN
F 2 "" H 5800 5900 50  0001 C CNN
F 3 "" H 5800 5900 50  0001 C CNN
	1    5800 5900
	0    1    1    0   
$EndComp
Wire Wire Line
	5800 5900 6050 5900
$EndSCHEMATC
