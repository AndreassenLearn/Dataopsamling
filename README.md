# Dataopsamling
Teknikfag

## Flashing to Arduino UNO using Microchip Studio

- Go to *`Tools` > `External Tools...`*.
- Add new Tool for Arduino UNO:
	- Title: `Arduino UNO`
	- Command: `C:\Program Files (x86)\Arduino\hardware\tools\avr\bin\avrdude.exe`
	- Arduments: `-C"C:\Program Files (x86)\arduino\hardware\tools\avr\etc\avrdude.conf" -v -patmega328p -carduino -PCOM3 -b115200 -D -Uflash:w:"$(ProjectDir)Debug\$(TargetName).hex":i` (Remember to set the correct COM port, i.e. `-PCOM3` for `COM3`)
	- Select the `Use Output Windows` option.
- Build solution (F7).
- Flash to Arduino UNO; `Menu` > `Tools` > `Arduino UNO`.
- The output will be displayed in the `Output` window.
