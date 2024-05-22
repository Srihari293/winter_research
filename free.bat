@echo off
REM Compile and upload the sketch to Arduino
cd /d "C:\Users\sriha\OneDrive\Documents\Arduino"
arduino-cli compile --fqbn arduino:avr:nano:cpu=atmega328old "C:\Users\sriha\Documents\Code\pneunochio\winter_research\free\ftp"
arduino-cli upload -p COM7 --fqbn arduino:avr:nano:cpu=atmega328old "C:\Users\sriha\Documents\Code\pneunochio\winter_research\free\ftp"

REM Start the EmotiBit Oscilloscope application
cd /d "C:\Program Files\EmotiBit\EmotiBit Oscilloscope"
start "" "EmotiBitOscilloscope.exe"

REM Run the Processing sketch
cd /d "C:\Users\sriha\Downloads\processing-4.2-windows-x64\processing-4.2"
processing-java --sketch="C:\Users\sriha\Documents\Code\pneunochio\winter_research\free\ftp_p" --run

echo Operations completed.
pause