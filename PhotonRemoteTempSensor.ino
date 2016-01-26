//This code was written by Ace Levenburg <acelevenberg@gmail.com> and Jennifer Fox <jenfoxbot@gmail.com>
/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <jenfoxbot@gmail.com> and <acelevenburg@gmail.com> wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.
 * ----------------------------------------------------------------------------
 */


// This #include statement was automatically added by the Particle IDE.
//This library is used to push data to the data.sparkfun.com server.
#include "SparkFunPhant/SparkFunPhant.h"

const char server[] = "data.sparkfun.com"; // Phant destination server
const char publicKey[] = "VGXjREG6wZIzrZdMvjwA"; // Phant public key
const char privateKey[] = "9Yg650Y1ERtMz12wdEWn"; // Phant private key

Phant phant(server, publicKey, privateKey); // Create a Phant object


const unsigned long postingRate = 20000; //Post rate to data.sparkfun.com (time in milliseconds)
unsigned long lastPost = millis();

const int TEMP102_ADDRESS = 72; //Address of TMP102
const int BYTES_TO_READ = 2; //Number of bytes to read in from TMP102 (should always be 2)

void setup() {
    Wire.begin(); //Initialize serial communication library
    Serial.begin(9600);
}

void loop() {
    if (Wire.isEnabled()) //Check if receiving a signal from I2C pins
    {
        if (lastPost + postingRate < millis()) //Wait to post until ~ 20s has lapsed
        {  
            Serial.print("Requesting from :");
            Serial.println(TEMP102_ADDRESS);
            Wire.requestFrom(TEMP102_ADDRESS, BYTES_TO_READ);
            if (Wire.available() == BYTES_TO_READ)
            {
                Serial.println("Reading!");
                byte MSB = Wire.read();
                byte LSB = Wire.read();
                int temp = ((( MSB << 8) | LSB) >> 4) * 0.0625; //Remove 4 empty bits from 2nd byte (see datasheet), 
                //combine 1st byte and 2nd byte then use conversion factor to get temp in deg. C (see datasheet)
                Serial.print("Temp is :"); 
                Serial.println(temp);
                postToPhant(temp, 'C'); //Post temperature data and unit (deg C) to your data stream at data.sparkfun.com (See lines 68 and on)
                lastPost = millis();
            }
            else
            {
                Serial.println("Unable to read the temperature"); //Used for debugging TMP102 output
            }
        }
    }
    else 
    {
        Serial.println("Wire is not enabled make sure to call Wire.begin()"); //Used for debugging I2C protocol
    }
}

//Thanks Jim Lindblom <jim@sparkfun.com> for the sample code and Phant library.
int postToPhant(int temp, char unit){
    
    phant.add("temp", temp); //Data stream field name "temp"
    phant.add("unit", unit); //Data stream field name "unit"
    TCPClient client;
    char response[512];
    int i = 0;
    int retVal = 0;
    
    if (client.connect(server, 80)) // Connect to the server
    {
		// Post message to indicate connect success
        Serial.println("Posting!"); 
		
		// phant.post() will return a string formatted as an HTTP POST.
		// It'll include all of the field/data values we added before.
		// Use client.print() to send that string to the server.
        client.print(phant.post());
        delay(1000);
		// Now we'll do some simple checking to see what (if any) response
		// the server gives us.
        while (client.available())
        {
            char c = client.read();
            Serial.print(c);	// Print the response for debugging help.
            if (i < 512)
                response[i++] = c; // Add character to response string
        }
		// Search the response string for "200 OK", if that's found the post
		// succeeded.
        if (strstr(response, "200 OK"))
        {
            Serial.println("Post success!");
            retVal = 1;
        }
        else if (strstr(response, "400 Bad Request"))
        {	// "400 Bad Request" means the Phant POST was formatted incorrectly.
			// This most commonly ocurrs because a field is either missing,
			// duplicated, or misspelled.
            Serial.println("Bad request");
            retVal = -1;
        }
        else
        {
			// Otherwise we got a response we weren't looking for.
            retVal = -2;
        }
    }
    else
    {	// If the connection failed, print a message:
        Serial.println("connection failed");
        retVal = -3;
    }
    client.stop();	// Close the connection to server.
    return retVal;	// Return error (or success) code.
}
