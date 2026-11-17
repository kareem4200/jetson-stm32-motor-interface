# jetson-stm32-motor-interface
HW interface between Jetson Nano and STM32 Blackpill 

Interesting observation: while playing around with pwm frequencies, I noticed that the 
higher the frequency, the slower the motor spins, at the same duty cycle. The reasonable explanation I found 
online is that this is due to the L298N old bipolar transistors which has a finite switching time and the higher 
the pwm frequency, the more the switching frequency and the more time L298N spends in the transition region 
where it is not fully on or off, which results in less effective voltage being applied to the motor.

at 10 kHz, and duty cycle less than 70%, the motor buzz and do not spin, but at 20 kHz, it does not buzz.
at 10 kHz, and duty cycle of 80%, the motor spins at a reasonable speed, but at 20 kHz, it spins a bit slower.