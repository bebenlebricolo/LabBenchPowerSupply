# LabBenchPowerSupply
A Diy lab bench power supply, made as simple as possible, from an Arduino Nano and generic discrete parts

## Motivation(s)
The main goal of this project is indeed to build a bench power supply, but I really wanted to provide a complete project including all the electronical design, mechanical design and software design that goes with it.

Also, despite using an Arduino as the "brain" of this project, I also wanted to decouple the software from any Arduino-based code, because I thougth it was a good occasion to show how "simple" it could be to directly target the chip, not using any framework to hide any detail.

This is the reason why I reimplemented all the necessary drivers to interact with the peripherals of the Atmega328P, and to some extent, tried to make them compatible with most of the cheap avr cores that we could find for a handful of dollars/euros/whatever.

# Electronical design
The Electonical design of this project is entirely made using KiCAD - which I love - and I wanted a rather simple circuit, built around generic parts.

## Goals
Here is a list of the features I wanted to integrate within this project :

### Basic requirements
* Standalone powersupply, using the mains power source as its primary power source
* Does not require any extreme soldering skills : most, if not all of the components shall be through-holes ones
* Outputs a decent wattage (about 10 Amps / 12 Volts DC => depends on the components one will choose )
* Easy to tune with few interactions needed
* User interface such as an LCD screen, cheap and neat!
* Voltage and current regulations
* Allow to connect/disconnect the load using a button/lever switch 

### Safety features
* Low voltages only : I want this project to only output voltages that are well below the hazardous range. I plan to output a maximum 16 Volts
* Features Thermal protection : thermal sensor coupled with a thermal regulation system (variable powered fans shall be used)
* Overcurrent protection (maybe using a relay connected to 100% hardware circuit)

### Miscellaneous
* Banana plugs output connectors, for ease of use
* Eventually, XT connectors interface (because the connector provide a polar interface which prevents reversing polarities, more handy)
* Simple buttons interface to drive the powersupply
* Would be nice to have some profiles registered in the system, such as 10 memories with various settings

# Software design
As succintly explained in the header of this readme, I want a simple software to run on this machine. However, there are some critical aspects to be kept in mind here, such as the several safety features which have to be implemented cleanly.
This brought me to redevelop all the drivers only using the datasheet of Atmel's (now Microchip) Atmega328P as a reference.

## Testing
While developping the software for this project, I needed a way to consolidate my developments as it is not really trivial to debug the code once it is embedded on the device. That is why I am writing my tests in parallel to project development, which allows to detect errors earlier and make sure the logic is right (this is what testing is all about, after all!).

### Approach for drivers testing
#### Using GoogleTest testing framework
Testing an application is rather easy to do when developping application software as it is rather simple to substitute components, because already abstracted. 
However, it is a quite different topic when working on drivers, because they are completely dependent of the hardware they rely on. This is why I had to use a dependency injection approach, substituting the internal peripheral's interface register by stubbed ones, allowing me to 'simulate' the actual behavior of the peripheral.

#### Using simulators, such as simavr!
The bad thing is that even when trying to mimic the peripheral registers, sometimes we can make mistakes and all tests are passing, but the code does not run well on the real target. Actually, reproducing the hardware's behavior in test cases is subject to your own assumptions and understanding about the hardware, it is rather easy to get out of sync with it.

One solution is to use simulators, such the one embedded wihtin AtmelStudio7 or using simavr on Linux.
Despite not being 100% accurate, they both offer really good starting points when it comes to simulating the targeted avr core, I discovered many mistakes in the code like this, despite my tests being all passing.

## Drivers
The main goal here is to implement drivers whose only role is to abstract the hardware's peripheral details. No less, and certainly no more. As a result, they may need more high level components which will handle the low-levels details instead of direclty calling it from application software.

### Example : timers
For instance, all the timers APIs are matching the underlying hardware, exposing things such as their running mode (fast_pwm generator, phase_correct_pwm_generator, Clear Timer on Compare match, Output Compare modes, Interrupt-based, non interrupt-based, etc).

Their purpose is to provide a uniform interface, that you can interact with either using unitary calls (such as timer_8_bit_set_interrupt_enable()) or complete configuration calls such as timer_8_bit_init() which needs an explicit configuration object.

So their interface is quite complete (and tested), but we still need some more specialized modules to use them.

To continue with the Timer example, let's think about a PWM module : let's say I want a PWM module, which configures one of the available timer to produce an accurate, hardware-based pwm signal.
This module will one of the 8 bit timer, and perform all the necessary driver manipulation to achieve its goal.

We could then have something like this : 
```C
/* Error handling should be done in between calls but is eluded for clarity */
pwm_error_t ret = PWM_ERROR_OK;
ret = pwm_init(gpio_structure);
ret = pwm_set_frequency(150'000);
ret = pwm_set_duty_cycle(75);
ret = pwm_start();
```

All the drivers are built using this approach : a driver is no more and no less than a driver. This leaves the possibility to fine-tune the hardware as well as building high level modules, taylored for each given purposes.

Last but not least, as the drivers expose a fairly complete set of accessors and functions that could be used at any moment, it could represent a substantial amount of unused functions. This is why they are built with the `-flto` flag as well as the `-ffunction-sections` and `-Wl,--gc-sections` flags, which allow respectively :
* `-flto` : tells the linker to perform a link-time optimisation pass (trims unused functions for instance)
* `-ffunction-sections` : place each functions in its own section
* `-Wl,--gc-sections` : garbage-collects unused sections

Those flags are defined within the `avr-gcc-toolchain.cmake` file, which also defines a bunch of parameters to cross-compile the project onto avr targets.

# Required tools to build on Linux :
## Building application firmware
### Avr gcc toolchain and ABI
* gcc-avr (version >= 8.3.0)
* avr-libc
* binutils-avr

*Note : at the moment, ubuntu repositories does not provide an up-to-date version of avr-gcc toolchain(5.4.0, which does not support -lto flag). You can either build avr-gcc from sources to use a more version of gcc (like 9.3.0) or retrieve an existing package if it has been compiled by someone before. This note will hopefully disapear as I have already done this step myself, I will need to build a consistent package for it though*

### Cmake tools
* Cmake (version >= 3.0)

## Building tests
To build the tests, make sure you have `libgtest-dev`installed on your machine
### Ubuntu 20.04
* Google tests libraries and binaries
    * libgtest-dv
    * googletest

* GCC tools :
    * build-essentials or 
    * gcc (version >= 8.3.0)