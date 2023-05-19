# Serial Scope with Arduino Nano Every

This project allows you to monitor binary data traffic on both directions of a serial port.
The default configuration is for an Epsp connection as used with the Epson HX-20, PX-8 and PX-4
laptops to pheriperials like disk drives and display units. 

Below is a typical exchange between a PX-4 "1>" and a floppy terminal unit "2>". This is essentially a half duplex connection, as only one end is in sending mode at any time. A transfer direction change is indicated with a newline character and an origin prompt, either "1>" or "2>". A timeout (100ms) is used to separate multiple messages from one side.

The sketch can be adapted to other Arduino's, provided they have two serial ports and a console. The console baud rate has to be three times higher than the monitored traffic baud rate.


	1>0431312305
	2>06
	1>01003123770232
	2>06
	1>0201040203F4
	2>06
	1>04
	2>010123317780B3
	1>06
	2>02E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E5E500037B
	1>06
	2>04
	1>04

[The schema](DualSerialMonitor.kicad_sch.pdf)

fjkraan@electrickery.nl, 2023-05-19