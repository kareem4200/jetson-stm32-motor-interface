# jetson-stm32-motor-interface
HW interface between Jetson Nano and STM32 Blackpill 

Interesting observation: while playing around with pwm frequencies, I noticed that the 
higher the frequency, the slower the motor spins, at the same duty cycle. The reasonable explanation I found 
online is that this is due to the L298N old bipolar transistors which has a finite switching time and the higher 
the pwm frequency, the more the switching frequency and the more time L298N spends in the transition region 
where it is not fully on or off, which results in less effective voltage being applied to the motor.

at 10 kHz, and duty cycle less than 70%, the motor buzz and do not spin, but at 20 kHz, it does not buzz.
at 10 kHz, and duty cycle of 80%, the motor spins at a reasonable speed, but at 20 kHz, it spins a bit slower.

The current method I use as safety mechanism when the direction reverses is to set the duty cycle to 0, wait for a few microseconds, and then set the new direction and duty cycle. But for 4 motors this becomes worst case 4 * us and blocks everything else. Later when USB comm is implemented, timing becomes more critical with commands coming from the Jetson Nano and there should be command timeouts. We need a non-blocking method later to handle this.