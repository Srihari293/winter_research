// Reads EmotiBit data from an OSC stream and plots data in a window
import oscP5.*;
import netP5.*;
import processing.serial.*;

// ------------ CHANGE PARAMETERS HERE --------------- //
// Look in EmotiBit Oscilloscope/data/oscOutputSettings.xml for EmotiBit OSC port and addresses
String oscAddress1 = "/EmotiBit/0/HR"; 
String oscAddress2 = "/EmotiBit/0/EDA"; 
int oscPort = 12345;
String arduinoPortName = "COM3"; // Replace with your Arduino's port name
int arduinoBaudRate = 9600; // Set the same baud rate as used in the Arduino code
// --------------------------------------------------- //

// Change these variables to change the filters
float samplingFreq = 25; // change to match the sampling frequency of the data
boolean lowPass = false; // toggles on/off the low-pass filter
float lpCut = 3; // adjusts the cut frequency of the low-pass filter
boolean highPass = false; // toggles on/off the high-pass filter
float hpCut = 1; // adjusts the cut frequency of the high-pass filter

OscP5 oscP5;
FloatList dataListHR = new FloatList();
FloatList dataListEDA = new FloatList();
Serial arduinoPort; // Serial object for Arduino communication

// filter variables
float lpFiltVal;
float hpFiltVal;
boolean firstFilt = false;

// --------------------------------------------------- //

void setup() {
  size(620, 840);
  /* start oscP5, listening for incoming messages at port 12345 */
  oscP5 = new OscP5(this, oscPort);
  arduinoPort = new Serial(this, arduinoPortName, arduinoBaudRate);
}

// --------------------------------------------------- //

void draw() {
  if (dataListHR.size() > 0 && dataListEDA.size() > 0) {
    // Create data visualizations
    float dataHR = dataListHR.get(dataListHR.size() - 1); // get the most recent HR data point
    float dataEDA = dataListEDA.get(dataListEDA.size() - 1); // get the most recent EDA data point
    sendToArduino(dataHR, dataEDA); // Send EDA data to Arduino
  }
}

// --------------------------------------------------- //

// Process incoming OSC message
void oscEvent(OscMessage theOscMessage) {
  if (theOscMessage.checkAddrPattern(oscAddress1)) {
    Object[] args = theOscMessage.arguments();
    for (int n = 0; n < args.length; n++) {
      float data = theOscMessage.get(n).floatValue();
      data = filter(data);
      dataListHR.append(data); // store EDA data for plotting and autoscaling
    }
  } else if (theOscMessage.checkAddrPattern(oscAddress2)) {
    Object[] args = theOscMessage.arguments();
    for (int n = 0; n < args.length; n++) {
      float data = theOscMessage.get(n).floatValue();
      dataListEDA.append(data); // store PPG:IR data for plotting and autoscaling
    }
  }
}

String strHR;
String strEDA;
void sendToArduino(float dataHR, float dataEDA) {
  
  // HR
  strHR = "H"+String.valueOf((int)dataHR)+"\n";
  println("(P -> A) Sending HR data: " + strHR);
  arduinoPort.write(strHR); // Send HR data to Arduino as string
   
  // EDA
  strEDA = "E"+String.valueOf(dataEDA)+"\n";
  println("(P -> A) Sending EDA data: " + dataEDA);
  arduinoPort.write(strEDA); // Send EDA data to Arduino as string
  delay(300);
}

String val;
boolean firstContact = false; // Flag to indicate the initial contact
void serialEvent(Serial myPort) {
  // Put the incoming data into a String - the '\n' is our end delimiter indicating the end of a complete packet
  val = myPort.readStringUntil('\n');
  // Make sure our data isn't empty before continuing
  if (val != null) {
    // Trim whitespace and formatting characters (like carriage return)
    val = trim(val);
    println(val);

    // Look for our 'A' string to start the handshake
    // If it's there, clear the buffer, and send a request for data
    if (firstContact == false) {
      if (val.equals("A")) {
        myPort.clear();
        firstContact = true;
        myPort.write("A");
        println("contact");
      }
    } else {
      // Process the received data
      //if (val.startsWith("E")) {
        //float edaData = float(val.substring(1)); // Parse the EDA data
        //println("EDA" + edaData);
      //} 
      //else if (val.startsWith("P")) {
      //int ppgirData = int(val.substring(1)); // Parse the PPGIR data
      //println("PPGIR data received: " + ppgirData);
      // }
    }
  }
}


float filter(float data) {
  float DIGITAL_FILTER_PI = 3.1415926535897932384626433832795;
  float DIGITAL_FILTER_E = 2.7182818284590452353602874713526;
  float lpAlpha = pow(DIGITAL_FILTER_E, -2.f * DIGITAL_FILTER_PI * lpCut / samplingFreq);
  float hpAlpha = pow(DIGITAL_FILTER_E, -2.f * DIGITAL_FILTER_PI * hpCut / samplingFreq);
  if (lowPass) {
    if (firstFilt) {
      lpFiltVal = data;
    } else {
      lpFiltVal = data * (1. - lpAlpha) + lpFiltVal * lpAlpha;
    }
    //println("filter LP: " + data + ", " + lpFiltVal + ", " + lpAlpha);
    data = lpFiltVal;
  }
  if (highPass) {
    if (firstFilt) {
      hpFiltVal = data;
    } else {
      hpFiltVal = data * (1. - hpAlpha) + hpFiltVal * hpAlpha;
    }
    //println("filter HP: " + data + ", " + hpFiltVal + ", " + hpAlpha);
    data = data - hpFiltVal;
  }
  firstFilt = false;
  return data;
}