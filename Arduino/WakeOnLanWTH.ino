/*  Wake On Lan Web Server

    Based on code from Khoi Hang, thanks Hang :muscle:
*/
// Decides where the Debug info is sent
#define DEBUG_ETHERNET_WEBSERVER_PORT     Serial

// Debug level from 0 to 4
#define _ETHERNET_WEBSERVER_LOGLEVEL_     3

#define NTP_REQUEST_PORT      123

#include <WebServer_WT32_ETH01.h>
#include <AsyncUDP_WT32_ETH01.h>

#include <time.h>

// 0.ca.pool.ntp.org
IPAddress timeServerIP = IPAddress(208, 81, 1, 244);

// this initializes the WebServer, connects via ETH
WebServer server(80);
AsyncUDP Udp;

// REPLACE with your mac address
unsigned char target_mac_address[6] = {0xA8,0xA1,0x59,0x2E,0x5D,0x6E};
unsigned char ff[6] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
unsigned char deviceMac[6] = { 0xAB, 0xCD, 0xEF, 0x12, 0x34, 0x56};

char timeServer[]         = "time.nist.gov";  // NTP server
int counter               = 0;

/* WakeOnLan over UDP implementation */

void sendMagicPacket() {
  printf("Sending magic packet to %", ff);
  char packet[102];
  int i = 0;
  memcpy(packet, ff, 6);
  for ( i = 1; i < 17; i++ ) {
    memcpy(packet+(i*6), target_mac_address, 6);
  }
  // size_t AsyncUDP::broadcastTo(const char * data, uint16_t port, tcpip_adapter_if_t tcpip_if)
  // Udp.broadcast((uint8_t*)packet, 102);
  // broadcast packet
  Udp.broadcast((uint8_t*)packet, sizeof(packet));
}

/* Web Server handling functions */

// prints out board info when IP accessed directly
void handleRoot()
{
  String html = F("Hello from HelloServer running on ");

  html += String(BOARD_NAME);

  server.send(200, F("text/plain"), html);
  
}

// if file on server not found
void handleNotFound()
{
  String message = F("File Not Found\n\n");

  message += F("URI: ");
  message += server.uri();
  message += F("\nMethod: ");
  message += (server.method() == HTTP_GET) ? F("GET") : F("POST");
  message += F("\nArguments: ");
  message += server.args();
  message += F("\n");

  for ( uint8_t i = 0; i < server.args(); i++ )
  {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, F("text/plain"), message);
}

// wakes up device
void handleWake() {
  sendMagicPacket();

  String html = F("Waking up your device! teehee <3 I love you babez");
  server.send(200, F("text/plain"), html);
}

/* Functions for handling UDP packets */

void handlePacket(AsyncUDPPacket packet) {
  Serial.print("UDP Packet Type: ");
  Serial.print(packet.isBroadcast() ? "Broadcast" : packet.isMulticast() ? "Multicast" : "Unicast");
  Serial.print(", From: ");
  Serial.print(packet.remoteIP());
  Serial.print(":");
  Serial.print(packet.remotePort());
  Serial.print(", To: ");
  Serial.print(packet.localIP());
  Serial.print(":");
  Serial.print(packet.localPort());
  Serial.print(", Length: ");
  Serial.print(packet.length());
  Serial.print(", Data: ");
  Serial.write(packet.data(), packet.length());
  Serial.println();

  packet.printf("Got %u bytes of data", packet.length());
}

void setup() {
  // Open serial, wait for port to open
  Serial.begin(115200);
  while (!Serial);

  // prints basic info
  Serial.print("\nStarting WakeOnLan script on " + String(ARDUINO_BOARD));
  Serial.println(" with " + String(SHIELD_TYPE));
  Serial.println(WEBSERVER_WT32_ETH01_VERSION);


  WT32_ETH01_onEvent();
  
  // PHY_POWER = pin number for the external crystal oscillator (16)
  // PHY_ADDR set to 1 by default
  // Initializes the Ethernet connection
  // bool begin(uint8_t phy_addr=ETH_PHY_ADDR, int power=ETH_PHY_POWER, int mdc=ETH_PHY_MDC, int mdio=ETH_PHY_MDIO,
  //           eth_phy_type_t type=ETH_PHY_TYPE, eth_clock_mode_t clk_mode=ETH_CLK_MODE);
  ETH.begin( ETH_PHY_ADDR, ETH_PHY_POWER );

  // This program gets IP from DHCP, does not need to be initialized to a set ip/subnet
  // ETH.config(myIP, myGW, mySN, myDNS);         uncomment to set own ip (See IPAddress type)

  // Waits for connection
  WT32_ETH01_waitForConnect();
  // Connection found ! Do things ...

  // Set up the WebServer
  server.on(F("/"), handleRoot);      // sets basic IP to handleRoot
  server.on(F("/inline"), []() {      // inline fun defeinition for /inline call
    server.send(200, F("text/plain"), F("This works as well"));
  });
  server.onNotFound(handleNotFound);  // sets rest to handleNotFound handler
  server.on(F("/wake"), handleWake);
  // Begins WebServer ...
  server.begin();
  Serial.print(F("HTTP EthernetWebServer is @ IP: "));
  Serial.println( ETH.localIP() );

  // Begin UDP socket
  if (Udp.connect(timeServerIP, NTP_REQUEST_PORT))
  {
    Serial.println("UDP connected");
  }
  if ( Udp.listen(1234) ) {
    Serial.print("UDP Listening on IP: ");
    Serial.println(ETH.localIP());

    Udp.onPacket(handlePacket);
  }

}

void loop() {
  // opens server up, functions uses server attributes for .on() calls
  server.handleClient();
}
