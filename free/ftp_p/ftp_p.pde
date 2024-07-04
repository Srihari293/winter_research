import oscP5.*;
import netP5.*;
import processing.serial.*;
import java.io.PrintWriter;
import java.net.InetAddress;
import java.text.SimpleDateFormat;
import java.util.Calendar;

// ------------ CHANGE PARAMETERS HERE --------------- //
String oscAddress1 = "/EmotiBit/0/HR"; 
String oscAddress2 = "/EmotiBit/0/EDA"; 
int oscPort = 12345;
String arduinoPortName = "COM4"; // Replace with your Arduino's port name
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

PFont f;
String intro = "Welcome to the Pneunochio Experiment!";

// recording statements
int RightPressed=0;
int statement_counter = 0;
boolean calibrationCompleted = false;
float base_line = 0.0;

// CSV writer
PrintWriter output;

void setup() {
  size(1000, 400);
  f = createFont("Arial", 25, true);
  background(255, 0, 0);
  fill(0);
  textFont(f);
  text(intro, (width / 4), height / 2);
  
  /* start oscP5, listening for incoming messages at port 12345 */
  oscP5 = new OscP5(this, oscPort);
  arduinoPort = new Serial(this, arduinoPortName, arduinoBaudRate);
  
  // Generate unique filename with laptop ID and timestamp
  String hostname = "laptop1";
  String timestamp = getTimestamp();
  String filename = "data_" + hostname + "_" + timestamp + ".csv";
  
  // Initialize CSV file
  output = createWriter(filename);
  output.println("Date,Time,Heart Rate,Baseline");
}

void draw() {
  if (dataListHR.size() > 0 && dataListEDA.size() > 0) {
    // Create data visualizations
    float dataHR = dataListHR.get(dataListHR.size() - 1); // get the most recent HR data point
    float dataEDA = dataListEDA.get(dataListEDA.size() - 1); // get the most recent EDA data point
  
    if (keyPressed) {
      if (key == 's') {
        println("Starting");  
        arduinoPort.write("C");
        background(255, 255, 255);
      }
    }
    
    // Log data to CSV file
    logData(dataHR, base_line);
    sendToArduino(dataHR, dataEDA); // Send EDA data to Arduino
  }
}

void logData(float heartRate, float baseline) {
  // Get current date and time
  String date = nf(day(), 2) + nf(month(), 2) + nf(year(), 2);
  String time = nf(hour(), 2) + ":" + nf(minute(), 2) + ":" + nf(second(), 2);
  
  // Write data to CSV file
  output.println(date + "," + time + "," + heartRate + "," + baseline);
  output.flush(); // Ensure data is written to the file
}

// Process incoming OSC message
void oscEvent(OscMessage theOscMessage) {
  if (theOscMessage.checkAddrPattern(oscAddress1)) {
    Object[] args = theOscMessage.arguments();
    for (int n = 0; n < args.length; n++) {
      float data = theOscMessage.get(n).floatValue();
      data = filter(data);
      if (data < 170.0 && data > 0.0) {
        dataListHR.append(data); // store HR data for plotting and autoscaling     
      }
    }
  } else if (theOscMessage.checkAddrPattern(oscAddress2)) {
    Object[] args = theOscMessage.arguments();
    for (int n = 0; n < args.length; n++) {
      float data = theOscMessage.get(n).floatValue();
      if (data < 2.0 && data > 0.0) {
        dataListEDA.append(data); // store EDA data for plotting and autoscaling
      }
    }
  }
}

String strHR;
String strEDA;
void sendToArduino(float dataHR, float dataEDA) {
  // HR
  strHR = "H" + String.valueOf((int) dataHR) + "\n";
  arduinoPort.write(strHR); // Send HR data to Arduino as string
   
  // EDA
  strEDA = "E" + String.valueOf(dataEDA) + "\n";
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
      if (val.equals("B")) {
        myPort.clear();
        firstContact = true;
        myPort.write("A");
        println("contact");
      }
    } else {
      // Process the received data
      if (val.equals("Starting")) {
        println("Starting emotion monitor");
      }
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
    data = lpFiltVal;
  }
  if (highPass) {
    if (firstFilt) {
      hpFiltVal = data;
    } else {
      hpFiltVal = data * (1. - hpAlpha) + hpFiltVal * hpAlpha;
    }
    data = data - hpFiltVal;
  }
  firstFilt = false;
  return data;
}

String getHostname() {
  try {
    InetAddress inetAddress = InetAddress.getLocalHost();
    return inetAddress.getHostName();
  } catch (Exception e) {
    e.printStackTrace();
    return "unknown";
  }
}

String getTimestamp() {
  Calendar cal = Calendar.getInstance();
  SimpleDateFormat sdf = new SimpleDateFormat("ddMMyyyyHHmmss");
  return sdf.format(cal.getTime());
}
