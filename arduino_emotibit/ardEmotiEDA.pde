// Reads EmotiBit data from an OSC stream and plots data in a window

import oscP5.*;
import netP5.*;
import processing.serial.*;

// ------------ CHANGE PARAMETERS HERE --------------- //
// Look in EmotiBit Oscilloscope/data/oscOutputSettings.xml for EmotiBit OSC port and addresses
String oscAddress = "/EmotiBit/0/EDA"; 
String oscAddress2 = "/EmotiBit/0/PPG:IR"; 
int oscPort = 12345;
String arduinoPortName = "COM7"; // Replace with your Arduino's port name
int arduinoBaudRate = 9600; // Set the same baud rate as used in the Arduino code
// --------------------------------------------------- //

// Change these variables to change the filters
float samplingFreq = 25; // change to match the sampling frequency of the data
boolean lowPass = false; // toggles on/off the low-pass filter
float lpCut = 3; // adjusts the cut frequency of the low-pass filter
boolean highPass = false; // toggles on/off the high-pass filter
float hpCut = 1; // adjusts the cut frequency of the high-pass filter

OscP5 oscP5;
FloatList dataListEDA = new FloatList();
FloatList dataListPPGIR = new FloatList();
Serial arduinoPort; // Serial object for Arduino communication

// filter variables
float lpFiltVal;
float hpFiltVal;
boolean firstFilt = true;

// --------------------------------------------------- //

void setup() {
  size(620, 840);
  /* start oscP5, listening for incoming messages at port 12345 */
  oscP5 = new OscP5(this, oscPort);
  arduinoPort = new Serial(this, arduinoPortName, arduinoBaudRate);
}

// --------------------------------------------------- //

void draw() {
  if (dataListEDA.size() > 0 && dataListPPGIR.size() > 0) {
    // Create data visualizations
    float dataEDA = dataListEDA.get(dataListEDA.size() - 1); // get the most recent EDA data point
    float dataPPGIR = dataListPPGIR.get(dataListPPGIR.size() - 1); // get the most recent PPG:IR data point
    sendToArduino(dataEDA, dataPPGIR); // Send EDA data to Arduino
  }
}

// --------------------------------------------------- //

// Process incoming OSC message
void oscEvent(OscMessage theOscMessage) {
  if (theOscMessage.checkAddrPattern(oscAddress)) {
    Object[] args = theOscMessage.arguments();
    for (int n = 0; n < args.length; n++) {
      float data = theOscMessage.get(n).floatValue();
      data = filter(data);
      dataListEDA.append(data); // store EDA data for plotting and autoscaling
    }
  } else if (theOscMessage.checkAddrPattern(oscAddress2)) {
    Object[] args = theOscMessage.arguments();
    for (int n = 0; n < args.length; n++) {
      float data = theOscMessage.get(n).floatValue();
      dataListPPGIR.append(data); // store PPG:IR data for plotting and autoscaling
    }
  }
}

String strEDA;
void sendToArduino(float dataEDA, float dataPPGIR) {
  //arduinoPort.write("E");
  //println(dataEDA);
  //dataEDA = (long) (1000000 * dataEDA);
  //println("Sending EDA data: " + int(dataEDA));
  
  // EDA
  strEDA = "E"+String.valueOf(dataEDA)+"\n";
  println("Sending EDA data: " + strEDA);
  arduinoPort.write(strEDA); // Send EDA data to Arduino as string
  delay(175);
  //arduinoPort.write("P");
  //println("Sending PPGIR data: " + (int)dataPPGIR);
  //arduinoPort.write(String.valueOf((int)dataPPGIR)); // Send PPG:IR data to Arduino as string
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
