EESchema Schematic File Version 4
LIBS:LabBenchPowerSupply-cache
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 2
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
A4
Text Label 10550 1700 0    60   ~ 0
A5
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
L Device:Transformer_1P_SS T?
U 1 1 5E79C1FF
P 3450 1200
F 0 "T?" H 3450 1581 50  0000 C CNN
F 1 "Transformer_1P_SS" H 3450 1490 50  0000 C CNN
F 2 "" H 3450 1200 50  0001 C CNN
F 3 "~" H 3450 1200 50  0001 C CNN
	1    3450 1200
	1    0    0    -1  
$EndComp
NoConn ~ 3850 1200
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
P 4550 1825
F 0 "#PWR?" H 4550 1575 50  0001 C CNN
F 1 "Earth" H 4550 1675 50  0001 C CNN
F 2 "" H 4550 1825 50  0001 C CNN
F 3 "~" H 4550 1825 50  0001 C CNN
	1    4550 1825
	1    0    0    -1  
$EndComp
Connection ~ 4550 1825
$Comp
L Device:CP1 C?
U 1 1 5E7AB2AF
P 5575 1400
F 0 "C?" H 5690 1446 50  0000 L CNN
F 1 "470µ" H 5690 1355 50  0000 L CNN
F 2 "" H 5575 1400 50  0001 C CNN
F 3 "~" H 5575 1400 50  0001 C CNN
	1    5575 1400
	1    0    0    -1  
$EndComp
Text Notes 2625 650  0    50   ~ 0
220 V AC / 2x11.5 serie = 23V AC
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
Connection ~ 2675 1825
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
P 5225 1400
F 0 "C?" H 5340 1446 50  0000 L CNN
F 1 "470µ" H 5340 1355 50  0000 L CNN
F 2 "" H 5225 1400 50  0001 C CNN
F 3 "~" H 5225 1400 50  0001 C CNN
	1    5225 1400
	1    0    0    -1  
$EndComp
Wire Wire Line
	5575 1825 5575 1550
Wire Wire Line
	4550 1825 5225 1825
Wire Wire Line
	5575 1250 5575 950 
Wire Wire Line
	5225 1250 5225 950 
Connection ~ 5225 950 
Wire Wire Line
	5225 950  5575 950 
Wire Wire Line
	5225 1550 5225 1825
Connection ~ 5225 1825
Wire Wire Line
	5225 1825 5575 1825
Wire Wire Line
	5575 950  5800 950 
Connection ~ 5575 950 
$Comp
L Device:D D?
U 1 1 5E7FB0DA
P 4300 1375
F 0 "D?" H 4300 1159 50  0000 C CNN
F 1 "D" H 4300 1250 50  0000 C CNN
F 2 "" H 4300 1375 50  0001 C CNN
F 3 "~" H 4300 1375 50  0001 C CNN
	1    4300 1375
	0    1    1    0   
$EndComp
$Comp
L Device:D D?
U 1 1 5E7FBA05
P 4450 1225
F 0 "D?" V 4496 1146 50  0000 R CNN
F 1 "D" V 4405 1146 50  0000 R CNN
F 2 "" H 4450 1225 50  0001 C CNN
F 3 "~" H 4450 1225 50  0001 C CNN
	1    4450 1225
	-1   0    0    1   
$EndComp
$Comp
L Device:D D?
U 1 1 5E7FBD99
P 4600 1375
F 0 "D?" H 4600 1159 50  0000 C CNN
F 1 "D" H 4600 1250 50  0000 C CNN
F 2 "" H 4600 1375 50  0001 C CNN
F 3 "~" H 4600 1375 50  0001 C CNN
	1    4600 1375
	0    1    1    0   
$EndComp
$Comp
L Device:D D?
U 1 1 5E7FC2B4
P 4450 1525
F 0 "D?" V 4496 1446 50  0000 R CNN
F 1 "D" V 4405 1446 50  0000 R CNN
F 2 "" H 4450 1525 50  0001 C CNN
F 3 "~" H 4450 1525 50  0001 C CNN
	1    4450 1525
	-1   0    0    1   
$EndComp
Wire Wire Line
	2675 1825 4300 1825
Wire Wire Line
	4300 1525 4300 1825
Connection ~ 4300 1525
Connection ~ 4300 1825
Wire Wire Line
	4300 1825 4550 1825
Wire Wire Line
	4025 1400 4025 1225
Wire Wire Line
	4025 1225 4300 1225
Connection ~ 4300 1225
Wire Wire Line
	4600 1000 4600 1225
Connection ~ 4600 1225
Wire Wire Line
	4600 1525 4725 1525
Wire Wire Line
	4725 1525 4725 950 
Connection ~ 4600 1525
Wire Wire Line
	4725 950  5225 950 
Wire Wire Line
	3850 1400 4025 1400
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
F 1 "L" V 2324 2475 50  0000 C CNN
F 2 "" H 2225 2475 50  0001 C CNN
F 3 "~" H 2225 2475 50  0001 C CNN
	1    2225 2475
	0    -1   -1   0   
$EndComp
Wire Wire Line
	1925 2475 2075 2475
Connection ~ 1925 2475
Wire Wire Line
	5575 1825 5825 1825
Connection ~ 5575 1825
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
Text Label 5800 950  0    50   ~ 0
23v_smoothed
Text Label 3950 2475 0    50   ~ 0
RegulatedOut
Text Label 5825 1825 0    50   ~ 0
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
	2475 4050 2475 4200
$Comp
L Transistor_BJT:2N2219 Q?
U 1 1 5EB84D84
P 2375 5625
F 0 "Q?" H 2565 5671 50  0000 L CNN
F 1 "2N2222" H 2575 5475 50  0000 L CNN
F 2 "Package_TO_SOT_THT:TO-39-3" H 2575 5550 50  0001 L CIN
F 3 "http://www.onsemi.com/pub_link/Collateral/2N2219-D.PDF" H 2375 5625 50  0001 L CNN
	1    2375 5625
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 5EB84D98
P 2175 5775
F 0 "R?" H 2105 5729 50  0000 R CNN
F 1 "56k" H 2105 5820 50  0000 R CNN
F 2 "" V 2105 5775 50  0001 C CNN
F 3 "~" H 2175 5775 50  0001 C CNN
	1    2175 5775
	-1   0    0    1   
$EndComp
Connection ~ 2175 5625
Wire Wire Line
	2175 5925 2175 6125
Wire Wire Line
	3775 2475 3950 2475
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
Text Label 675  5025 2    60   ~ 0
mosfet_low_side
Text Label 3075 5025 0    60   ~ 0
buck_mosfet
Text Label 1250 2775 2    60   ~ 0
buck_mosfet
Wire Wire Line
	1250 2775 1450 2775
$Sheet
S 3600 6475 1875 525 
U 5EC77F4C
F0 "Cooling" 50
F1 "Cooling.sch" 50
F2 "FanControl" I L 3600 6725 50 
F3 "ThermistorVcc" I L 3600 6850 50 
F4 "TemperatureMeasurement" I R 5475 6600 50 
F5 "Vsupply" I L 3600 6600 50 
$EndSheet
Text Label 3600 6725 2    60   ~ 0
fan_control
$Comp
L Switch:SW_DPST SW?
U 1 1 5EDB4B06
P 3575 2375
F 0 "SW?" H 3575 2700 50  0000 C CNN
F 1 "SW_DPST" H 3575 2609 50  0000 C CNN
F 2 "" H 3575 2375 50  0001 C CNN
F 3 "~" H 3575 2375 50  0001 C CNN
	1    3575 2375
	1    0    0    -1  
$EndComp
Text Label 3775 2275 0    50   ~ 0
CommonLine
Text Label 3375 2275 2    50   ~ 0
ConnectLedGnd
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
F 1 "220k" H 8355 3595 50  0000 R CNN
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
F 1 "10k" H 8355 3945 50  0000 R CNN
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
$Comp
L power:+3V3 #PWR?
U 1 1 5EFD4716
P 3425 6850
F 0 "#PWR?" H 3425 6700 50  0001 C CNN
F 1 "+3.3V" V 3425 7050 28  0000 C CNN
F 2 "" H 3425 6850 50  0000 C CNN
F 3 "" H 3425 6850 50  0000 C CNN
	1    3425 6850
	0    -1   -1   0   
$EndComp
Wire Wire Line
	3425 6850 3600 6850
Text Label 5475 6600 0    60   ~ 0
TemperatureMeasurement
Text Notes 825  3925 0    50   ~ 0
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
	2475 5025 3075 5025
Text Label 1950 6125 2    50   ~ 0
CommonLine
Wire Wire Line
	1950 6125 2175 6125
Wire Wire Line
	2475 6125 2475 5825
Connection ~ 2175 6125
Wire Wire Line
	2175 6125 2475 6125
Text Notes 3950 975  0    50   ~ 0
23V
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
External buck converter board 23 -> 5V
Text Label 9250 4850 0    50   ~ 0
Vcc(5v)
Text Label 10250 950  1    50   ~ 0
Vcc(5v)
Wire Wire Line
	3850 1000 4250 1000
$Comp
L Device:D D?
U 1 1 5E890948
P 4250 750
F 0 "D?" H 4250 534 50  0000 C CNN
F 1 "D" H 4250 625 50  0000 C CNN
F 2 "" H 4250 750 50  0001 C CNN
F 3 "~" H 4250 750 50  0001 C CNN
	1    4250 750 
	0    1    1    0   
$EndComp
Wire Wire Line
	4250 1000 4250 900 
Connection ~ 4250 1000
Wire Wire Line
	4250 1000 4600 1000
$Comp
L Device:CP1 C?
U 1 1 5E897965
P 4450 725
F 0 "C?" H 4565 771 50  0000 L CNN
F 1 "470µ" H 4565 680 50  0000 L CNN
F 2 "" H 4450 725 50  0001 C CNN
F 3 "~" H 4450 725 50  0001 C CNN
	1    4450 725 
	1    0    0    -1  
$EndComp
Wire Wire Line
	4250 600  4250 550 
Wire Wire Line
	4250 550  4450 550 
Wire Wire Line
	4450 550  4450 575 
$Comp
L power:Earth #PWR?
U 1 1 5E8B1977
P 4450 875
F 0 "#PWR?" H 4450 625 50  0001 C CNN
F 1 "Earth" H 4450 725 50  0001 C CNN
F 2 "" H 4450 875 50  0001 C CNN
F 3 "~" H 4450 875 50  0001 C CNN
	1    4450 875 
	1    0    0    -1  
$EndComp
Connection ~ 4450 550 
Wire Wire Line
	4850 550  4850 700 
Wire Wire Line
	4850 700  5250 700 
Wire Wire Line
	4450 550  4850 550 
Text Label 5250 700  0    50   ~ 0
10v_smoothed
Text Label 1850 4050 2    50   ~ 0
10v_smoothed
Text Label 8500 3325 0    50   ~ 0
23v_smoothed
Text Label 1100 2475 2    50   ~ 0
23v_smoothed
Text Label 8025 4850 2    50   ~ 0
10v_smoothed
Text Label 3600 6600 2    50   ~ 0
23v_smoothed
Text Notes 2775 4250 0    50   ~ 0
Note : usually most of generic power mosfets are tolerant to \n30V between Gate and Source. If so, you can either connect the \n23V smoothed line instead of the 10V one, as a power line to \ncontrol the mosfet.
$Comp
L Transistor_BJT:BC307 Q?
U 1 1 5E9096F9
P 2375 4400
F 0 "Q?" H 2566 4354 50  0000 L CNN
F 1 "BC307" H 2566 4445 50  0000 L CNN
F 2 "Package_TO_SOT_THT:TO-92_Inline" H 2575 4325 50  0001 L CIN
F 3 "http://www.onsemi.com/pub_link/Collateral/BC307-D.PDF" H 2375 4400 50  0001 L CNN
	1    2375 4400
	1    0    0    1   
$EndComp
$Comp
L Device:R R?
U 1 1 5E909F1B
P 2175 4250
F 0 "R?" H 2105 4204 50  0000 R CNN
F 1 "56k" H 2105 4295 50  0000 R CNN
F 2 "" V 2105 4250 50  0001 C CNN
F 3 "~" H 2175 4250 50  0001 C CNN
	1    2175 4250
	-1   0    0    1   
$EndComp
Wire Wire Line
	1850 4050 2175 4050
Wire Wire Line
	2175 4050 2175 4100
Connection ~ 2175 4050
Wire Wire Line
	2175 4050 2475 4050
$Comp
L Transistor_BJT:2N2219 Q?
U 1 1 5E918642
P 1600 4625
F 0 "Q?" H 1790 4671 50  0000 L CNN
F 1 "2N2222" H 1800 4475 50  0000 L CNN
F 2 "Package_TO_SOT_THT:TO-39-3" H 1800 4550 50  0001 L CIN
F 3 "http://www.onsemi.com/pub_link/Collateral/2N2219-D.PDF" H 1600 4625 50  0001 L CNN
	1    1600 4625
	1    0    0    -1  
$EndComp
Wire Wire Line
	1700 4425 1700 4400
$Comp
L Device:R R?
U 1 1 5E920F2F
P 1200 4625
F 0 "R?" V 993 4625 50  0000 C CNN
F 1 "1k" V 1084 4625 50  0000 C CNN
F 2 "" V 1130 4625 50  0001 C CNN
F 3 "~" H 1200 4625 50  0001 C CNN
	1    1200 4625
	0    1    1    0   
$EndComp
$Comp
L Device:R R?
U 1 1 5E9214BF
P 1400 4775
F 0 "R?" H 1330 4729 50  0000 R CNN
F 1 "56k" H 1330 4820 50  0000 R CNN
F 2 "" V 1330 4775 50  0001 C CNN
F 3 "~" H 1400 4775 50  0001 C CNN
	1    1400 4775
	-1   0    0    1   
$EndComp
Wire Wire Line
	1350 4625 1400 4625
Connection ~ 1400 4625
$Comp
L power:Earth #PWR?
U 1 1 5E9291D2
P 1400 4925
F 0 "#PWR?" H 1400 4675 50  0001 C CNN
F 1 "Earth" H 1400 4775 50  0001 C CNN
F 2 "" H 1400 4925 50  0001 C CNN
F 3 "~" H 1400 4925 50  0001 C CNN
	1    1400 4925
	1    0    0    -1  
$EndComp
Wire Wire Line
	1400 4925 1700 4925
Wire Wire Line
	1700 4925 1700 4825
Connection ~ 1400 4925
$Comp
L Transistor_BJT:BC307 Q?
U 1 1 5E949A66
P 1600 5425
F 0 "Q?" H 1791 5379 50  0000 L CNN
F 1 "BC307" H 1791 5470 50  0000 L CNN
F 2 "Package_TO_SOT_THT:TO-92_Inline" H 1800 5350 50  0001 L CIN
F 3 "http://www.onsemi.com/pub_link/Collateral/BC307-D.PDF" H 1600 5425 50  0001 L CNN
	1    1600 5425
	1    0    0    1   
$EndComp
$Comp
L Device:R R?
U 1 1 5E94AF88
P 2025 4400
F 0 "R?" V 1818 4400 50  0000 C CNN
F 1 "1k" V 1909 4400 50  0000 C CNN
F 2 "" V 1955 4400 50  0001 C CNN
F 3 "~" H 2025 4400 50  0001 C CNN
	1    2025 4400
	0    1    1    0   
$EndComp
Connection ~ 2175 4400
Wire Wire Line
	1875 4400 1700 4400
$Comp
L Device:R R?
U 1 1 5E95C800
P 2025 5625
F 0 "R?" V 1818 5625 50  0000 C CNN
F 1 "1k" V 1909 5625 50  0000 C CNN
F 2 "" V 1955 5625 50  0001 C CNN
F 3 "~" H 2025 5625 50  0001 C CNN
	1    2025 5625
	0    1    1    0   
$EndComp
Wire Wire Line
	1700 5625 1875 5625
$Comp
L Device:R R?
U 1 1 5E974F87
P 1400 5275
F 0 "R?" H 1330 5229 50  0000 R CNN
F 1 "56k" H 1330 5320 50  0000 R CNN
F 2 "" V 1330 5275 50  0001 C CNN
F 3 "~" H 1400 5275 50  0001 C CNN
	1    1400 5275
	-1   0    0    1   
$EndComp
Wire Wire Line
	1250 5125 1400 5125
Wire Wire Line
	1700 5125 1700 5225
Connection ~ 1400 5125
Wire Wire Line
	1400 5125 1700 5125
Text Label 7475 3400 0    50   ~ 0
Vcc(5v)
Text Label 1250 5125 2    50   ~ 0
Vcc(5v)
$Comp
L Device:R R?
U 1 1 5E984F36
P 1250 5425
F 0 "R?" V 1043 5425 50  0000 C CNN
F 1 "1k" V 1134 5425 50  0000 C CNN
F 2 "" V 1180 5425 50  0001 C CNN
F 3 "~" H 1250 5425 50  0001 C CNN
	1    1250 5425
	0    1    1    0   
$EndComp
Connection ~ 1400 5425
Wire Wire Line
	1050 4625 800  4625
Wire Wire Line
	800  5425 1100 5425
Wire Wire Line
	675  5025 800  5025
Wire Wire Line
	800  4625 800  5025
Connection ~ 800  5025
Wire Wire Line
	800  5025 800  5425
Connection ~ 2475 5025
Wire Wire Line
	2475 5025 2475 5425
Wire Wire Line
	2475 4600 2475 5025
$EndSCHEMATC
