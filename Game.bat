@REM @echo off
@REM REM Navigate to the EmotiBit Oscilloscope's directory
@REM cd /d "C:\Program Files\EmotiBit\EmotiBit Oscilloscope"
@REM REM Start the EmotiBit Oscilloscope application
@REM start "" "EmotiBitOscilloscope.exe"
@REM cd "C:\Users\sriha\Downloads\processing-4.2-windows-x64\processing-4.2"
@REM processing-java --sketch="C:\Users\sriha\Documents\Code\pneunochio\winter_research\src\DetectLieHR" --run

@echo off
REM Compile and upload the sketch to Arduino
cd "C:\Users\sriha\OneDrive\Documents\Arduino"
REM arduino-cli compile --fqbn arduino:avr:nano:cpu=atmega328old "C:\Users\sriha\Documents\Code\pneunochio\winter_research\src\arduinoEDAHRPA"
arduino-cli upload -p COM7 --fqbn arduino:avr:nano:cpu=atmega328old "C:\Users\sriha\Documents\Code\pneunochio\winter_research\src\arduinoEDAHRPA"

REM Start the EmotiBit Oscilloscope application
cd /d "C:\Program Files\EmotiBit\EmotiBit Oscilloscope"
start "" "EmotiBitOscilloscope.exe"

REM Run the Processing sketch
cd "C:\Users\sriha\Downloads\processing-4.2-windows-x64\processing-4.2"
processing-java --sketch="C:\Users\sriha\Documents\Code\pneunochio\winter_research\src\DetectLieHR" --run

echo Operations completed.
pause