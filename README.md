# AD9958-Controller
Arduino control of a DDS AD9958 chip

This code uses bitbanging to write to the register of a 
quad channel DDS with 32 bit frequency words. 

The DDS was phase synchronized to the NIST maser/timing infra
and the output was fed into an RF amplifier to drive
an AOM at 80 MHz. 

Other registers can be selected to modulate the amplitude and
phase of the DDS which was later used to create high frequencies for
multi-tone frequency modulation spectroscopy.
