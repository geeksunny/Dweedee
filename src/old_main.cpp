#include <Arduino.h>
#include <SPI.h>
#include <midi_Defs.h>
#include <MIDI.h>
//#include <PrintStream.h>

// #define DEBUG_USB_HOST
#include <usbh_midi.h>
#include <usbhub.h>

// #ifdef USBCON
#define _MIDI_SERIAL_PORT Serial1
// #else
// #define _MIDI_SERIAL_PORT Serial
// #endif
// Create serial midi interface as MIDI
MIDI_CREATE_INSTANCE(HardwareSerial, _MIDI_SERIAL_PORT, MIDI);

#define LOBYTE(x) ((char*)(&(x)))[0]
#define HIBYTE(x) ((char*)(&(x)))[1]

USB Usb;
USBHub Hub(&Usb);
USBH_MIDI UsbMidi(&Usb);

void pollUsbMidi();
void pollSerialMidi();
void doDelay(uint32_t t1, uint32_t t2, uint32_t delayTime);
byte getstrdescr( byte addr, byte idx );
void printStringDescriptors(UsbDevice *pdev);

// TODO: sysex handling
// TODO: USB HUB SUPPORT

void setup() {
    Serial.begin(9600);
    Serial.println("Started Serial output.");

    Serial.println("Starting MIDI interface.");
    MIDI.begin(MIDI_CHANNEL_OMNI);
    // Disabling built-in thru handling in favor of testing manual thru handling.
    MIDI.turnThruOff();
    Serial.println("Started MIDI interface.");

    Serial.println("Starting USB interface.");
    if (Usb.Init() == -1) {
        while (1); //halt
    }
    Serial.println("Started USB interface.");
    delay(200);
    Serial.println("Ready.");
}

void loop() {
    Usb.Task();
    // uint32_t t1 = (uint32_t)micros();
    if ( UsbMidi ) {
        pollUsbMidi();
        pollSerialMidi();
    //} else {
        // Serial.println("Skipped.");
    }

//    if ( Usb.getUsbTaskState() == USB_STATE_RUNNING ) {
//        Serial.println("Starting descriptor print.");
//        Usb.ForEachUsbDevice(&printStringDescriptors);
//
//        while ( 1 ) { // stop
//#ifdef ESP8266
//            yield(); // needed in order to reset the watchdog timer on the ESP8266
//#endif
//        }
//    }

    // doDelay(t1, (uint32_t)micros(), 100);
}

// Delay time (max 16383 us)
void doDelay(uint32_t t1, uint32_t t2, uint32_t delayTime) {
    uint32_t t3;

    t3 = t2 - t1;
    if ( t3 < delayTime ) {
        delayMicroseconds(delayTime - t3);
    }
}

// Poll USB MIDI controller and send to serial MIDI
void pollUsbMidi() {
    uint8_t size;
    // TODO: sysex handling
    // https://github.com/YuuichiAkagawa/USBH_MIDI/blob/master/examples/bidirectional_converter/bidirectional_converter.ino#L99
    uint8_t outBuf[3];
    do {
        if ((size = UsbMidi.RecvData(outBuf)) > 0) {
            // midi::MidiType type = MIDI.getTypeFromStatusByte(outBuf[0]);
            // if (type == midi::Clock) {
            //     continue;
            // }
            // midi::Channel channel = MIDI.getChannelFromStatusByte(outBuf[0]);
            // Serial.print("USB | Type: ");
            // Serial.print(type, HEX);
            // Serial.print(" | Channel: ");
            // Serial.println(channel, DEC);

            // TODO: Parse values from outBuf
            // MIDI.send(type, outBuf[1], outBuf[2], channel);
            // Alternatively, publish directly to serial port.
            _MIDI_SERIAL_PORT.write(outBuf, size);
        }
    } while (size > 0);
}

// Poll Serial MIDI controller and send to USB MIDI
void pollSerialMidi() {
    if (MIDI.read()) {
        // Serial.println("MIDI data IN");
        midi::MidiType type = MIDI.getType();
        midi::Channel channel = MIDI.getChannel();
        // if (type == midi::Clock) {
        //     return;
        // }
        uint8_t msg[4];
        switch(type) {
            case midi::ActiveSensing :
                // TODO ??
                // Serial.println("MIDI ACTIVE SENSING");
                // break;
            case midi::SystemExclusive :
                // TODO
                // Serial.println("MIDI SYS EX");
                // break;
            default :
                // Serial.print("Serial | Type: ");
                // Serial.print(type, HEX);
                // Serial.print(" | Channel: ");
                // Serial.println(channel, DEC);
                // Status byte
                // msg[0] = (type | ((channel - 1) & 0x0f));
                msg[0] = type;
                // Data bytes
                msg[1] = MIDI.getData1();
                msg[2] = MIDI.getData2();
                // Send to USB
                UsbMidi.SendData(msg, 0);
                Usb.Task();  // Probably not necessary
                // if (result == 0) {
                //     Serial.println("UsbMidi.SendData(msg, 0) SUCCESS");
                // } else {
                //     Serial.println("UsbMidi.SendData FAILED");
                // }
                // Send to serial (manual-thru mode)
                // MIDI.send(type, msg[1], msg[2], channel);
                // Alternatively, publish directly to serial port.
                _MIDI_SERIAL_PORT.write(msg, 3);
                break;
        }
    }
}

void printStringDescriptors(UsbDevice *pdev) {
    USB_DEVICE_DESCRIPTOR deviceDescriptor;
    byte rcode;
    rcode = Usb.getDevDescr( pdev->address.devAddress, 0, 0x12, ( uint8_t *)&deviceDescriptor );
    if ( rcode ) {
        Serial.println( rcode, HEX );
    }

    for (int i = 1; i <= 5; i++) {
        rcode = getstrdescr( pdev->address.devAddress, i);                 //get string descriptor
        if( rcode ) {
            Serial.println( rcode, HEX );
        }
    }
//    while( 1 );                          //stop
}

byte getstrdescr( byte addr, byte idx ) {
    uint8_t buf[ 66 ];
    byte rcode;
    byte length;
    byte i;
    unsigned int langid;
    rcode = Usb.getStrDescr( addr, 0, 1, 0, 0, buf );  //get language table length
    if( rcode ) {
        Serial.println("Error retrieving LangID table length");
        return( rcode );
    }
    length = buf[ 0 ];      //length is the first byte
    rcode = Usb.getStrDescr( addr, 0, length, 0, 0, buf );  //get language table
    if( rcode ) {
        Serial.println("Error retrieving LangID table");
        return( rcode );
    }
    HIBYTE( langid ) = buf[ 3 ];                            //get first langid
    LOBYTE( langid ) = buf[ 2 ];
    rcode = Usb.getStrDescr( addr, 0, 1, idx, langid, buf );
    if( rcode ) {
        Serial.println("Error retrieving string length");
        return( rcode );
    }
    length = buf[ 0 ];
    rcode = Usb.getStrDescr( addr, 0, length, idx, langid, buf );
    if( rcode ) {
        Serial.println("Error retrieving string");
        return( rcode );
    }

//    Serial << "Length: " << length << endl;
    for( i = 2; i < length; i+=2 ) {
        Serial.print( (char) buf[ i ] );
    }

    Serial.print("\n");
    return( rcode );
}
