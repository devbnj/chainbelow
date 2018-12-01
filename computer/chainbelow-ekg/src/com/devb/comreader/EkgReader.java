/*
 * EkgReader.java
 *
 *  Created on: Apr 24, 2015
 *      Author: DevbNJ
 *  Copyright and Product Acknowledgments:
 *  	RXTX Library - http://rxtx.qbang.org/wiki/index.php/Main_Page
 *      Arduino IDE - http://playground.arduino.cc/interfacing/java
 *  Distributed under Open Source Apache License
 *  Copyright Devb Inc. All Rights Reserved
 *  Please do not remove the copyright notice
 */

package com.devb.comreader;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;

import gnu.io.CommPortIdentifier;
import gnu.io.SerialPort;
import gnu.io.SerialPortEvent;
import gnu.io.SerialPortEventListener;

import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Enumeration;
import java.util.Vector;

// Used by baggers
class Ekg {
	int adc;
	int timer;
}

// Used by baggers
class Hr {
	int r;
	int timer;
}

/**
 * *********************************************************
 * Main class: EkgReader
 * @author DevbNJ
 * *********************************************************
 */
public class EkgReader implements SerialPortEventListener {

	SerialPort serialPort;
	// Collect data.
	public Vector<Ekg> ekgBagger = new Vector<Ekg>();
	// Collect Panchanga data.
	public Vector<String> panchanga = new Vector<String>();
	// ADC Channel
	int channel = 0;
	// For all channels (in case of multiple channels)
	boolean channel_do = false;
	double dt1 = 0.0;
	// EKG Data for the system
	Ekg ekg = new Ekg();
	// Buffered input
	private BufferedReader brInput;
	// The output stream 
	private OutputStream osOutput;
	// Port time out in Milisecs
	private static final int TIMEOUT = 2000;
	// Baud rate 
	private static final int BAUDRATE = 57600;
	// Data smoothing
	int[] dataSmoothPts;

	// The communication port used for EKG ADC inputs
	private static final String PORT_NAMES[] = { 
		"/dev/tty.usbserial-A9007UX1", // Mac OSX
			"/dev/ttyACM0", // Raspberry Pi
			"/dev/ttyUSB0", // Linux
			"COM2", // Windows
	};
	

	/**
	 * method: determineCurvePts
	 * @param dataPoints
	 */
	public void determineCurvePts(Integer[] dataPoints) {
		// Determine size
		int n = dataPoints.length - 1;

		// Compute Bezier points - right side array
		double[] rightArray = new double[n];

		// Set X coordinate values
		for (int i=1; i < n-1; i++) {
			rightArray[i] = 4 * dataPoints[i].intValue() + 2
					* dataPoints[i + 1].intValue();
		}
		
		rightArray[0] = dataPoints[0].intValue() + 2 * dataPoints[1].intValue();
		rightArray[n - 1] = (8 * dataPoints[n - 1].intValue() + dataPoints[n]
				.intValue()) / 2.0;
		
		// Get control points X-values
		double[] x = determineControlPts(rightArray);

		// Fill output arrays.
		dataSmoothPts = new int[n];
		for (int i = 0; i < n; ++i) {
			// First control point
			dataSmoothPts[i] = (int) x[i];
		}
	}

	/**
	 * method: determineControlPts
	 * @param rightArray
	 * @return
	 */
	private double[] determineControlPts(double[] rightArray) {
		int arrayLen = rightArray.length;
		// results to be returned
		double[] results = new double[arrayLen]; 
		// throwaway
		double[] throwAway = new double[arrayLen]; 

		double b = 2.0;
		results[0] = rightArray[0] / b;
		// decomposition and forward substitution.
		for (int i = 1; i < arrayLen; i++) { 
			throwAway[i] = 1 / b;
			b = (i < arrayLen - 1 ? 4.0 : 3.5) - throwAway[i];
			results[i] = (rightArray[i] - results[i - 1]) / b;
		}
		// backsubstitution.
		for (int i = 1; i < arrayLen; i++) {
			results[arrayLen - i - 1] -= throwAway[arrayLen - i] * results[arrayLen - i]; 
		}
		return results;
	}

	
	/**
	 * method: begin
	 * @param: none
	 * @return: none
	 */
	public void begin() {
		// System.setProperty("gnu.io.rxtx.SerialPorts", "/dev/ttyACM0");
		System.setProperty("gnu.io.rxtx.SerialPorts", "COM2");

		CommPortIdentifier portId = null;
		Enumeration portEnum = CommPortIdentifier.getPortIdentifiers();

		// First, Find an instance of serial port as set in PORT_NAMES.
		while (portEnum.hasMoreElements()) {
			CommPortIdentifier currPortId = (CommPortIdentifier) portEnum
					.nextElement();
			for (String portName : PORT_NAMES) {
				if (currPortId.getName().equals(portName)) {
					portId = currPortId;
					break;
				}
			}
		}
		if (portId == null) {
			System.out.println("Could not find COM port.");
			return;
		}

		try {
			// open serial port, and use class name for the appName.
			serialPort = (SerialPort) portId.open(this.getClass().getName(), TIMEOUT);

			// set port parameters
			serialPort.setSerialPortParams(BAUDRATE, SerialPort.DATABITS_8,
					SerialPort.STOPBITS_1, SerialPort.PARITY_NONE);

			// open the streams
			brInput = new BufferedReader(new InputStreamReader(serialPort.getInputStream()));
			osOutput = serialPort.getOutputStream();

			// add event listeners
			serialPort.addEventListener(this);
			serialPort.notifyOnDataAvailable(true);
		} catch (Exception e) {
			System.err.println(e.toString());
		}
	}

	/**
	 * Shutdown Port
	 * Method: close
	 * @param: none
	 * @return: none
	 */
	public synchronized void close() {
		if (serialPort != null) {
			serialPort.removeEventListener();
			serialPort.close();
		}
	}

	/**
	 * Event handlers
	 * Method: serialEvent
	 * @param: SerialPortEvent oEvent
	 * @return: none
	 * Prefix(es) 
	 * ADC - for analog / digital data
	 * PCH - for panchanga
	 * HDR - headers
	 */
	public synchronized void serialEvent(SerialPortEvent oEvent) {
		if (oEvent.getEventType() == SerialPortEvent.DATA_AVAILABLE) {
			try {
				String inputLine = brInput.readLine();
				String delim = ",";
				String[] st = inputLine.split(delim);
				Integer dt = null;
				Integer tm = null;
				// For several channels
				if (channel_do) {
					if (st[0].equals("ADC")) {
						dt = new Integer(st[1]);
						tm = new Integer(st[2]);
						ekg = new Ekg();
						ekg.adc = dt;
						ekg.timer = tm;
						if (channel++ == 0) {
							dt1 = dt;
						}
					}
					if (channel == 1) {
						if (st[0].equals("ADC")) {
							ekgBagger.add(ekg);
							System.out.println(ekg.adc);
							dt1 = 0;
						} else if (st[0].equals("PCH")) {
							panchanga.add(st[1]);
						}
					}
					if (channel > 11) {
						channel = 0;
					}
				// Only one channel	
				} else { 
					if (st[0].equals("ADC")) {
						dt = new Integer(st[1]);
						tm = new Integer(st[2]);
						ekg = new Ekg();
						ekg.adc = dt;
						ekg.timer = tm;
						// check for overshoots
						if ((dt.intValue() > 0) && (dt.intValue() < 1025)) {
							ekgBagger.add(ekg);
						}
					} else if (st[0].equals("PCH")) {
						panchanga.add(st[1]);
					}
					System.out.println(inputLine);
				}
			// On exception - do nothing	
			} catch (Exception e) { /*e.printStackTrace();*/ }
		}
	}

	/**
	 * Method: main
	 * @param args
	 * @throws Exception
	 */
	public static void main(String[] args) throws Exception {
		// Buffer for file I/O
		StringBuffer sBuffer;
		// Initialize ekg on serial port
		EkgReader ekgreader = new EkgReader();
		// Setup the file (output)
		FileWriter fw;
		// start the ekg read
		ekgreader.begin();
		// determine start time
		long t = System.currentTimeMillis();
		// set the end time (1 minute)
		long end = t + 60000;
		
		// initialize the buffer
		sBuffer = new StringBuffer("var y = [");
		
		// wait for the thread to complete
		while (System.currentTimeMillis() < end) {
			try {
				Thread.sleep(60000);
			} catch (InterruptedException ie) {	}
		}
		
		System.out.println("Ekg read");
		ekgreader.close();

		Ekg[] inData = ekgreader.ekgBagger.toArray(new Ekg[ekgreader.ekgBagger.size()]);
		/*
		 * Hold this thought
		 * 
		ekg.determineCurvePts(inData);
		
	    for (int i=0; i< ekg.dataSmoothPts.length; i++) {
	        sBuffer.append(new Integer(ekg.dataSmoothPts[i]).toString());
	        sBuffer.append(", ");
	    }
	    */
		// Heart Rate
		Hr hr = new Hr();
		Vector<Hr> vhr = new Vector<Hr>();
		int maxR = 0;
		int r = 0;
		boolean flag = false;
		int fTimer = inData[0].timer;
		int lTimer = inData[inData.length-1].timer;
	    for (int i=0; i< inData.length; i++) {
	    	r = inData[i].adc; 
	    	if (r > maxR) { // rising curve could be P,Q,R
	    		maxR = r; 
	    		flag = false;
	    	}
	    	if (r < maxR) { // falling curve
	    		if (!flag) {
	    			hr = new Hr();
	    			hr.r = r;
	    			hr.timer = inData[i].timer;
	    			vhr.add(hr);
	    		}
	    		if (!flag) flag = true;
	    	}
	    	sBuffer.append("[");
	        sBuffer.append(inData[i].adc);
	        sBuffer.append(",");
	        sBuffer.append(inData[i].timer);
	        sBuffer.append("],");
	    }
	    sBuffer.append("[0,0]];");
	    int heartRate = vhr.size() / (3 * (lTimer - fTimer)) * 60;
	    System.out.println("Heart Rate = "+heartRate);
		try {
			DateFormat dateFormat = new SimpleDateFormat("yyyy/MM/dd HH:mm:ss");
			Calendar cal = Calendar.getInstance();
			fw = new FileWriter(new File("graph/ekg.html"));
			fw.write("<!DOCTYPE html>\n");
			fw.write("<html>\n");
			fw.write("<head>\n");
			fw.write("<title>EKG Plot</title>\n");
			fw.write("<!-- Generated code -->\n");
			fw.write("<meta http-equiv=\"Content-Type\"\n");
			fw.write("	content=\"text/html; charset=utf-8\">\n");
			fw.write("<style>\n");
			fw.write("canvas {\n");
			fw.write("    background:#000;\n");
			fw.write("    cursor:crosshair;\n");
			fw.write("}\n");
			fw.write("</style>\n");
			fw.write("</head>\n");
			fw.write("<body>\n");
			fw.write("<h1>EKG Plot - "+dateFormat.format(cal.getTime())+"</h1>\n");
			fw.write("<canvas id=\"demo\" width=1000 height=400></canvas><br>\n");
			fw.write("<p>EKG Graph</p>\n");
			fw.write("<hr />\n");
			fw.write("<h1>Panchanga</h1>\n");
			Enumeration<String> ve = ekgreader.panchanga.elements();
			fw.write("<ul>");
			while(ve.hasMoreElements())
				fw.write("<li>"+ve.nextElement().toString()+"</li>\n");
			fw.write("</ul>");
			fw.write("<script>\n");
			fw.write("var canvas = document.getElementById(\"demo\");\n");
			fw.write("var ctx = canvas.getContext(\"2d\");\n");
			fw.write("var w = demo.width,\n");
			fw.write("    h = demo.height,\n");
			fw.write("    px = 0, opx = 0, speed = 3,\n");
			fw.write("    py = h * 0.8, opy = py,\n");
			fw.write("    jj = 0;\n");
			fw.write("    scanBarWidth = 20;\n");
			fw.write("\n");
			fw.write("\n");
			fw.write("ctx.strokeStyle = '#00bd00';\n");
			fw.write("ctx.lineWidth = 3;\n");
			fw.write("\n");
			fw.write(sBuffer.toString()); 
			fw.write("\n");
			fw.write("\n");
			fw.write("loop();\n");
			fw.write("\n");
			fw.write("console.log(y[0][0]); console.log(y[0][1]); console.log(y[1][0]);\n");
			fw.write("function loop() {\n");
			fw.write("\n");
			fw.write("    px += speed;\n");
			fw.write("\n");
			fw.write("    py = Math.floor(y[jj][0]/10);\n");
			fw.write("    jj += 60;\n");
			fw.write("    if (jj >= y.length-1) jj = 0;\n");
			fw.write("\n");
			fw.write("    ctx.clearRect(px,0, scanBarWidth, h);\n");
			fw.write("    ctx.beginPath();\n");
			fw.write("    ctx.moveTo(opx, opy);\n");
			fw.write("    ctx.lineTo(px, py);\n");
			fw.write("    ctx.stroke();\n");
			fw.write("\n");
			fw.write("    opx = px;\n");
			fw.write("    opy = py;\n");
			fw.write("\n");
			fw.write("    if (opx > w) {\n");
			fw.write("        px = opx = -speed;\n");
			fw.write("    }\n");
			fw.write("\n");
			fw.write("    requestAnimationFrame(loop);\n");
			fw.write("}\n");
			fw.write("\n");
			fw.write("</script>\n");
			fw.write("</body>\n");
			fw.write("</html>\n");
			
			fw.close();
		} catch (IOException ex) {
			ex.printStackTrace();
		}
		System.out.println("Done");
	}
}
