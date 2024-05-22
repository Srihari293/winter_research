@echo off
REM Compile and upload the sketch to Arduino
cd "C:\Users\Exertion Games Lab\bin"

arduino-cli compile --fqbn arduino:avr:nano:cpu=atmega328old "C:\Users\Exertion Games Lab\Documents\GitHub\winter_research\src\arduinoEDAHRPA"
arduino-cli upload -p COM7 --fqbn arduino:avr:nano:cpu=atmega328old "C:\Users\Exertion Games Lab\Documents\GitHub\winter_research\src\arduinoEDAHRPA"

REM Start the EmotiBit Oscilloscope application
cd /d "C:\Program Files\EmotiBit\EmotiBit Oscilloscope"
start "" "EmotiBitOscilloscope.exe"

REM Run the Processing sketch
cd "C:\Users\Exertion Games Lab\Downloads\processing-4.2-windows-x64\processing-4.2"
processing-java --sketch="C:\Users\Exertion Games Lab\Documents\GitHub\winter_research\src\DetectLieHR" --run

echo Operations completed.
pause