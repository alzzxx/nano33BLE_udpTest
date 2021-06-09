
#include <SPI.h>
#include <EthernetCustom.h>

#define ET_PIN_NSS 10
#define ET_PIN_HI digitalWrite(ET_PIN_NSS, HIGH)

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED}; // MAC address of arduino board
unsigned int localPort = 8888;                     // Arduino local port
unsigned int remotePort = 58769;                   // UDP port of remote PC
char imageName[10] = "ImageNum_";                  // File name string
char fileName[15];                                 // char to be sent to PC
int n = 0;                                         // number of image sent

IPAddress ip(192, 168, 1, 10);            // ip address of arduino board
IPAddress subnet(255, 255, 255, 0);       // subnet for arduino
IPAddress remote(192, 168, 1, 80);        // ip address of PC
IPAddress remotesubnet(255, 255, 255, 0); // subnet for pc

EthernetUDP Udp;
bool shieldStart(void);

void setup()
{
    Serial.begin(115200);
    delay(3000);
    Serial.println(F("Starting UDP test"));
    while (!shieldStart())
    {
        Serial.println(F("Shield not started, retrying"));
        delay(10);
    }
    Serial.println(F("Shield started, now sending test packets"));
}

void loop()
{
    Serial.print("I'm on main loop, sending packet number: ");
    Serial.println(n);
    delay(1000);
    sprintf(fileName, "%s%i", imageName, n);
    Udp.beginPacket(remote, remotePort); // intiate UDP communications
    Udp.write(fileName);                 // send packet
    Udp.endPacket();
    Serial.print("Send packet called: ");
    Serial.println(fileName);
    n++;
    delay(1000);
}

bool shieldStart(void)
{
    bool shieldOK, hardOK, linkOK, udpOK;
    pinMode(ET_PIN_NSS, OUTPUT);
    ET_PIN_HI;
    delay(200);
    Serial.println(F("Starting shield ethernet"));
    Ethernet.init(ET_PIN_NSS); // activate CS for ethernet shield
    Ethernet.begin(mac, ip);   // start the ethernet
    Serial.println(F("Ethernet began"));

    uint8_t i = 0;

    while (i < 10)
    {
        if (Ethernet.hardwareStatus() == EthernetNoHardware)
        {
            i++;
            Serial.println(F("Ethernet shield was not found, oh no!"));
            hardOK = false;
            delay(10);
        }
        else if (Ethernet.hardwareStatus() == EthernetW5500)
        {
            Serial.println(F("W5500 controller detected, right on!"));
            hardOK = true;
            break;
        }
    }

    i = 0;
    while (i < 10)
    {
        if (Ethernet.linkStatus() == LinkOFF)
        {
            i++;
            Serial.println(F("You need to connect the ethernet cable, it's obviously disconnected"));
            linkOK = false;
            delay(10);
        }
        else if (Ethernet.linkStatus() == LinkON)
        {
            Serial.println(F("Nice, ethernet cable connected"));
            linkOK = true;
            break;
        }
        else
        {
            Serial.println(F("This isn't the controller you're looking for (but at least Link is not OFF) ;)"));
            linkOK = true;
            break;
        }
    }

    i = 0;

    while (i < 10)
    {
        /* 
        * try to start Udp, if everhthing is ok return isUdpOK true
        */
        if (!Udp.begin(localPort))
        {
            i++;
            Serial.println("Error connecting device. UDP socket not available, arghhh!");
            udpOK = false;
            delay(10);
        }
        else
        {
            Serial.println("Connection Udp OK, sweet baby. . .");
            Serial.print("Local IP address: ");
            for (int y = 0; y < 4; y++)
            {
                Serial.print(ip[y]);
                if (y < 3)
                    Serial.print(".");
            }
            Serial.print(", port: ");
            Serial.println(localPort);
            udpOK = true;
            break;
        }
    }

    shieldOK = hardOK && linkOK && udpOK;
    if (shieldOK)
    {
        Serial.println("All right let's send those Udp packets!");
    }
    else
    {
        Serial.println("Dude, there is something wrong with the shield :(");
    }
    return shieldOK;
}
