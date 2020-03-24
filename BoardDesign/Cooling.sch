EESchema Schematic File Version 4
LIBS:LabBenchPowerSupply-cache
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 2 2
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
L Motor:Fan M?
U 1 1 5EC94014
P 3550 1625
AR Path="/5EC94014" Ref="M?"  Part="1" 
AR Path="/5EC77F4C/5EC94014" Ref="M?"  Part="1" 
F 0 "M?" H 3708 1721 50  0000 L CNN
F 1 "Fan" H 3708 1630 50  0000 L CNN
F 2 "" H 3550 1635 50  0001 C CNN
F 3 "~" H 3550 1635 50  0001 C CNN
	1    3550 1625
	1    0    0    -1  
$EndComp
Wire Wire Line
	3900 1325 3900 1125
Connection ~ 3900 1325
Wire Wire Line
	3900 1325 4225 1325
$Comp
L Transistor_BJT:2N2219 Q?
U 1 1 5EC9401F
P 3450 2025
AR Path="/5EC9401F" Ref="Q?"  Part="1" 
AR Path="/5EC77F4C/5EC9401F" Ref="Q?"  Part="1" 
F 0 "Q?" H 3640 2071 50  0000 L CNN
F 1 "2N2222" H 3650 1875 50  0000 L CNN
F 2 "Package_TO_SOT_THT:TO-39-3" H 3650 1950 50  0001 L CIN
F 3 "http://www.onsemi.com/pub_link/Collateral/2N2219-D.PDF" H 3450 2025 50  0001 L CNN
	1    3450 2025
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 5EC94025
P 2950 2025
AR Path="/5EC94025" Ref="R?"  Part="1" 
AR Path="/5EC77F4C/5EC94025" Ref="R?"  Part="1" 
F 0 "R?" V 2743 2025 50  0000 C CNN
F 1 "1k" V 2834 2025 50  0000 C CNN
F 2 "" V 2880 2025 50  0001 C CNN
F 3 "~" H 2950 2025 50  0001 C CNN
	1    2950 2025
	0    1    1    0   
$EndComp
$Comp
L Device:R R?
U 1 1 5EC9402B
P 3175 2250
AR Path="/5EC9402B" Ref="R?"  Part="1" 
AR Path="/5EC77F4C/5EC9402B" Ref="R?"  Part="1" 
F 0 "R?" H 3105 2204 50  0000 R CNN
F 1 "56k" H 3105 2295 50  0000 R CNN
F 2 "" V 3105 2250 50  0001 C CNN
F 3 "~" H 3175 2250 50  0001 C CNN
	1    3175 2250
	-1   0    0    1   
$EndComp
Wire Wire Line
	3100 2025 3175 2025
Wire Wire Line
	3175 2025 3175 2100
Wire Wire Line
	3175 2025 3250 2025
Connection ~ 3175 2025
Wire Wire Line
	3175 2400 3550 2400
Wire Wire Line
	3550 2225 3550 2400
$Comp
L Device:R R?
U 1 1 5EC94037
P 4825 2025
AR Path="/5EC94037" Ref="R?"  Part="1" 
AR Path="/5EC77F4C/5EC94037" Ref="R?"  Part="1" 
F 0 "R?" V 4618 2025 50  0000 C CNN
F 1 "1k" V 4709 2025 50  0000 C CNN
F 2 "" V 4755 2025 50  0001 C CNN
F 3 "~" H 4825 2025 50  0001 C CNN
	1    4825 2025
	0    -1   1    0   
$EndComp
$Comp
L Device:R R?
U 1 1 5EC9403D
P 4600 2250
AR Path="/5EC9403D" Ref="R?"  Part="1" 
AR Path="/5EC77F4C/5EC9403D" Ref="R?"  Part="1" 
F 0 "R?" H 4530 2204 50  0000 R CNN
F 1 "56k" H 4530 2295 50  0000 R CNN
F 2 "" V 4530 2250 50  0001 C CNN
F 3 "~" H 4600 2250 50  0001 C CNN
	1    4600 2250
	1    0    0    1   
$EndComp
Wire Wire Line
	4675 2025 4600 2025
Wire Wire Line
	4600 2025 4600 2100
Wire Wire Line
	4600 2025 4525 2025
Connection ~ 4600 2025
Wire Wire Line
	4600 2400 4225 2400
Wire Wire Line
	4225 2225 4225 2400
Wire Wire Line
	3550 2400 3900 2400
Connection ~ 3550 2400
Connection ~ 4225 2400
$Comp
L power:Earth #PWR?
U 1 1 5EC9404C
P 3900 2400
AR Path="/5EC9404C" Ref="#PWR?"  Part="1" 
AR Path="/5EC77F4C/5EC9404C" Ref="#PWR?"  Part="1" 
F 0 "#PWR?" H 3900 2150 50  0001 C CNN
F 1 "Earth" H 3900 2250 50  0001 C CNN
F 2 "" H 3900 2400 50  0001 C CNN
F 3 "~" H 3900 2400 50  0001 C CNN
	1    3900 2400
	1    0    0    -1  
$EndComp
Connection ~ 3900 2400
Wire Wire Line
	3900 2400 4225 2400
Wire Wire Line
	4975 2025 4975 2600
Wire Wire Line
	4975 2600 2800 2600
Wire Wire Line
	2800 2600 2800 2025
Wire Wire Line
	2800 2025 2550 2025
Connection ~ 2800 2025
$Comp
L Device:D D?
U 1 1 5EC9405A
P 4500 1600
AR Path="/5EC9405A" Ref="D?"  Part="1" 
AR Path="/5EC77F4C/5EC9405A" Ref="D?"  Part="1" 
F 0 "D?" V 4454 1679 50  0000 L CNN
F 1 "D" V 4545 1679 50  0000 L CNN
F 2 "" H 4500 1600 50  0001 C CNN
F 3 "~" H 4500 1600 50  0001 C CNN
	1    4500 1600
	0    1    1    0   
$EndComp
$Comp
L Motor:Fan M?
U 1 1 5EC94060
P 4225 1625
AR Path="/5EC94060" Ref="M?"  Part="1" 
AR Path="/5EC77F4C/5EC94060" Ref="M?"  Part="1" 
F 0 "M?" H 4383 1721 50  0000 L CNN
F 1 "Fan" H 4383 1630 50  0000 L CNN
F 2 "" H 4225 1635 50  0001 C CNN
F 3 "~" H 4225 1635 50  0001 C CNN
	1    4225 1625
	-1   0    0    -1  
$EndComp
Wire Wire Line
	4500 1450 4500 1325
Wire Wire Line
	4500 1325 4225 1325
Connection ~ 4225 1325
Wire Wire Line
	4225 1825 4500 1825
Connection ~ 4225 1825
$Comp
L Transistor_BJT:2N2219 Q?
U 1 1 5EC9406B
P 4325 2025
AR Path="/5EC9406B" Ref="Q?"  Part="1" 
AR Path="/5EC77F4C/5EC9406B" Ref="Q?"  Part="1" 
F 0 "Q?" H 4516 2071 50  0000 L CNN
F 1 "2N2222" H 4516 1980 50  0000 L CNN
F 2 "Package_TO_SOT_THT:TO-39-3" H 4525 1950 50  0001 L CIN
F 3 "http://www.onsemi.com/pub_link/Collateral/2N2219-D.PDF" H 4325 2025 50  0001 L CNN
	1    4325 2025
	-1   0    0    -1  
$EndComp
Wire Wire Line
	4500 1750 4500 1825
$Comp
L Device:D D?
U 1 1 5EC94072
P 3275 1600
AR Path="/5EC94072" Ref="D?"  Part="1" 
AR Path="/5EC77F4C/5EC94072" Ref="D?"  Part="1" 
F 0 "D?" V 3229 1679 50  0000 L CNN
F 1 "D" V 3320 1679 50  0000 L CNN
F 2 "" H 3275 1600 50  0001 C CNN
F 3 "~" H 3275 1600 50  0001 C CNN
	1    3275 1600
	0    1    1    0   
$EndComp
Wire Wire Line
	3550 1825 3275 1825
Wire Wire Line
	3275 1825 3275 1750
Connection ~ 3550 1825
Wire Wire Line
	3275 1450 3275 1325
Wire Wire Line
	3275 1325 3550 1325
Connection ~ 3550 1325
Wire Wire Line
	3550 1325 3900 1325
Text Notes 4825 1675 0    50   ~ 0
Don't forget freewheel diodes!\n50V+ peak might occur at \ntransistors collectors otherwise
Text Notes 2600 3025 0    50   ~ 0
Base resistor to ground references the transistor to a\nknown 'low' voltage, preventing it to auto-start due to parasitic current flow running between collector and emitter\n(collector-base diode is reversed polarised but a small current is allowed to flow in, thus activating \nthe transistor effect.)
Text HLabel 3900 1125 0    50   Input ~ 0
12VSmoothed
Text HLabel 2550 2025 0    50   Input ~ 0
FanControl
$Comp
L Device:Thermistor_NTC TH?
U 1 1 5EF9CC94
P 8525 2150
F 0 "TH?" H 8623 2196 50  0000 L CNN
F 1 "Thermistor_NTC" H 8623 2105 50  0000 L CNN
F 2 "" H 8525 2200 50  0001 C CNN
F 3 "~" H 8525 2200 50  0001 C CNN
	1    8525 2150
	1    0    0    -1  
$EndComp
Wire Wire Line
	8525 2300 8525 2400
$Comp
L Device:R R?
U 1 1 5EF9CCA2
P 8525 1775
F 0 "R?" H 8455 1729 50  0000 R CNN
F 1 "270k" H 8455 1820 50  0000 R CNN
F 2 "" V 8455 1775 50  0001 C CNN
F 3 "~" H 8525 1775 50  0001 C CNN
	1    8525 1775
	-1   0    0    1   
$EndComp
Wire Wire Line
	8525 1925 8525 1975
Wire Wire Line
	8525 1975 8775 1975
Connection ~ 8525 1975
Wire Wire Line
	8525 1975 8525 2000
$Comp
L Device:CP1 C?
U 1 1 5EF9CCAC
P 8125 2175
AR Path="/5EF9CCAC" Ref="C?"  Part="1" 
AR Path="/5EE32D8E/5EF9CCAC" Ref="C?"  Part="1" 
F 0 "C?" H 8240 2221 50  0000 L CNN
F 1 "10µ" H 8240 2130 50  0000 L CNN
F 2 "" H 8125 2175 50  0001 C CNN
F 3 "~" H 8125 2175 50  0001 C CNN
	1    8125 2175
	1    0    0    -1  
$EndComp
Wire Wire Line
	8125 2025 8125 1975
Wire Wire Line
	8125 1975 8525 1975
Wire Wire Line
	8125 2325 8125 2400
Wire Wire Line
	8125 2400 8325 2400
$Comp
L power:Earth #PWR?
U 1 1 5EFA2D32
P 8325 2400
AR Path="/5EFA2D32" Ref="#PWR?"  Part="1" 
AR Path="/5EC77F4C/5EFA2D32" Ref="#PWR?"  Part="1" 
F 0 "#PWR?" H 8325 2150 50  0001 C CNN
F 1 "Earth" H 8325 2250 50  0001 C CNN
F 2 "" H 8325 2400 50  0001 C CNN
F 3 "~" H 8325 2400 50  0001 C CNN
	1    8325 2400
	1    0    0    -1  
$EndComp
Connection ~ 8325 2400
Wire Wire Line
	8325 2400 8525 2400
Wire Wire Line
	8525 1625 8325 1625
Text HLabel 8325 1625 0    50   Input ~ 0
ThermistorVcc
Text HLabel 8775 1975 2    50   Input ~ 0
TemperatureMeasurement
$EndSCHEMATC
