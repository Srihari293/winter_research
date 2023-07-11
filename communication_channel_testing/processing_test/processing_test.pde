/* */
// Dummy data for test 1, 2 and 3
import processing.serial.*;

Serial myPort;  // Create object from Serial class
String portName;
int i = 0;

void setup() 
{
  size(200,200); //make our canvas 200 x 200 pixels big
  portName = Serial.list()[0]; //change the 0 to a 1 or 2 etc. to match your port
  myPort = new Serial(this, portName, 9600);
}

void draw() {
   for(i = 0; i<10000; i++){
     println("i = ",i, "in port", portName);
     myPort.write(i);
     delay(1000);
   }
}
