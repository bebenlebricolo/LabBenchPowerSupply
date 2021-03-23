# I2C End to End testing firmwares
Those fake firmwares were written in the aim to help optimizing/debugging I2C drivers.
They run on real chips, so we'll need real working devices (for instance Arduino Uno(s) or Arduino nano(s)).
As those firmwares try to test the I2C driver, the only dependency requirement shall be restricted to that driver only.

# Scenarios
We'll need to test several configurations such as :
* A master I2C device writing to a slave device
* A master I2C device reading from a slave device without opcode (direct read)
* A master I2C device reading from a slave device with opcode (an I2C write is performed first)
* A master I2C device writing data to a slave device then reading data back from it