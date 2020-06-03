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