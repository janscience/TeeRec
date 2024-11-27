#include <Arduino.h>
#include <PowerSave.h>


#if defined(__IMXRT1062__)

#include "usb_dev.h"

#ifdef __cplusplus
extern "C" {
#endif
    extern void start_usb_pll(void);
#ifdef __cplusplus
}
#endif

FASTRUN void start_usb_pll( void ) {
    while ( 1 ) {
        uint32_t n = CCM_ANALOG_PLL_USB1;
        if ( n & CCM_ANALOG_PLL_USB1_DIV_SELECT ) {
            // bypass 24 MHz
            CCM_ANALOG_PLL_USB1_CLR = 0xC000;
            CCM_ANALOG_PLL_USB1_SET = CCM_ANALOG_PLL_USB1_BYPASS; // bypass
            CCM_ANALOG_PLL_USB1_CLR = CCM_ANALOG_PLL_USB1_POWER | // power down
            CCM_ANALOG_PLL_USB1_DIV_SELECT |                      // use 480 MHz
            CCM_ANALOG_PLL_USB1_ENABLE |                          // disable
            CCM_ANALOG_PLL_USB1_EN_USB_CLKS;                      // disable usb
            continue;
        }
        if ( !( n & CCM_ANALOG_PLL_USB1_ENABLE ) ) {
            CCM_ANALOG_PLL_USB1_SET = CCM_ANALOG_PLL_USB1_ENABLE;
            continue;
        }
        if ( !( n & CCM_ANALOG_PLL_USB1_POWER ) ) {
            CCM_ANALOG_PLL_USB1_SET = CCM_ANALOG_PLL_USB1_POWER;
            continue;
        }
        if ( !( n & CCM_ANALOG_PLL_USB1_LOCK ) ) {
            
            continue;
        }
        if ( n & CCM_ANALOG_PLL_USB1_BYPASS ) {
            CCM_ANALOG_PLL_USB1_CLR = CCM_ANALOG_PLL_USB1_BYPASS;
            continue;
        }
        if ( !( n & CCM_ANALOG_PLL_USB1_EN_USB_CLKS ) ) {
            CCM_ANALOG_PLL_USB1_SET = CCM_ANALOG_PLL_USB1_EN_USB_CLKS;
            continue;
        }
        break;
    }
}

#endif


void shutdown_usb() {
  Serial.println("\n\nSHUTTING DOWN USB");
  Serial.end();
#if defined(__IMXRT1062__)
  /* This is
     void SnoozeUSBSerial::disableDriver( uint8_t mode )
     from https://github.com/duff2013/Snooze/blob/master/src/hal/TEENSY_40/SnoozeUSBSerial.cpp
   */
#if F_CPU >= 20000000 && defined(USB_SERIAL)
  if (CCM_ANALOG_PLL_USB1_POWER > 0) {
    USBPHY1_CTRL_CLR = USBPHY_CTRL_CLKGATE;
    USBPHY1_PWD_CLR = 0xFFFFFFFF;
    NVIC_ENABLE_IRQ( IRQ_USB1 );
    start_usb_pll( );
    usb_cdc_line_rtsdtr = 0;
    usb_cdc_line_rtsdtr_millis = systick_millis_count;
  }
#endif
#endif
}


void shutdown_pins() {
  /* From https://www.pjrc.com/teensy/low_power.html:
     All I/O pins default to INPUT mode. That is safest, since you may connect a sensitive signal. But unconnected pins in INPUT mode can change voltage due to the slightest electrostatic coupling. If they drift to a voltage near the "middle", between digital high and low, the input circuitry in the chip consumes unnecessary power. On a Teensy++, many extra mA of current can be wasted with 40+ unconnected pins.
     
     The solution is easy. Just add code which configures all unused pins to output mode.
  */
}

