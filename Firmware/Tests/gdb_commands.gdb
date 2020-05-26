#set disable-randomization off
set disable-randomization off

#b 662
b 667
#b i2c.c:945
#b I2cBusSimulator::process

#r --gtest_filter=I2cTestFixture.test_read_message_from_fake_device
r
