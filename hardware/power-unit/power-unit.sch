EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "Power unit"
Date "2020-03-01"
Rev "0.1"
Comp "bubblefoil"
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Connector:Screw_Terminal_01x03 J5
U 1 1 5E5C1FE4
P 6200 3750
F 0 "J5" H 6280 3792 50  0000 L CNN
F 1 "LED left" H 6280 3701 50  0000 L CNN
F 2 "TerminalBlock:TerminalBlock_Altech_AK300-3_P5.00mm" H 6200 3750 50  0001 C CNN
F 3 "~" H 6200 3750 50  0001 C CNN
	1    6200 3750
	1    0    0    -1  
$EndComp
$Comp
L Connector:Screw_Terminal_01x03 J6
U 1 1 5E5C34B9
P 6200 4050
F 0 "J6" H 6280 4092 50  0000 L CNN
F 1 "LED right" H 6280 4001 50  0000 L CNN
F 2 "TerminalBlock:TerminalBlock_Altech_AK300-3_P5.00mm" H 6200 4050 50  0001 C CNN
F 3 "~" H 6200 4050 50  0001 C CNN
	1    6200 4050
	1    0    0    -1  
$EndComp
$Comp
L Device:Fuse F1
U 1 1 5E5C7167
P 5350 4000
F 0 "F1" V 5250 3950 50  0000 C CNN
F 1 "5A" V 5450 4100 50  0000 C CNN
F 2 "Fuse:Fuseholder_Cylinder-5x20mm_Stelvio-Kontek_PTF78_Horizontal_Open" V 5280 4000 50  0001 C CNN
F 3 "~" H 5350 4000 50  0001 C CNN
	1    5350 4000
	0    1    1    0   
$EndComp
$Comp
L Device:Fuse F2
U 1 1 5E5C7FAF
P 5350 4300
F 0 "F2" V 5250 4250 50  0000 C CNN
F 1 "2.5A" V 5450 4400 50  0000 C CNN
F 2 "Fuse:Fuseholder_Cylinder-5x20mm_Stelvio-Kontek_PTF78_Horizontal_Open" V 5280 4300 50  0001 C CNN
F 3 "~" H 5350 4300 50  0001 C CNN
	1    5350 4300
	0    1    1    0   
$EndComp
$Comp
L Device:Fuse F3
U 1 1 5E5C82C6
P 5350 4550
F 0 "F3" V 5250 4500 50  0000 C CNN
F 1 "2.5A" V 5450 4550 50  0000 C CNN
F 2 "Fuse:Fuseholder_Cylinder-5x20mm_Stelvio-Kontek_PTF78_Horizontal_Open" V 5280 4550 50  0001 C CNN
F 3 "~" H 5350 4550 50  0001 C CNN
	1    5350 4550
	0    1    1    0   
$EndComp
$Comp
L Connector:Screw_Terminal_01x02 J1
U 1 1 5E5CACE4
P 6200 2850
F 0 "J1" H 6280 2842 50  0000 L CNN
F 1 "VCC" H 6280 2751 50  0000 L CNN
F 2 "TerminalBlock:TerminalBlock_Altech_AK300-2_P5.00mm" H 6200 2850 50  0001 C CNN
F 3 "~" H 6200 2850 50  0001 C CNN
	1    6200 2850
	1    0    0    -1  
$EndComp
$Comp
L Connector:Screw_Terminal_01x02 J2
U 1 1 5E5CBD00
P 6200 3050
F 0 "J2" H 6280 3042 50  0000 L CNN
F 1 "PWR Switch" H 6280 2951 50  0000 L CNN
F 2 "TerminalBlock:TerminalBlock_Altech_AK300-2_P5.00mm" H 6200 3050 50  0001 C CNN
F 3 "~" H 6200 3050 50  0001 C CNN
	1    6200 3050
	1    0    0    -1  
$EndComp
$Comp
L Connector:Screw_Terminal_01x02 J3
U 1 1 5E5CE32B
P 6200 3250
F 0 "J3" H 6280 3242 50  0000 L CNN
F 1 "5V out" H 6280 3151 50  0000 L CNN
F 2 "TerminalBlock:TerminalBlock_Altech_AK300-2_P5.00mm" H 6200 3250 50  0001 C CNN
F 3 "~" H 6200 3250 50  0001 C CNN
	1    6200 3250
	1    0    0    -1  
$EndComp
$Comp
L Connector:Screw_Terminal_01x02 J4
U 1 1 5E5CE69E
P 6200 3450
F 0 "J4" H 6280 3442 50  0000 L CNN
F 1 "Rear LED data" H 6280 3351 50  0000 L CNN
F 2 "TerminalBlock:TerminalBlock_Altech_AK300-2_P5.00mm" H 6200 3450 50  0001 C CNN
F 3 "~" H 6200 3450 50  0001 C CNN
	1    6200 3450
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR01
U 1 1 5E5D3689
P 3750 4200
F 0 "#PWR01" H 3750 4050 50  0001 C CNN
F 1 "+5V" H 3765 4373 50  0000 C CNN
F 2 "" H 3750 4200 50  0001 C CNN
F 3 "" H 3750 4200 50  0001 C CNN
	1    3750 4200
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR03
U 1 1 5E5D4034
P 3750 4600
F 0 "#PWR03" H 3750 4350 50  0001 C CNN
F 1 "GND" H 3750 4450 50  0000 C CNN
F 2 "" H 3750 4600 50  0001 C CNN
F 3 "" H 3750 4600 50  0001 C CNN
	1    3750 4600
	1    0    0    -1  
$EndComp
$Comp
L power:GNDPWR #PWR09
U 1 1 5E5D5763
P 6000 2650
F 0 "#PWR09" H 6000 2450 50  0001 C CNN
F 1 "GNDPWR" V 6005 2542 50  0000 R CNN
F 2 "" H 6000 2600 50  0001 C CNN
F 3 "" H 6000 2600 50  0001 C CNN
	1    6000 2650
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR07
U 1 1 5E5E74AE
P 5800 4550
F 0 "#PWR07" H 5800 4300 50  0001 C CNN
F 1 "GND" H 5805 4377 50  0000 C CNN
F 2 "" H 5800 4550 50  0001 C CNN
F 3 "" H 5800 4550 50  0001 C CNN
	1    5800 4550
	1    0    0    -1  
$EndComp
Wire Wire Line
	6000 2650 6000 2850
$Comp
L Mechanical:MountingHole_Pad H3
U 1 1 5E5F736D
P 3650 4200
F 0 "H3" V 3650 4400 50  0000 C CNN
F 1 "+5V" V 3550 4100 50  0000 C CNN
F 2 "Connector_Wire:SolderWirePad_1x01_Drill1.2mm" H 3650 4200 50  0001 C CNN
F 3 "~" H 3650 4200 50  0001 C CNN
	1    3650 4200
	0    -1   -1   0   
$EndComp
$Comp
L Mechanical:MountingHole_Pad H4
U 1 1 5E5F8143
P 3650 4600
F 0 "H4" V 3650 4800 50  0000 C CNN
F 1 "GND" V 3700 4500 50  0000 C CNN
F 2 "Connector_Wire:SolderWirePad_1x01_Drill1.2mm" H 3650 4600 50  0001 C CNN
F 3 "~" H 3650 4600 50  0001 C CNN
	1    3650 4600
	0    -1   -1   0   
$EndComp
Wire Wire Line
	6000 3450 5900 3450
Wire Wire Line
	5900 3450 5900 3950
Wire Wire Line
	5900 3950 6000 3950
Wire Wire Line
	6000 3550 5950 3550
Wire Wire Line
	5950 3550 5950 3850
Wire Wire Line
	5950 3850 6000 3850
Wire Wire Line
	6000 3650 5800 3650
Wire Wire Line
	5800 3650 5800 4150
Wire Wire Line
	5800 4150 6000 4150
Wire Wire Line
	6000 3350 5800 3350
Wire Wire Line
	5800 3350 5800 3650
Connection ~ 5800 3650
Wire Wire Line
	5800 4150 5800 4550
Connection ~ 5800 4150
Wire Wire Line
	5500 4300 5550 4300
Wire Wire Line
	5550 4300 5550 3750
Wire Wire Line
	5550 3750 6000 3750
Wire Wire Line
	6000 4050 5650 4050
Wire Wire Line
	5650 4050 5650 4550
Wire Wire Line
	5650 4550 5500 4550
$Comp
L power:+5V #PWR05
U 1 1 5E5FF4A1
P 5000 4000
F 0 "#PWR05" H 5000 3850 50  0001 C CNN
F 1 "+5V" H 5015 4173 50  0000 C CNN
F 2 "" H 5000 4000 50  0001 C CNN
F 3 "" H 5000 4000 50  0001 C CNN
	1    5000 4000
	1    0    0    -1  
$EndComp
Wire Wire Line
	5000 4000 5200 4000
Wire Wire Line
	5200 4000 5200 4300
Connection ~ 5200 4000
Wire Wire Line
	5200 4300 5200 4550
Connection ~ 5200 4300
Wire Wire Line
	5500 3250 5500 4000
Wire Wire Line
	6000 3250 5500 3250
$Comp
L power:GNDPWR #PWR04
U 1 1 5E5F6CFF
P 5300 3300
F 0 "#PWR04" H 5300 3100 50  0001 C CNN
F 1 "GNDPWR" H 5304 3146 50  0000 C CNN
F 2 "" H 5300 3250 50  0001 C CNN
F 3 "" H 5300 3250 50  0001 C CNN
	1    5300 3300
	1    0    0    -1  
$EndComp
Wire Wire Line
	3750 2950 3800 2950
$Comp
L Mechanical:MountingHole_Pad H2
U 1 1 5E5EB13F
P 3650 2950
F 0 "H2" V 3650 3150 50  0000 C CNN
F 1 "GND" V 3700 2850 50  0000 C CNN
F 2 "Connector_Wire:SolderWirePad_1x01_Drill1.2mm" H 3650 2950 50  0001 C CNN
F 3 "~" H 3650 2950 50  0001 C CNN
	1    3650 2950
	0    -1   -1   0   
$EndComp
$Comp
L Mechanical:MountingHole_Pad H1
U 1 1 5E5EA0FF
P 3650 2650
F 0 "H1" V 3650 2850 50  0000 C CNN
F 1 "VCC" V 3700 2550 50  0000 C CNN
F 2 "Connector_Wire:SolderWirePad_1x01_Drill1.2mm" H 3650 2650 50  0001 C CNN
F 3 "~" H 3650 2650 50  0001 C CNN
	1    3650 2650
	0    -1   -1   0   
$EndComp
$Comp
L Device:D D1
U 1 1 5E5D2272
P 4800 3500
F 0 "D1" H 4800 3400 50  0000 C CNN
F 1 "D" H 4800 3600 50  0000 C CNN
F 2 "Diode_THT:D_DO-41_SOD81_P12.70mm_Horizontal" H 4800 3500 50  0001 C CNN
F 3 "~" H 4800 3500 50  0001 C CNN
	1    4800 3500
	1    0    0    -1  
$EndComp
$Comp
L power:GNDPWR #PWR02
U 1 1 5E5CFB75
P 3800 2950
F 0 "#PWR02" H 3800 2750 50  0001 C CNN
F 1 "GNDPWR" H 3804 2796 50  0000 C CNN
F 2 "" H 3800 2900 50  0001 C CNN
F 3 "" H 3800 2900 50  0001 C CNN
	1    3800 2950
	1    0    0    -1  
$EndComp
Text GLabel 6000 3050 0    50   Input ~ 0
BAT+
Text GLabel 6000 2950 0    50   Input ~ 0
BAT+
Text GLabel 5100 2750 2    50   Input ~ 0
BAT+
$Comp
L Relay:G5LE-1 K1
U 1 1 5E5C5AA2
P 4800 2950
F 0 "K1" V 5367 2950 50  0000 C CNN
F 1 "G5LE-1" V 5276 2950 50  0000 C CNN
F 2 "Relay_THT:Relay_SPDT_Omron-G5LE-1" H 5250 2900 50  0001 L CNN
F 3 "http://www.omron.com/ecb/products/pdf/en-g5le.pdf" H 4800 2950 50  0001 C CNN
	1    4800 2950
	0    -1   -1   0   
$EndComp
NoConn ~ 4500 2850
Wire Wire Line
	4500 2650 3750 2650
Text GLabel 6000 3150 0    50   Input ~ 0
PRWSW
Text GLabel 4500 3150 0    50   Input ~ 0
PRWSW
Text GLabel 4650 3500 0    50   Input ~ 0
PRWSW
Wire Wire Line
	5100 3150 5300 3150
Wire Wire Line
	5300 3150 5300 3300
Wire Wire Line
	4950 3500 5100 3500
Wire Wire Line
	5100 3500 5100 3150
Connection ~ 5100 3150
$Comp
L Mechanical:MountingHole H5
U 1 1 5E69599A
P 3000 2300
F 0 "H5" H 3100 2346 50  0000 L CNN
F 1 "MountingHole" H 3100 2255 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 3000 2300 50  0001 C CNN
F 3 "~" H 3000 2300 50  0001 C CNN
	1    3000 2300
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H7
U 1 1 5E699EA8
P 5600 2300
F 0 "H7" H 5700 2346 50  0000 L CNN
F 1 "MountingHole" H 5700 2255 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 5600 2300 50  0001 C CNN
F 3 "~" H 5600 2300 50  0001 C CNN
	1    5600 2300
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H8
U 1 1 5E69A42C
P 5700 4950
F 0 "H8" H 5800 4996 50  0000 L CNN
F 1 "MountingHole" H 5800 4905 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 5700 4950 50  0001 C CNN
F 3 "~" H 5700 4950 50  0001 C CNN
	1    5700 4950
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H6
U 1 1 5E69AC62
P 3000 4950
F 0 "H6" H 3100 4996 50  0000 L CNN
F 1 "MountingHole" H 3100 4905 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 3000 4950 50  0001 C CNN
F 3 "~" H 3000 4950 50  0001 C CNN
	1    3000 4950
	1    0    0    -1  
$EndComp
$EndSCHEMATC
