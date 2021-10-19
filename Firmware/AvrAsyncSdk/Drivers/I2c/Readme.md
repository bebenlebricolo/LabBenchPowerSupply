# I2C Driver for Atmega328xx chip (and other AVR cores)
This I2C driver was implemented as part of the LabBenchPowerSupply project, in its very early development stages.
It provides a simple interface to configure and use the Atmel's/Microchip's Two Wire Interface peripheral embedded in most of their AVR core,
in fully asynchronous operation mode.

## What's provided with it
### Fully asynchronous operating mode
This driver could be used **with** or **without** hardware-based interrupt (twi interrupt vector), leaving full control over the software stack and providing the ability to configure and use several TWI peripheral using a **single driver**.
For instance, this could be useful when working with the Atmega328PB which embeds several TWI, all wired to the same interrupt (twi_vect).
By design, this driver is not blocking : this means the CPU is free to continue its work instead of wasting cycles waiting for TWI hardware to complete its task. 

### Simple interface, full control over configuration
As this driver could be relatively complex, it was essential to reduce the amount of work put into the device configuration to its bare minimum.
This driver then provides a default configuration, that one may use as a starting point to configure the driver.
On the other hand, full control might be required at some point, and one may not want to fully reconfigure the whole device when a single parameter can just be changed. This is easily done using the **unitary parameter API** offered with the driver : it allows to get/set a single parameter of the device.
However, be carefull when using those ones : operation is only permitted while TWI peripheral is not processing (i.e. TWINT flag is set to 1).

### Simple operations as master, or slave
This driver provides two functions to handle reading and writing to a slave, **i2c_write** and **i2c_read**. They both configure the driver as a master, and sets relevant data in TWI registers. Then, the i2c_process() function or interrupt will be used to handle the transaction.
Also, the slave interface of this driver allows to define an I2C exposed data interface and uses a dedicated function pointer used to handle incoming bytes.
This callback function needs to be overridden by application software, and is called whenever the devices is addressed as a slave on the I2C bus.

### Fully tested interface
This driver comes with a load of tests (written with google Tests framework) which allow to put the device into several scenarios and see how it's behaving.
Although testing the unitary interface is pretty trivial, testing the actual master or slaves operating modes requires some more work.
As a consequence, I've written a rather complete I2C bus simulator which registers virtual I2C devices and exchanges data between them.
This is why I had to write a simulator for the TWI peripheral (which is called **twi_hardware_stub**), the I2c bus simulator itself, called **I2cBusSimulator** (and which is the only written in C++, because why not!), and another virtual I2C device which is called **i2c_fake_device**.

This design allows to test the driver under many scenarios, as a master, transmitting mode, master receveiver, slave transmitter, slave receiver, and even mixing all modes together in a sequence to assert that the driver handles mode transitions just fine. 

# How to use the driver
Note that the examples below are only here to depict how this driver works and does not reflect any application firmware design.
For more details about the inner mechanics, a good look at **i2c_driver_tests.cpp** located in Tests/ might give a more detailed picture on how the driver might be used.

## Initialisation
First, you will need to configure it in the initialisation step:

```C
#include "i2c.h"
...
...
// In application code : (for instance in a function called init_transmission() =
i2c_config_t config;
i2c_error_t ret = i2c_get_default_config(&config);
// Check that everything's fine
if (I2C_ERROR_OK != ret)
{
    // Handle error here
}

// Initialise the handle with the right data :
// Note : step of using pointer to memory might appear weird, or even a bad idea.
// However, this is the key to handle several TWI devices as well as enabling Dependency Injection for testing purposes
// Furthermore, those registers are not accessed very often (we are not accessing them like we would if those were GPIO registers with precise PWM timing requirements!)
config.handle.TWCR = &TWCR;
config.handle.TWBR = &TWBR;
config.handle.TWSR = &TWSR;
config.handle.TWDR = &TWDR;
config.handle.TWAR = &TWAR;
config.handle.TWAMR = &TWAMR;

// Set whatever parameter you want in config object
config.slave_address = 0x34;

// Assuming this is the only device used by your application firmware
// Note : to be able to do this, application firmware shall provide a "config.h" file which defines the I2C_DEVICES_COUNT macro like so : 
// #define I2C_DEVICES_COUNT (1U)
ret = i2c_init(0U, &config);
if (I2C_ERROR_OK != ret)
{
    // Handle error here
}
i2c_state_t state;
ret = i2c_get_state(0U, &state);
if (I2C_ERROR_OK != ret || I2C_STATE_READY != state)
{
    // Handle error here
}
```

## Starting a transmission as master (for instance, a write transmission)

```C
// Within some application code :

...
...

// Define a buffer with enough space to carry all required data
uint8_t buffer[MULTI_BYTES_ARRAY_LEN + 1] = {0};

// First byte of the buffer contains the operation code / command which will be interpreted by targeted device
buffer[0] = TARGET_DEVICE_COMMAND_MESSAGE;

// Fill in the message ...
snprintf((char *) (buffer + 1), MULTI_BYTES_ARRAY_LEN, "Some message to be written");

// If you want to use interrupts :
// sei();

// Device is in its READY state, time to ask it to start a write transaction
ret = i2c_write(0U, 0x23, buffer, 25, 0U);
if (I2C_ERROR_OK != ret)
{
    // Handle error here
}

``` 

## Processing data (if not interrupt based)

```C
// If NOT using interrupts, you will have to call :
i2c_process(0U);
// Otherwise, interrupt service routine will do that for you automatically

// Polling the device state to know if the transaction has completed :
i2c_state_t state;
ret = i2c_get_state(0U, &state);
if (I2C_ERROR_OK != ret)
{
    // Handle error here
}

if (I2C_STATE_READY == state)
{
    // Proceed with next operation
}

```
