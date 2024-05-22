@echo off
REM Compile and upload the sketch to Arduino
@REM cd /d "C:\Users\Exertion Games Lab\Documents\Arduino"
cd /d "C:\Users\Exertion Games Lab\bin"

arduino-cli compile --fqbn arduino:avr:nano:cpu=atmega328old "C:\Users\Exertion Games Lab\Documents\GitHub\winter_research\free\ftp"
arduino-cli upload -p COM7 --fqbn arduino:avr:nano:cpu=atmega328old "C:\Users\Exertion Games Lab\Documents\GitHub\winter_research\free\ftp"

REM Start the EmotiBit Oscilloscope application
cd /d "C:\Program Files\EmotiBit\EmotiBit Oscilloscope"
start "" "EmotiBitOscilloscope.exe"

REM Run the Processing sketch
cd /d "C:\Users\Exertion Games Lab\Downloads\processing-4.2-windows-x64\processing-4.2"
processing-java --sketch="C:\Users\Exertion Games Lab\Documents\GitHub\winter_research\free\ftp_p" --run

echo Operations completed.
pause