EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 3 3
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Text HLabel 3725 2575 0    50   Input ~ 0
minus_button
Text HLabel 5650 2575 0    50   Input ~ 0
plus_button
Text HLabel 3700 3625 0    50   Input ~ 0
variable_select_button
Text HLabel 5625 3650 0    50   Input ~ 0
mode_button
$Comp
L Switch:SW_Push SW?
U 1 1 5E8BD24C
P 4525 2575
F 0 "SW?" H 4525 2860 50  0000 C CNN
F 1 "SW_Push" H 4525 2769 50  0000 C CNN
F 2 "" H 4525 2775 50  0001 C CNN
F 3 "~" H 4525 2775 50  0001 C CNN
	1    4525 2575
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 5E8BD8A7
P 4100 2425
F 0 "R?" H 4170 2471 50  0000 L CNN
F 1 "10k" H 4170 2380 50  0000 L CNN
F 2 "" V 4030 2425 50  0001 C CNN
F 3 "~" H 4100 2425 50  0001 C CNN
	1    4100 2425
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 5E8BDD87
P 4875 2575
F 0 "R?" V 4668 2575 50  0000 C CNN
F 1 "330" V 4759 2575 50  0000 C CNN
F 2 "" V 4805 2575 50  0001 C CNN
F 3 "~" H 4875 2575 50  0001 C CNN
	1    4875 2575
	0    1    1    0   
$EndComp
$Comp
L Device:C C?
U 1 1 5E8BE3A1
P 4100 2725
F 0 "C?" H 4215 2771 50  0000 L CNN
F 1 "100n" H 4215 2680 50  0000 L CNN
F 2 "" H 4138 2575 50  0001 C CNN
F 3 "~" H 4100 2725 50  0001 C CNN
	1    4100 2725
	1    0    0    -1  
$EndComp
Wire Wire Line
	5025 2575 5025 2875
Wire Wire Line
	5025 2875 4650 2875
$Comp
L power:GND #PWR?
U 1 1 5E8BF022
P 4650 2875
F 0 "#PWR?" H 4650 2625 50  0001 C CNN
F 1 "GND" H 4655 2702 50  0000 C CNN
F 2 "" H 4650 2875 50  0001 C CNN
F 3 "" H 4650 2875 50  0001 C CNN
	1    4650 2875
	1    0    0    -1  
$EndComp
Connection ~ 4650 2875
Wire Wire Line
	4100 2275 4100 2150
Wire Wire Line
	4100 2150 3975 2150
Text HLabel 3975 2150 0    50   Input ~ 0
Vcc(5V)
Wire Wire Line
	4100 2575 4325 2575
Connection ~ 4100 2575
Wire Wire Line
	4100 2875 4650 2875
Wire Wire Line
	3725 2575 4100 2575
$Comp
L Switch:SW_Push SW?
U 1 1 5E8C29E7
P 6450 2575
F 0 "SW?" H 6450 2860 50  0000 C CNN
F 1 "SW_Push" H 6450 2769 50  0000 C CNN
F 2 "" H 6450 2775 50  0001 C CNN
F 3 "~" H 6450 2775 50  0001 C CNN
	1    6450 2575
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 5E8C29ED
P 6025 2425
F 0 "R?" H 6095 2471 50  0000 L CNN
F 1 "10k" H 6095 2380 50  0000 L CNN
F 2 "" V 5955 2425 50  0001 C CNN
F 3 "~" H 6025 2425 50  0001 C CNN
	1    6025 2425
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 5E8C29F3
P 6800 2575
F 0 "R?" V 6593 2575 50  0000 C CNN
F 1 "330" V 6684 2575 50  0000 C CNN
F 2 "" V 6730 2575 50  0001 C CNN
F 3 "~" H 6800 2575 50  0001 C CNN
	1    6800 2575
	0    1    1    0   
$EndComp
$Comp
L Device:C C?
U 1 1 5E8C29F9
P 6025 2725
F 0 "C?" H 6140 2771 50  0000 L CNN
F 1 "100n" H 6140 2680 50  0000 L CNN
F 2 "" H 6063 2575 50  0001 C CNN
F 3 "~" H 6025 2725 50  0001 C CNN
	1    6025 2725
	1    0    0    -1  
$EndComp
Wire Wire Line
	6950 2575 6950 2875
Wire Wire Line
	6950 2875 6575 2875
$Comp
L power:GND #PWR?
U 1 1 5E8C2A01
P 6575 2875
F 0 "#PWR?" H 6575 2625 50  0001 C CNN
F 1 "GND" H 6580 2702 50  0000 C CNN
F 2 "" H 6575 2875 50  0001 C CNN
F 3 "" H 6575 2875 50  0001 C CNN
	1    6575 2875
	1    0    0    -1  
$EndComp
Connection ~ 6575 2875
Wire Wire Line
	6025 2275 6025 2150
Wire Wire Line
	6025 2150 5900 2150
Text HLabel 5900 2150 0    50   Input ~ 0
Vcc(5V)
Wire Wire Line
	6025 2575 6250 2575
Connection ~ 6025 2575
Wire Wire Line
	6025 2875 6575 2875
Wire Wire Line
	5650 2575 6025 2575
$Comp
L Switch:SW_Push SW?
U 1 1 5E8C405E
P 4500 3625
F 0 "SW?" H 4500 3910 50  0000 C CNN
F 1 "SW_Push" H 4500 3819 50  0000 C CNN
F 2 "" H 4500 3825 50  0001 C CNN
F 3 "~" H 4500 3825 50  0001 C CNN
	1    4500 3625
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 5E8C4064
P 4075 3475
F 0 "R?" H 4145 3521 50  0000 L CNN
F 1 "10k" H 4145 3430 50  0000 L CNN
F 2 "" V 4005 3475 50  0001 C CNN
F 3 "~" H 4075 3475 50  0001 C CNN
	1    4075 3475
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 5E8C406A
P 4850 3625
F 0 "R?" V 4643 3625 50  0000 C CNN
F 1 "330" V 4734 3625 50  0000 C CNN
F 2 "" V 4780 3625 50  0001 C CNN
F 3 "~" H 4850 3625 50  0001 C CNN
	1    4850 3625
	0    1    1    0   
$EndComp
$Comp
L Device:C C?
U 1 1 5E8C4070
P 4075 3775
F 0 "C?" H 4190 3821 50  0000 L CNN
F 1 "100n" H 4190 3730 50  0000 L CNN
F 2 "" H 4113 3625 50  0001 C CNN
F 3 "~" H 4075 3775 50  0001 C CNN
	1    4075 3775
	1    0    0    -1  
$EndComp
Wire Wire Line
	5000 3625 5000 3925
Wire Wire Line
	5000 3925 4625 3925
$Comp
L power:GND #PWR?
U 1 1 5E8C4078
P 4625 3925
F 0 "#PWR?" H 4625 3675 50  0001 C CNN
F 1 "GND" H 4630 3752 50  0000 C CNN
F 2 "" H 4625 3925 50  0001 C CNN
F 3 "" H 4625 3925 50  0001 C CNN
	1    4625 3925
	1    0    0    -1  
$EndComp
Connection ~ 4625 3925
Wire Wire Line
	4075 3325 4075 3200
Wire Wire Line
	4075 3200 3950 3200
Text HLabel 3950 3200 0    50   Input ~ 0
Vcc(5V)
Wire Wire Line
	4075 3625 4300 3625
Connection ~ 4075 3625
Wire Wire Line
	4075 3925 4625 3925
Wire Wire Line
	3700 3625 4075 3625
$Comp
L Switch:SW_Push SW?
U 1 1 5E8C5039
P 6425 3650
F 0 "SW?" H 6425 3935 50  0000 C CNN
F 1 "SW_Push" H 6425 3844 50  0000 C CNN
F 2 "" H 6425 3850 50  0001 C CNN
F 3 "~" H 6425 3850 50  0001 C CNN
	1    6425 3650
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 5E8C503F
P 6000 3500
F 0 "R?" H 6070 3546 50  0000 L CNN
F 1 "10k" H 6070 3455 50  0000 L CNN
F 2 "" V 5930 3500 50  0001 C CNN
F 3 "~" H 6000 3500 50  0001 C CNN
	1    6000 3500
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 5E8C5045
P 6775 3650
F 0 "R?" V 6568 3650 50  0000 C CNN
F 1 "330" V 6659 3650 50  0000 C CNN
F 2 "" V 6705 3650 50  0001 C CNN
F 3 "~" H 6775 3650 50  0001 C CNN
	1    6775 3650
	0    1    1    0   
$EndComp
$Comp
L Device:C C?
U 1 1 5E8C504B
P 6000 3800
F 0 "C?" H 6115 3846 50  0000 L CNN
F 1 "100n" H 6115 3755 50  0000 L CNN
F 2 "" H 6038 3650 50  0001 C CNN
F 3 "~" H 6000 3800 50  0001 C CNN
	1    6000 3800
	1    0    0    -1  
$EndComp
Wire Wire Line
	6925 3650 6925 3950
Wire Wire Line
	6925 3950 6550 3950
$Comp
L power:GND #PWR?
U 1 1 5E8C5053
P 6550 3950
F 0 "#PWR?" H 6550 3700 50  0001 C CNN
F 1 "GND" H 6555 3777 50  0000 C CNN
F 2 "" H 6550 3950 50  0001 C CNN
F 3 "" H 6550 3950 50  0001 C CNN
	1    6550 3950
	1    0    0    -1  
$EndComp
Connection ~ 6550 3950
Wire Wire Line
	6000 3350 6000 3225
Wire Wire Line
	6000 3225 5875 3225
Text HLabel 5875 3225 0    50   Input ~ 0
Vcc(5V)
Wire Wire Line
	6000 3650 6225 3650
Connection ~ 6000 3650
Wire Wire Line
	6000 3950 6550 3950
Wire Wire Line
	5625 3650 6000 3650
Text Notes 4000 1800 0    50   ~ 0
Caps are here to serve as cheap debouncers and parallel 330 Ohms \nresistor is used to prevent wire inductance to generate high voltages\nwhen switch interrupts current flow suddenly
$Comp
L Connector_Generic:Conn_01x04 J?
U 1 1 5E8D6162
P 3600 5000
F 0 "J?" H 3680 4992 50  0000 L CNN
F 1 "Conn_01x04" H 3680 4901 50  0000 L CNN
F 2 "" H 3600 5000 50  0001 C CNN
F 3 "~" H 3600 5000 50  0001 C CNN
	1    3600 5000
	1    0    0    -1  
$EndComp
Text HLabel 3400 5000 0    50   Input ~ 0
i2c_sda
Text HLabel 3400 5100 0    50   Input ~ 0
i2c_scl
$Comp
L power:GND #PWR?
U 1 1 5E8D70CD
P 3400 5200
F 0 "#PWR?" H 3400 4950 50  0001 C CNN
F 1 "GND" H 3405 5027 50  0000 C CNN
F 2 "" H 3400 5200 50  0001 C CNN
F 3 "" H 3400 5200 50  0001 C CNN
	1    3400 5200
	1    0    0    -1  
$EndComp
Text HLabel 3400 4900 0    50   Input ~ 0
Vcc(5V)
$EndSCHEMATC