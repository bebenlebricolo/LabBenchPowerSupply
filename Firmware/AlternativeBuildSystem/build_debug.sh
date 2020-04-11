avr-gcc ../App/src/main.c -c -o main.o -I../Drivers/Adc/inc -I../Drivers/Peripheral/inc -I/usr/avr/include -I/usr/avr/include/avr -D__AVR_ATmega328__ -mmcu=atmega328p -g -O0
avr-gcc ../Drivers/Adc/src/adc.c -c -o adc.o -I../Drivers/Adc/inc -I../Drivers/Peripheral/inc -I../Drivers/Adc/private_inc -I/usr/avr/include -I/usr/avr/include/avr -D__AVR_ATmega328__ -mmcu=atmega328p -g -O0
avr-gcc ../Drivers/Adc/src/adc_stack.c -c -o adc_stack.o -I../Drivers/Adc/inc -I../Drivers/Peripheral/inc -I../Drivers/Adc/private_inc -I/usr/avr/include -I/usr/avr/include/avr -D__AVR_ATmega328__ -mmcu=atmega328p -g -O0
avr-gcc main.o adc.o adc_stack.o -o firmware /usr/avr/lib/avr5/crtatmega328p.o -L/usr/lib/gcc/avr/9.3.0/avr5/
#avr-ld main.o adc.o adc_stack.o -o firmware -l/usr/avr/lib/libc.a
