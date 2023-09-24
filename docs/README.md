# Docs

Unlike CNLohr, I'm not using a custom board as I didn't have time to order it. Instead I used an Arduino Pro Mini Board which has an ATmega328P-AU chip (unlike the ATmega168PB CNLohr used). It should be mostly compatible for what we need (no special chip features are used, everything is bit-banged). 

![Pro Mine Pinout: https://www.robotistan.com/Data/EditorFiles/%C3%9Cr%C3%BCnler%20A%C3%A7%C4%B1klama/arduino-pro-mini-pinout.png](pro_mini_pinout.png)

The Pro Mini board has a 16MHz crystal, whereas CNLohr used the internal 8MHz oscillator and a 32.768 kHz external one.
I need to modify my hardware as the RTC oscillator is needed.

I am using a USBASP ICSP programmer instead of usbtiny.
Note: You may need to add udev rules or just run avrdude as admin. PlatformIO udev rules should do. Otherwise, see this guide: https://andreasrohner.at/posts/Electronics/How-to-fix-device-permissions-for-the-USBasp-programmer/

Steps to porting:
- Check for pin compatibility
- Check wether connecting SD-Card is possible
- Check whether firmware can be compiled for this chip
- Check fuse configuration
- Modify firmware to adjust timing for faster clock
  - Adjust the cycle-based 24-hour timing system
  - Adjust the bit-banged driver as with a 16 Mhz crystal some delays will probably need to be added to get the timing right

## Step 1: Check for pin compatibility

Problem: the PB chip series is not quite compatible with the P chip series. In fact, it's an overhauled chip.

<table>
    <tr>
        <th>PB-Series Chips</th>
        <th>P-Series Chips</th>
    </tr>
    <tr>
        <td>Datasheet: (to be added)</td>
        <td>Datasheet: https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf</td>
    </tr>
    <tr>
        <td><img src="pb_series_pinout.png" style="width: 40vw;"> </img></td>
        <td><img src="p_series_pinout.png" style="width: 40vw;"> </img></td>
    </tr>
</table>

Problems:
- (PE0 and PE1 programming pins don't exist but they are not needed luckily)
- we cannot use pins PE2 and PE3, as the older 328P chip only has ADC functionality on these. In fact, they are not even on any register.
  - don't need PE2, as we will put power to SD and power to display together.
  - don't need PE3, as we will not be using card detection. We will simply assume the card is present.

## Fuse configuration

Fuse Calculator: https://www.engbedded.com/fusecalc/

My original Fuse config (from Arduino-compatible setup): (E:FD, H:DE, L:FF) (external oscillator)
Disable Brown-Out detection: (E:FF)
Fuse config for default internal 8MHz oscillator: (L:E2)
External full-swing is the same as CNLohr: (L:E6)

Currently, I was not able to get the RTC function to work, because my 32k crystal probably doesn't work (couldn't verify).

AVR RTC feature info: https://ww1.microchip.com/downloads/en/Appnotes/Atmel-1259-Real-Time-Clock-RTC-Using-the-Asynchronous-Timer_AP-Note_AVR134.pdf
