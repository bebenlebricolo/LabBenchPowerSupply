EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 3
Title "Lab bench power supply"
Date "2020-03-25"
Rev "1.0"
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Text Label 8700 1100 0    60   ~ 0
1(Tx)
Text Label 8700 1200 0    60   ~ 0
0(Rx)
Text Label 8700 1300 0    60   ~ 0
Reset
Text Label 8700 1900 2    60   ~ 0
fan_control
Text Label 8700 2000 0    60   ~ 0
7
Text Label 8700 2100 0    60   ~ 0
8
Text Label 8700 2400 0    60   ~ 0
11(**/MOSI)
Text Label 8700 2500 0    60   ~ 0
12(MISO)
Text Label 10550 2500 0    60   ~ 0
13(SCK)
Text Label 10550 2100 0    60   ~ 0
OutVoltageSensing
Text Label 10550 2000 0    60   ~ 0
SecondarySmoothed
Text Label 10550 1900 0    60   ~ 0
TemperatureMeasurement
Text Label 10550 1800 0    60   ~ 0
I2C_SDA
Text Label 10550 1700 0    60   ~ 0
I2C_SCL
Text Label 10550 1600 0    60   ~ 0
A6
Text Label 10550 1500 0    60   ~ 0
A7
Text Label 10550 2300 0    60   ~ 0
AREF
Text Label 10550 1300 0    60   ~ 0
Reset
Text Notes 10800 1000 0    60   ~ 0
Holes
Text Notes 8500 575  0    60   ~ 0
Shield for Arduino Nano
Wire Notes Line
	8475 650  9675 650 
Wire Notes Line
	9675 650  9675 475 
$Comp
L Connector_Generic:Conn_01x01 P3
U 1 1 56D73ADD
P 10800 650
F 0 "P3" V 10900 650 50  0000 C CNN
F 1 "CONN_01X01" V 10900 650 50  0001 C CNN
F 2 "Socket_Arduino_Nano:1pin_Nano" H 10800 650 50  0001 C CNN
F 3 "" H 10800 650 50  0000 C CNN
	1    10800 650 
	0    -1   -1   0   
$EndComp
NoConn ~ 10800 850 
$Comp
L Connector_Generic:Conn_01x01 P4
U 1 1 56D73D86
P 10900 650
F 0 "P4" V 11000 650 50  0000 C CNN
F 1 "CONN_01X01" V 11000 650 50  0001 C CNN
F 2 "Socket_Arduino_Nano:1pin_Nano" H 10900 650 50  0001 C CNN
F 3 "" H 10900 650 50  0000 C CNN
	1    10900 650 
	0    -1   -1   0   
$EndComp
$Comp
L Connector_Generic:Conn_01x01 P5
U 1 1 56D73DAE
P 11000 650
F 0 "P5" V 11100 650 50  0000 C CNN
F 1 "CONN_01X01" V 11100 650 50  0001 C CNN
F 2 "Socket_Arduino_Nano:1pin_Nano" H 11000 650 50  0001 C CNN
F 3 "" H 11000 650 50  0000 C CNN
	1    11000 650 
	0    -1   -1   0   
$EndComp
$Comp
L Connector_Generic:Conn_01x01 P6
U 1 1 56D73DD9
P 11100 650
F 0 "P6" V 11200 650 50  0000 C CNN
F 1 "CONN_01X01" V 11200 650 50  0001 C CNN
F 2 "Socket_Arduino_Nano:1pin_Nano" H 11100 650 50  0001 C CNN
F 3 "" H 11100 650 50  0000 C CNN
	1    11100 650 
	0    -1   -1   0   
$EndComp
NoConn ~ 10900 850 
NoConn ~ 11000 850 
NoConn ~ 11100 850 
$Comp
L Connector_Generic:Conn_01x15 P1
U 1 1 56D73FAC
P 9550 1800
F 0 "P1" H 9550 2600 50  0000 C CNN
F 1 "Digital" V 9650 1800 50  0000 C CNN
F 2 "Socket_Arduino_Nano:Socket_Strip_Arduino_1x15" H 9550 1800 50  0001 C CNN
F 3 "" H 9550 1800 50  0000 C CNN
	1    9550 1800
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x15 P2
U 1 1 56D740C7
P 9950 1800
F 0 "P2" H 9950 2600 50  0000 C CNN
F 1 "Analog" V 10050 1800 50  0000 C CNN
F 2 "Socket_Arduino_Nano:Socket_Strip_Arduino_1x15" H 9950 1800 50  0001 C CNN
F 3 "" H 9950 1800 50  0000 C CNN
	1    9950 1800
	-1   0    0    -1  
$EndComp
$Comp
L power:GND #PWR01
U 1 1 56D7422C
P 9250 2600
F 0 "#PWR01" H 9250 2350 50  0001 C CNN
F 1 "GND" H 9250 2450 50  0000 C CNN
F 2 "" H 9250 2600 50  0000 C CNN
F 3 "" H 9250 2600 50  0000 C CNN
	1    9250 2600
	1    0    0    -1  
$EndComp
Wire Wire Line
	9350 1400 9250 1400
Wire Wire Line
	9250 1400 9250 2600
Wire Wire Line
	9350 1100 8700 1100
Wire Wire Line
	8700 1200 9350 1200
Wire Wire Line
	9350 1300 8700 1300
Wire Wire Line
	8700 1500 9350 1500
Wire Wire Line
	9350 1600 8700 1600
Wire Wire Line
	8700 1700 9350 1700
Wire Wire Line
	9350 1800 8700 1800
Wire Wire Line
	8700 1900 9350 1900
Wire Wire Line
	9350 2000 8700 2000
Wire Wire Line
	8700 2100 9350 2100
Wire Wire Line
	9350 2200 8700 2200
Wire Wire Line
	8700 2300 9350 2300
Wire Wire Line
	9350 2400 8700 2400
Wire Wire Line
	8700 2500 9350 2500
$Comp
L power:GND #PWR02
U 1 1 56D746ED
P 10250 2600
F 0 "#PWR02" H 10250 2350 50  0001 C CNN
F 1 "GND" H 10250 2450 50  0000 C CNN
F 2 "" H 10250 2600 50  0000 C CNN
F 3 "" H 10250 2600 50  0000 C CNN
	1    10250 2600
	1    0    0    -1  
$EndComp
Wire Wire Line
	10250 2600 10250 1200
Wire Wire Line
	10250 1200 10150 1200
Wire Wire Line
	10150 1100 10250 1100
Wire Wire Line
	10250 1100 10250 950 
$Comp
L power:+5V #PWR03
U 1 1 56D747E8
P 10350 950
F 0 "#PWR03" H 10350 800 50  0001 C CNN
F 1 "+5V" V 10350 1150 28  0000 C CNN
F 2 "" H 10350 950 50  0000 C CNN
F 3 "" H 10350 950 50  0000 C CNN
	1    10350 950 
	1    0    0    -1  
$EndComp
Wire Wire Line
	10350 950  10350 1400
Wire Wire Line
	10350 1400 10150 1400
$Comp
L power:+3V3 #PWR04
U 1 1 56D74854
P 10450 950
F 0 "#PWR04" H 10450 800 50  0001 C CNN
F 1 "+3.3V" V 10450 1150 28  0000 C CNN
F 2 "" H 10450 950 50  0000 C CNN
F 3 "" H 10450 950 50  0000 C CNN
	1    10450 950 
	1    0    0    -1  
$EndComp
Wire Wire Line
	10450 950  10450 2400
Wire Wire Line
	10450 2400 10150 2400
Wire Wire Line
	10550 1300 10150 1300
Wire Wire Line
	10150 1500 10550 1500
Wire Wire Line
	10550 1600 10150 1600
Wire Wire Line
	10550 1700 10150 1700
Wire Wire Line
	10150 1800 10550 1800
Wire Wire Line
	10550 1900 10150 1900
Wire Wire Line
	10550 2000 10150 2000
Wire Wire Line
	10150 2100 10550 2100
Wire Wire Line
	10550 2200 10150 2200
Wire Wire Line
	10550 2300 10150 2300
Wire Wire Line
	10150 2500 10550 2500
Wire Notes Line
	11200 1050 10650 1050
Wire Notes Line
	10650 1050 10650 500 
Wire Notes Line
	11200 2850 8450 2850
Wire Notes Line
	8450 2850 8450 500 
Text Notes 9650 1100 0    60   ~ 0
1
$Comp
L Device:C C?
U 1 1 5E79D587
P 2675 1175
F 0 "C?" H 2790 1221 50  0000 L CNN
F 1 "C" H 2790 1130 50  0000 L CNN
F 2 "" H 2713 1025 50  0001 C CNN
F 3 "~" H 2675 1175 50  0001 C CNN
	1    2675 1175
	1    0    0    -1  
$EndComp
Wire Wire Line
	3050 1000 2675 1000
Wire Wire Line
	2675 1000 2675 1025
Wire Wire Line
	2675 1325 2675 1400
Wire Wire Line
	2675 1400 3050 1400
$Comp
L Device:Fuse F?
U 1 1 5E79F192
P 2350 1000
F 0 "F?" V 2153 1000 50  0000 C CNN
F 1 "Fuse" V 2244 1000 50  0000 C CNN
F 2 "" V 2280 1000 50  0001 C CNN
F 3 "~" H 2350 1000 50  0001 C CNN
	1    2350 1000
	0    1    1    0   
$EndComp
Wire Wire Line
	2500 1000 2675 1000
Connection ~ 2675 1000
Wire Wire Line
	2675 1400 2200 1400
Connection ~ 2675 1400
$Comp
L Device:C C?
U 1 1 5E7A17CF
P 2675 1550
F 0 "C?" H 2790 1596 50  0000 L CNN
F 1 "C" H 2790 1505 50  0000 L CNN
F 2 "" H 2713 1400 50  0001 C CNN
F 3 "~" H 2675 1550 50  0001 C CNN
	1    2675 1550
	1    0    0    -1  
$EndComp
$Comp
L power:Earth #PWR?
U 1 1 5E7A9721
P 5050 1825
F 0 "#PWR?" H 5050 1575 50  0001 C CNN
F 1 "Earth" H 5050 1675 50  0001 C CNN
F 2 "" H 5050 1825 50  0001 C CNN
F 3 "~" H 5050 1825 50  0001 C CNN
	1    5050 1825
	1    0    0    -1  
$EndComp
Connection ~ 5050 1825
$Comp
L Device:CP1 C?
U 1 1 5E7AB2AF
P 6075 1400
F 0 "C?" H 6190 1446 50  0000 L CNN
F 1 "470µ" H 6190 1355 50  0000 L CNN
F 2 "" H 6075 1400 50  0001 C CNN
F 3 "~" H 6075 1400 50  0001 C CNN
	1    6075 1400
	1    0    0    -1  
$EndComp
Text Notes 3050 1800 0    50   ~ 0
220 V AC / 2x11.5 parallel\n*  12V AC / 16V DC rectified
$Comp
L Connector:Conn_WallPlug_Earth P?
U 1 1 5E7C9D33
P 1275 1100
F 0 "P?" H 1342 1425 50  0000 C CNN
F 1 "Conn_WallPlug_Earth" H 875 1325 50  0000 C CNN
F 2 "" H 1675 1100 50  0001 C CNN
F 3 "~" H 1675 1100 50  0001 C CNN
	1    1275 1100
	1    0    0    -1  
$EndComp
Wire Wire Line
	2675 1700 2675 1825
Wire Wire Line
	2675 1825 1675 1825
Wire Wire Line
	1575 1000 1650 1000
Wire Wire Line
	1575 1200 2200 1200
Wire Wire Line
	2200 1200 2200 1400
Wire Wire Line
	1575 1400 1675 1400
Wire Wire Line
	1675 1400 1675 1825
$Comp
L Switch:SW_SPST SW?
U 1 1 5E7DFAE2
P 1850 1000
F 0 "SW?" H 1850 1235 50  0000 C CNN
F 1 "SW_SPST" H 1850 1144 50  0000 C CNN
F 2 "" H 1850 1000 50  0001 C CNN
F 3 "~" H 1850 1000 50  0001 C CNN
	1    1850 1000
	1    0    0    -1  
$EndComp
Wire Wire Line
	2050 1000 2200 1000
$Comp
L Device:CP1 C?
U 1 1 5E7E36FA
P 5725 1400
F 0 "C?" H 5840 1446 50  0000 L CNN
F 1 "470µ" H 5840 1355 50  0000 L CNN
F 2 "" H 5725 1400 50  0001 C CNN
F 3 "~" H 5725 1400 50  0001 C CNN
	1    5725 1400
	1    0    0    -1  
$EndComp
Wire Wire Line
	6075 1825 6075 1550
Wire Wire Line
	5050 1825 5725 1825
Wire Wire Line
	6075 1250 6075 950 
Wire Wire Line
	5725 1250 5725 950 
Wire Wire Line
	5725 950  6075 950 
Wire Wire Line
	5725 1550 5725 1825
Connection ~ 5725 1825
Wire Wire Line
	5725 1825 6075 1825
Wire Wire Line
	6075 950  6300 950 
Connection ~ 6075 950 
$Comp
L Device:D D?
U 1 1 5E7FB0DA
P 4800 1375
F 0 "D?" H 4800 1159 50  0000 C CNN
F 1 "D" H 4800 1250 50  0000 C CNN
F 2 "" H 4800 1375 50  0001 C CNN
F 3 "~" H 4800 1375 50  0001 C CNN
	1    4800 1375
	0    1    1    0   
$EndComp
$Comp
L Device:D D?
U 1 1 5E7FBA05
P 4950 1225
F 0 "D?" V 4996 1146 50  0000 R CNN
F 1 "D" V 4905 1146 50  0000 R CNN
F 2 "" H 4950 1225 50  0001 C CNN
F 3 "~" H 4950 1225 50  0001 C CNN
	1    4950 1225
	-1   0    0    1   
$EndComp
$Comp
L Device:D D?
U 1 1 5E7FBD99
P 5100 1375
F 0 "D?" H 5100 1159 50  0000 C CNN
F 1 "D" H 5100 1250 50  0000 C CNN
F 2 "" H 5100 1375 50  0001 C CNN
F 3 "~" H 5100 1375 50  0001 C CNN
	1    5100 1375
	0    1    1    0   
$EndComp
$Comp
L Device:D D?
U 1 1 5E7FC2B4
P 4950 1525
F 0 "D?" V 4996 1446 50  0000 R CNN
F 1 "D" V 4905 1446 50  0000 R CNN
F 2 "" H 4950 1525 50  0001 C CNN
F 3 "~" H 4950 1525 50  0001 C CNN
	1    4950 1525
	-1   0    0    1   
$EndComp
Wire Wire Line
	4800 1525 4800 1825
Connection ~ 4800 1525
Wire Wire Line
	4800 1825 5050 1825
Connection ~ 4800 1225
$Comp
L NMOS_TH:IRFZ14PBF U?
U 1 1 5E823E21
P 1450 2525
F 0 "U?" V 1743 2525 50  0000 C CNN
F 1 "IRFZ14PBF" V 1652 2525 50  0000 C CNN
F 2 "" H 1450 2525 50  0001 C CNN
F 3 "" H 1450 2525 50  0001 C CNN
	1    1450 2525
	0    -1   -1   0   
$EndComp
Wire Wire Line
	1100 2475 1250 2475
Wire Wire Line
	1650 2475 1925 2475
Wire Wire Line
	1925 2475 1925 2625
$Comp
L Device:D_Schottky D?
U 1 1 5E82A595
P 1925 2775
F 0 "D?" V 1879 2854 50  0000 L CNN
F 1 "D_Schottky" V 1970 2854 50  0000 L CNN
F 2 "" H 1925 2775 50  0001 C CNN
F 3 "~" H 1925 2775 50  0001 C CNN
	1    1925 2775
	0    1    1    0   
$EndComp
$Comp
L power:Earth #PWR?
U 1 1 5E82CEAE
P 1925 3100
F 0 "#PWR?" H 1925 2850 50  0001 C CNN
F 1 "Earth" H 1925 2950 50  0001 C CNN
F 2 "" H 1925 3100 50  0001 C CNN
F 3 "~" H 1925 3100 50  0001 C CNN
	1    1925 3100
	1    0    0    -1  
$EndComp
$Comp
L Device:L L?
U 1 1 5E82DAFE
P 2225 2475
F 0 "L?" V 2415 2475 50  0000 C CNN
F 1 "110uH" V 2324 2475 50  0000 C CNN
F 2 "" H 2225 2475 50  0001 C CNN
F 3 "~" H 2225 2475 50  0001 C CNN
	1    2225 2475
	0    -1   -1   0   
$EndComp
Wire Wire Line
	1925 2475 2075 2475
Connection ~ 1925 2475
Wire Wire Line
	6075 1825 6325 1825
Connection ~ 6075 1825
$Comp
L Device:CP1 C?
U 1 1 5E853BFE
P 2600 2800
F 0 "C?" H 2715 2846 50  0000 L CNN
F 1 "CP1" H 2715 2755 50  0000 L CNN
F 2 "" H 2600 2800 50  0001 C CNN
F 3 "~" H 2600 2800 50  0001 C CNN
	1    2600 2800
	1    0    0    -1  
$EndComp
$Comp
L Device:CP1 C?
U 1 1 5E8541D6
P 2975 2800
F 0 "C?" H 3090 2846 50  0000 L CNN
F 1 "CP1" H 3090 2755 50  0000 L CNN
F 2 "" H 2975 2800 50  0001 C CNN
F 3 "~" H 2975 2800 50  0001 C CNN
	1    2975 2800
	1    0    0    -1  
$EndComp
Wire Wire Line
	2375 2475 2600 2475
Wire Wire Line
	2975 2475 2975 2650
Wire Wire Line
	2600 2650 2600 2475
Connection ~ 2600 2475
Wire Wire Line
	2600 2475 2975 2475
Wire Wire Line
	2975 3100 2975 2950
Wire Wire Line
	2600 2950 2600 3100
Connection ~ 2600 3100
Wire Wire Line
	2600 3100 2975 3100
Wire Wire Line
	2975 2475 3375 2475
Connection ~ 2975 2475
$Comp
L Device:R_Shunt R?
U 1 1 5E866D27
P 3450 3100
F 0 "R?" V 3225 3100 50  0000 C CNN
F 1 "R_Shunt" V 3316 3100 50  0000 C CNN
F 2 "" V 3380 3100 50  0001 C CNN
F 3 "~" H 3450 3100 50  0001 C CNN
	1    3450 3100
	0    1    1    0   
$EndComp
Wire Wire Line
	2975 3100 3250 3100
Connection ~ 2975 3100
Wire Wire Line
	3650 3100 3950 3100
Text Label 6300 950  0    50   ~ 0
MainPowerLine
Text Label 4350 2475 0    50   ~ 0
RegulatedOut
Text Label 6325 1825 0    50   ~ 0
CommonLine
Text Label 1425 3100 2    50   ~ 0
CommonLine
Wire Wire Line
	3350 3250 2975 3250
Wire Wire Line
	2975 3250 2975 3100
Wire Wire Line
	3550 3250 3550 3475
$Comp
L Device:CP1 C?
U 1 1 5E87E7BA
P 2600 3250
F 0 "C?" H 2485 3204 50  0000 R CNN
F 1 "CP1" H 2485 3295 50  0000 R CNN
F 2 "" H 2600 3250 50  0001 C CNN
F 3 "~" H 2600 3250 50  0001 C CNN
	1    2600 3250
	-1   0    0    1   
$EndComp
Wire Wire Line
	2600 3400 2600 3475
Wire Wire Line
	2375 3475 2600 3475
Connection ~ 2600 3475
Wire Wire Line
	2600 3475 3550 3475
Text Label 2375 3475 2    50   ~ 0
CurrentSensing
Text Label 10550 2200 0    50   ~ 0
CurrentSensing
Wire Wire Line
	3550 4050 3550 4200
$Comp
L Transistor_BJT:2N2219 Q?
U 1 1 5EB84D84
P 3450 5625
F 0 "Q?" H 3640 5671 50  0000 L CNN
F 1 "BC337" H 3650 5475 50  0000 L CNN
F 2 "Package_TO_SOT_THT:TO-39-3" H 3650 5550 50  0001 L CIN
F 3 "http://www.onsemi.com/pub_link/Collateral/2N2219-D.PDF" H 3450 5625 50  0001 L CNN
	1    3450 5625
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 5EB84D98
P 3250 5775
F 0 "R?" H 3180 5729 50  0000 R CNN
F 1 "56k" H 3180 5820 50  0000 R CNN
F 2 "" V 3180 5775 50  0001 C CNN
F 3 "~" H 3250 5775 50  0001 C CNN
	1    3250 5775
	-1   0    0    1   
$EndComp
Connection ~ 3250 5625
Wire Wire Line
	3250 5925 3250 6125
Text Label 8700 2300 2    60   ~ 0
mosfet_low_side
Text Label 8700 1500 2    60   ~ 0
plus_button
Text Label 8700 1600 2    60   ~ 0
minus_button
Text Label 8700 1700 2    60   ~ 0
var_select_button
Text Label 8700 1800 2    60   ~ 0
mode_button
Text Label 1875 4625 2    60   ~ 0
mosfet_high_side
Text Label 4150 5025 0    60   ~ 0
mosfet_gate
Text Label 1250 2775 2    60   ~ 0
mosfet_gate
Wire Wire Line
	1250 2775 1450 2775
$Sheet
S 7900 5675 1875 525 
U 5EC77F4C
F0 "Cooling" 50
F1 "Cooling.sch" 50
F2 "FanControl" I L 7900 5925 50 
F3 "ThermistorVcc" I L 7900 6050 50 
F4 "TemperatureMeasurement" I R 9775 5800 50 
F5 "Vsupply" I L 7900 5800 50 
$EndSheet
Text Label 7900 5925 2    60   ~ 0
fan_control
$Comp
L Device:LED D?
U 1 1 5EDBDAEF
P 7475 3550
F 0 "D?" V 7514 3433 50  0000 R CNN
F 1 "LED" V 7423 3433 50  0000 R CNN
F 2 "" H 7475 3550 50  0001 C CNN
F 3 "~" H 7475 3550 50  0001 C CNN
	1    7475 3550
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R?
U 1 1 5EDBF69D
P 7475 3850
F 0 "R?" H 7405 3804 50  0000 R CNN
F 1 "470" H 7405 3895 50  0000 R CNN
F 2 "" V 7405 3850 50  0001 C CNN
F 3 "~" H 7475 3850 50  0001 C CNN
	1    7475 3850
	-1   0    0    1   
$EndComp
Text Label 7350 4125 2    50   ~ 0
ConnectLedGnd
Wire Wire Line
	7475 4000 7475 4125
Wire Wire Line
	7475 4125 7350 4125
$Comp
L Connector:Conn_01x02_Female J?
U 1 1 5EDCB6EE
P 1850 6775
F 0 "J?" H 1878 6751 50  0000 L CNN
F 1 "XT60" H 1878 6660 50  0000 L CNN
F 2 "" H 1850 6775 50  0001 C CNN
F 3 "~" H 1850 6775 50  0001 C CNN
	1    1850 6775
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x02_Female J?
U 1 1 5EDCBFC8
P 1850 7150
F 0 "J?" H 1878 7126 50  0000 L CNN
F 1 "BananaConnector" H 1878 7035 50  0000 L CNN
F 2 "" H 1850 7150 50  0001 C CNN
F 3 "~" H 1850 7150 50  0001 C CNN
	1    1850 7150
	1    0    0    -1  
$EndComp
Text Label 1650 6775 2    50   ~ 0
RegulatedOut
Text Label 1650 7150 2    50   ~ 0
RegulatedOut
Wire Notes Line
	600  6450 600  7625
Wire Notes Line
	600  7625 2925 7625
Wire Notes Line
	2925 7625 2925 6450
Wire Notes Line
	2925 6450 600  6450
Text Notes 1225 6375 0    50   ~ 0
Power connectors
Text Label 3950 3100 0    50   ~ 0
ReturnOut
Text Label 1650 6875 2    50   ~ 0
ReturnOut
Text Label 1650 7250 2    50   ~ 0
ReturnOut
$Comp
L Device:R R?
U 1 1 5EE6415A
P 8425 3550
AR Path="/5EE6415A" Ref="R?"  Part="1" 
AR Path="/5EE32D8E/5EE6415A" Ref="R?"  Part="1" 
F 0 "R?" H 8355 3504 50  0000 R CNN
F 1 "180k" H 8355 3595 50  0000 R CNN
F 2 "" V 8355 3550 50  0001 C CNN
F 3 "~" H 8425 3550 50  0001 C CNN
	1    8425 3550
	-1   0    0    1   
$EndComp
$Comp
L Device:R R?
U 1 1 5EE64161
P 8425 3900
AR Path="/5EE64161" Ref="R?"  Part="1" 
AR Path="/5EE32D8E/5EE64161" Ref="R?"  Part="1" 
F 0 "R?" H 8355 3854 50  0000 R CNN
F 1 "5.6k" H 8355 3945 50  0000 R CNN
F 2 "" V 8355 3900 50  0001 C CNN
F 3 "~" H 8425 3900 50  0001 C CNN
	1    8425 3900
	-1   0    0    1   
$EndComp
Text Label 8500 4200 0    50   ~ 0
CommonLine
Wire Wire Line
	8425 4050 8425 4200
Wire Wire Line
	8425 4200 8500 4200
Wire Wire Line
	8425 3400 8425 3325
Wire Wire Line
	8425 3325 8500 3325
Wire Wire Line
	8125 3725 8125 3775
Wire Wire Line
	8125 4075 8125 4200
Wire Wire Line
	8425 3700 8425 3725
Connection ~ 8425 3725
Wire Wire Line
	8425 3725 8425 3750
Wire Wire Line
	8125 3725 8425 3725
Wire Wire Line
	8125 4200 8425 4200
Connection ~ 8425 4200
$Comp
L Device:R R?
U 1 1 5EE6417A
P 10000 3550
AR Path="/5EE6417A" Ref="R?"  Part="1" 
AR Path="/5EE32D8E/5EE6417A" Ref="R?"  Part="1" 
F 0 "R?" H 9930 3504 50  0000 R CNN
F 1 "150k" H 9930 3595 50  0000 R CNN
F 2 "" V 9930 3550 50  0001 C CNN
F 3 "~" H 10000 3550 50  0001 C CNN
	1    10000 3550
	-1   0    0    1   
$EndComp
$Comp
L Device:R R?
U 1 1 5EE64180
P 10000 3900
AR Path="/5EE64180" Ref="R?"  Part="1" 
AR Path="/5EE32D8E/5EE64180" Ref="R?"  Part="1" 
F 0 "R?" H 9930 3854 50  0000 R CNN
F 1 "5.6k" H 9930 3945 50  0000 R CNN
F 2 "" V 9930 3900 50  0001 C CNN
F 3 "~" H 10000 3900 50  0001 C CNN
	1    10000 3900
	-1   0    0    1   
$EndComp
Text Label 10075 4200 0    50   ~ 0
CommonLine
Wire Wire Line
	10000 4050 10000 4200
Wire Wire Line
	10000 4200 10075 4200
Wire Wire Line
	10000 3400 10000 3325
Wire Wire Line
	10000 3325 10075 3325
$Comp
L Device:CP1 C?
U 1 1 5EE6418B
P 9700 3925
AR Path="/5EE6418B" Ref="C?"  Part="1" 
AR Path="/5EE32D8E/5EE6418B" Ref="C?"  Part="1" 
F 0 "C?" H 9815 3971 50  0000 L CNN
F 1 "10µ" H 9815 3880 50  0000 L CNN
F 2 "" H 9700 3925 50  0001 C CNN
F 3 "~" H 9700 3925 50  0001 C CNN
	1    9700 3925
	1    0    0    -1  
$EndComp
Wire Wire Line
	9700 3725 9700 3775
Wire Wire Line
	9700 4075 9700 4200
Wire Wire Line
	10000 3700 10000 3725
Connection ~ 10000 3725
Wire Wire Line
	10000 3725 10000 3750
Wire Wire Line
	9700 3725 10000 3725
Wire Wire Line
	9700 4200 10000 4200
Connection ~ 10000 4200
Text Label 10075 3325 0    50   ~ 0
RegulatedOut
$Comp
L Device:CP1 C?
U 1 1 5EE6416C
P 8125 3925
AR Path="/5EE6416C" Ref="C?"  Part="1" 
AR Path="/5EE32D8E/5EE6416C" Ref="C?"  Part="1" 
F 0 "C?" H 8240 3971 50  0000 L CNN
F 1 "10µ" H 8240 3880 50  0000 L CNN
F 2 "" H 8125 3925 50  0001 C CNN
F 3 "~" H 8125 3925 50  0001 C CNN
	1    8125 3925
	1    0    0    -1  
$EndComp
Wire Wire Line
	8425 3725 8600 3725
Wire Wire Line
	10000 3725 10150 3725
Text Label 10150 3725 0    60   ~ 0
OutVoltageSensing
Text Label 8600 3725 0    60   ~ 0
SecondarySmoothed
Wire Notes Line
	8100 3100 11000 3100
Wire Notes Line
	11000 3100 11000 4300
Wire Notes Line
	11000 4300 8025 4300
Wire Notes Line
	8025 4300 8025 3100
Text Notes 9100 3050 0    50   ~ 0
Voltages measurement
Wire Wire Line
	7725 6050 7900 6050
Text Label 9775 5800 0    60   ~ 0
TemperatureMeasurement
Text Notes 725  3975 0    50   ~ 0
Power MOSFET transistors driver section with \nanti-latch protection (single input which controls\nboth sides of the mosfet driver section)
Wire Wire Line
	1425 3100 1925 3100
Connection ~ 1925 3100
Wire Wire Line
	1925 3100 2600 3100
Wire Wire Line
	1925 2925 1925 3100
Wire Notes Line
	4500 2000 4500 3550
Wire Notes Line
	4500 3550 550  3550
Wire Notes Line
	550  3550 550  2000
Wire Notes Line
	550  2000 4500 2000
Text Notes 1900 1975 0    50   ~ 0
Power section (Buck converter)
Wire Wire Line
	3550 5025 4150 5025
Wire Wire Line
	3550 6125 3550 5825
Wire Wire Line
	3250 6125 3400 6125
$Comp
L Connector_Generic:Conn_01x02 J?
U 1 1 5E873FAA
P 8225 4850
F 0 "J?" H 8305 4842 50  0000 L CNN
F 1 "Conn_01x02" H 8305 4751 50  0000 L CNN
F 2 "" H 8225 4850 50  0001 C CNN
F 3 "~" H 8225 4850 50  0001 C CNN
	1    8225 4850
	1    0    0    -1  
$EndComp
Text Label 8025 4950 2    50   ~ 0
CommonLine
$Comp
L Connector_Generic:Conn_01x02 J?
U 1 1 5E875461
P 9050 4850
F 0 "J?" H 8968 5067 50  0000 C CNN
F 1 "Conn_01x02" H 8968 4976 50  0000 C CNN
F 2 "" H 9050 4850 50  0001 C CNN
F 3 "~" H 9050 4850 50  0001 C CNN
	1    9050 4850
	-1   0    0    -1  
$EndComp
Text Label 9250 4950 0    50   ~ 0
CommonLine
Wire Notes Line
	8025 4525 9200 4525
Wire Notes Line
	9200 4525 9200 5275
Wire Notes Line
	9200 5275 8025 5275
Wire Notes Line
	8025 5275 8025 4525
Text Notes 7825 4475 0    50   ~ 0
External buck converter board 33V -> 5V
Text Label 9250 4850 0    50   ~ 0
Vcc(5v)
Text Label 10250 950  1    50   ~ 0
Vcc(5v)
$Comp
L Device:D D?
U 1 1 5E890948
P 4750 750
F 0 "D?" H 4750 534 50  0000 C CNN
F 1 "D" H 4750 625 50  0000 C CNN
F 2 "" H 4750 750 50  0001 C CNN
F 3 "~" H 4750 750 50  0001 C CNN
	1    4750 750 
	0    1    1    0   
$EndComp
Wire Wire Line
	4750 1000 4750 900 
Connection ~ 4750 1000
$Comp
L Device:CP1 C?
U 1 1 5E897965
P 4950 725
F 0 "C?" H 5065 771 50  0000 L CNN
F 1 "470µ" H 5065 680 50  0000 L CNN
F 2 "" H 4950 725 50  0001 C CNN
F 3 "~" H 4950 725 50  0001 C CNN
	1    4950 725 
	1    0    0    -1  
$EndComp
Wire Wire Line
	4750 600  4750 550 
Wire Wire Line
	4750 550  4950 550 
Wire Wire Line
	4950 550  4950 575 
$Comp
L power:Earth #PWR?
U 1 1 5E8B1977
P 4950 875
F 0 "#PWR?" H 4950 625 50  0001 C CNN
F 1 "Earth" H 4950 725 50  0001 C CNN
F 2 "" H 4950 875 50  0001 C CNN
F 3 "~" H 4950 875 50  0001 C CNN
	1    4950 875 
	1    0    0    -1  
$EndComp
Connection ~ 4950 550 
Wire Wire Line
	5350 550  5350 700 
Wire Wire Line
	5350 700  5750 700 
Wire Wire Line
	4950 550  5350 550 
Text Label 5750 700  0    50   ~ 0
MonoRectification(16V)
Text Label 2925 4050 2    50   ~ 0
MonoRectification(16V)
Text Label 8500 3325 0    50   ~ 0
MainPowerLine
Text Label 1100 2475 2    50   ~ 0
MainPowerLine
Text Label 8025 4850 2    50   ~ 0
MonoRectification(16V)
Text Label 7900 5800 2    50   ~ 0
MainPowerLine
Text Notes 4000 4725 0    50   ~ 0
Note : usually most of generic power mosfets are tolerant to \naround 20/30V between Gate and Source. \nIf so, you can use the secondary rectified smoothed line \nas the reference voltage to drive the mosfet gate.\nOtherwise, another power supply line (e.g. 12V line) \nwould be necessary and extra care will be required \nin the aim to drive mosfets in their safe area
$Comp
L Transistor_BJT:BC307 Q?
U 1 1 5E9096F9
P 3450 4400
F 0 "Q?" H 3641 4354 50  0000 L CNN
F 1 "BC327" H 3641 4445 50  0000 L CNN
F 2 "Package_TO_SOT_THT:TO-92_Inline" H 3650 4325 50  0001 L CIN
F 3 "http://www.onsemi.com/pub_link/Collateral/BC307-D.PDF" H 3450 4400 50  0001 L CNN
	1    3450 4400
	1    0    0    1   
$EndComp
$Comp
L Device:R R?
U 1 1 5E909F1B
P 3250 4250
F 0 "R?" H 3180 4204 50  0000 R CNN
F 1 "56k" H 3180 4295 50  0000 R CNN
F 2 "" V 3180 4250 50  0001 C CNN
F 3 "~" H 3250 4250 50  0001 C CNN
	1    3250 4250
	-1   0    0    1   
$EndComp
Wire Wire Line
	2925 4050 3250 4050
Wire Wire Line
	3250 4050 3250 4100
Connection ~ 3250 4050
Wire Wire Line
	3250 4050 3550 4050
$Comp
L Transistor_BJT:2N2219 Q?
U 1 1 5E918642
P 2675 4625
F 0 "Q?" H 2865 4671 50  0000 L CNN
F 1 "BC337" H 2875 4475 50  0000 L CNN
F 2 "Package_TO_SOT_THT:TO-39-3" H 2875 4550 50  0001 L CIN
F 3 "http://www.onsemi.com/pub_link/Collateral/2N2219-D.PDF" H 2675 4625 50  0001 L CNN
	1    2675 4625
	1    0    0    -1  
$EndComp
Wire Wire Line
	2775 4425 2775 4400
$Comp
L Device:R R?
U 1 1 5E920F2F
P 2275 4625
F 0 "R?" V 2068 4625 50  0000 C CNN
F 1 "1k" V 2159 4625 50  0000 C CNN
F 2 "" V 2205 4625 50  0001 C CNN
F 3 "~" H 2275 4625 50  0001 C CNN
	1    2275 4625
	0    1    1    0   
$EndComp
$Comp
L Device:R R?
U 1 1 5E9214BF
P 2475 4775
F 0 "R?" H 2405 4729 50  0000 R CNN
F 1 "56k" H 2405 4820 50  0000 R CNN
F 2 "" V 2405 4775 50  0001 C CNN
F 3 "~" H 2475 4775 50  0001 C CNN
	1    2475 4775
	-1   0    0    1   
$EndComp
Wire Wire Line
	2425 4625 2475 4625
Connection ~ 2475 4625
Wire Wire Line
	2475 4925 2775 4925
Wire Wire Line
	2775 4925 2775 4825
$Comp
L Device:R R?
U 1 1 5E94AF88
P 3100 4400
F 0 "R?" V 2893 4400 50  0000 C CNN
F 1 "1k" V 2984 4400 50  0000 C CNN
F 2 "" V 3030 4400 50  0001 C CNN
F 3 "~" H 3100 4400 50  0001 C CNN
	1    3100 4400
	0    1    1    0   
$EndComp
Connection ~ 3250 4400
Wire Wire Line
	2950 4400 2775 4400
$Comp
L Device:R R?
U 1 1 5E95C800
P 3100 5625
F 0 "R?" V 2893 5625 50  0000 C CNN
F 1 "1k" V 2984 5625 50  0000 C CNN
F 2 "" V 3030 5625 50  0001 C CNN
F 3 "~" H 3100 5625 50  0001 C CNN
	1    3100 5625
	0    1    1    0   
$EndComp
Text Label 7475 3400 0    50   ~ 0
Vcc(5v)
Wire Wire Line
	2125 4625 1875 4625
Connection ~ 3550 5025
Wire Wire Line
	3550 5025 3550 5425
Wire Wire Line
	3550 4600 3550 5025
Wire Wire Line
	5300 1000 5300 1525
Wire Wire Line
	5300 1525 5100 1525
Wire Wire Line
	4750 1000 5300 1000
Connection ~ 5100 1525
Wire Wire Line
	5100 1225 5100 950 
Wire Wire Line
	5100 950  5725 950 
Connection ~ 5100 1225
Connection ~ 5725 950 
$Comp
L Switch:SW_DPST SW?
U 1 1 5EDB4B06
P 5150 2175
F 0 "SW?" H 5150 2500 50  0000 C CNN
F 1 "SW_DPST" H 5150 2409 50  0000 C CNN
F 2 "" H 5150 2175 50  0001 C CNN
F 3 "~" H 5150 2175 50  0001 C CNN
	1    5150 2175
	1    0    0    -1  
$EndComp
Wire Wire Line
	3975 2475 4350 2475
Connection ~ 2475 4925
$Comp
L power:Earth #PWR?
U 1 1 5E9291D2
P 2475 4925
F 0 "#PWR?" H 2475 4675 50  0001 C CNN
F 1 "Earth" H 2475 4775 50  0001 C CNN
F 2 "" H 2475 4925 50  0001 C CNN
F 3 "~" H 2475 4925 50  0001 C CNN
	1    2475 4925
	1    0    0    -1  
$EndComp
$Comp
L Transistor_BJT:2N2219 Q?
U 1 1 5E832B19
P 2125 5100
F 0 "Q?" H 2315 5146 50  0000 L CNN
F 1 "BC337" H 2325 4950 50  0000 L CNN
F 2 "Package_TO_SOT_THT:TO-39-3" H 2325 5025 50  0001 L CIN
F 3 "http://www.onsemi.com/pub_link/Collateral/2N2219-D.PDF" H 2125 5100 50  0001 L CNN
	1    2125 5100
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 5E832B1F
P 1925 5250
F 0 "R?" H 1855 5204 50  0000 R CNN
F 1 "56k" H 1855 5295 50  0000 R CNN
F 2 "" V 1855 5250 50  0001 C CNN
F 3 "~" H 1925 5250 50  0001 C CNN
	1    1925 5250
	-1   0    0    1   
$EndComp
Connection ~ 1925 5100
$Comp
L Device:R R?
U 1 1 5E832B27
P 1775 5100
F 0 "R?" V 1568 5100 50  0000 C CNN
F 1 "1k" V 1659 5100 50  0000 C CNN
F 2 "" V 1705 5100 50  0001 C CNN
F 3 "~" H 1775 5100 50  0001 C CNN
	1    1775 5100
	0    1    1    0   
$EndComp
Wire Wire Line
	1450 5100 1600 5100
Wire Wire Line
	1925 5400 2075 5400
Wire Wire Line
	2225 5400 2225 5300
Wire Wire Line
	2225 4900 2075 4900
Wire Wire Line
	2075 4900 2075 4375
Wire Wire Line
	2075 4375 2475 4375
Wire Wire Line
	2475 4375 2475 4625
$Comp
L power:Earth #PWR?
U 1 1 5E85A54C
P 2075 5400
F 0 "#PWR?" H 2075 5150 50  0001 C CNN
F 1 "Earth" H 2075 5250 50  0001 C CNN
F 2 "" H 2075 5400 50  0001 C CNN
F 3 "~" H 2075 5400 50  0001 C CNN
	1    2075 5400
	1    0    0    -1  
$EndComp
Connection ~ 2075 5400
Wire Wire Line
	2075 5400 2225 5400
Wire Wire Line
	1600 5100 1600 5625
Wire Wire Line
	1600 5625 2950 5625
Connection ~ 1600 5100
Wire Wire Line
	1600 5100 1625 5100
Text Label 1450 5100 2    60   ~ 0
mosfet_low_side
Text Label 8700 2200 2    60   ~ 0
mosfet_high_side
$Comp
L power:Earth #PWR?
U 1 1 5E8C0212
P 3400 6125
F 0 "#PWR?" H 3400 5875 50  0001 C CNN
F 1 "Earth" H 3400 5975 50  0001 C CNN
F 2 "" H 3400 6125 50  0001 C CNN
F 3 "~" H 3400 6125 50  0001 C CNN
	1    3400 6125
	1    0    0    -1  
$EndComp
Connection ~ 3400 6125
Wire Wire Line
	3400 6125 3550 6125
$Sheet
S 4150 6600 1850 475 
U 5E85D9C1
F0 "UserInterface" 50
F1 "UserInterface.sch" 50
F2 "minus_button" I L 4150 6800 50 
F3 "plus_button" I L 4150 6700 50 
F4 "variable_select_button" I L 4150 7000 50 
F5 "mode_button" I L 4150 6900 50 
F6 "Vcc(5V)" I R 6000 6925 50 
F7 "i2c_sda" I R 6000 6800 50 
F8 "i2c_scl" I R 6000 6675 50 
$EndSheet
Text Label 4150 6700 2    60   ~ 0
plus_button
Text Label 4150 6800 2    60   ~ 0
minus_button
Text Label 4150 7000 2    60   ~ 0
var_select_button
Text Label 4150 6900 2    60   ~ 0
mode_button
Text Label 6000 6675 0    60   ~ 0
I2C_SCL
Text Label 6000 6800 0    60   ~ 0
I2C_SDA
Text Label 6000 6925 0    50   ~ 0
Vcc(5v)
$Comp
L Device:Transformer_1P_2S T?
U 1 1 5E86D6B6
P 3450 1200
F 0 "T?" H 3450 1781 50  0000 C CNN
F 1 "Transformer_1P_2S" H 3450 1690 50  0000 C CNN
F 2 "" H 3450 1200 50  0001 C CNN
F 3 "~" H 3450 1200 50  0001 C CNN
	1    3450 1200
	1    0    0    -1  
$EndComp
Connection ~ 2675 1825
Connection ~ 4800 1825
Wire Wire Line
	2675 1825 4800 1825
Wire Wire Line
	3850 1100 3950 1100
Wire Wire Line
	3950 1100 3950 1400
Wire Wire Line
	3950 1600 3850 1600
Wire Wire Line
	3850 1300 4000 1300
Wire Wire Line
	4000 1300 4000 1000
Wire Wire Line
	4000 800  3850 800 
Wire Wire Line
	4000 1000 4750 1000
Connection ~ 4000 1000
Wire Wire Line
	4000 1000 4000 800 
Wire Wire Line
	3950 1400 4400 1400
Wire Wire Line
	4400 1400 4400 1225
Wire Wire Line
	4400 1225 4800 1225
Connection ~ 3950 1400
Wire Wire Line
	3950 1400 3950 1600
Text Notes 600  2200 0    39   ~ 0
Note : mosfet could either be of N or P type\nN type will not get until full potential but is often \ncheaper, P type can maximise output potential (vout = vin)
Wire Notes Line
	675  4850 2400 4850
Wire Notes Line
	2400 4850 2400 5875
Wire Notes Line
	2400 5875 675  5875
Wire Notes Line
	675  5875 675  4850
Text Notes 700  5875 0    31   ~ 0
Anti collision protection circuit:\nwhen both high and low lines are triggered, \nthis transistor pulls back to ground the base of high side \nnpn driver, resulting in the extinction of high side driver, \nleaving low side driver on alone (forces N mosfets to close.\nMust reverse logic if P Mosfet is used instead of a N one)
Text Label 7725 6050 2    50   ~ 0
Vcc(5v)
Text Notes 7575 2200 0    50   ~ 0
OC1A
Text Notes 7575 2300 0    50   ~ 0
OC2A
Text Notes 7575 1900 0    50   ~ 0
OC0A
Wire Notes Line
	7825 1875 8175 1875
Wire Notes Line
	7775 2175 7900 2175
Wire Notes Line
	7800 2275 7925 2275
$EndSCHEMATC
