#include <mbed.h>
#include <MQTTClientMbedOs.h>

DigitalOut led(LED1);
InterruptIn button(USER_BUTTON);
Thread t;
EventQueue queue(5 * EVENTS_EVENT_SIZE);
Serial pc(USBTX, USBRX);

WiFiInterface *wifi;
TCPSocket socket;
MQTTClient client(&socket);

const char* hostname = "mqtt.netpie.io"; 
uint16_t  port = 1883;

SocketAddress socketAddr(hostname, port);

const char* mqtt_client = "10b7201e-f3e8-44d3-976f-6b4748194c41";
const char* mqtt_username = "ridwJi5E8yyRrYcktLG5d3sdL5jfs1a7";
const char* mqtt_password = ".*P7j63elKqV0*kaALxgxESmq5dnLA*J";

const char* topic = "ICT710";

MQTT::Message message;


void  connect_wifi() {
    wifi = WiFiInterface::get_default_instance();
    if (!wifi) {
        printf("ERROR: No WiFiInterface found.\n");
        return;
    }

    pc.printf("\nConnecting to %s...\n\r", MBED_CONF_APP_WIFI_SSID);

    int ret = wifi->connect(MBED_CONF_APP_WIFI_SSID, MBED_CONF_APP_WIFI_PASSWORD, NSAPI_SECURITY_WPA_WPA2);
    if (ret != 0) {
        pc.printf("\nConnection error: %d\n", ret);
        return;
    }

    pc.printf("Success\n\n\r");
    pc.printf("MAC: %s\n\r", wifi->get_mac_address());
    SocketAddress a;
    wifi->get_ip_address(&a);
    pc.printf("IP: %s\n\r", a.get_ip_address());
    wifi->get_netmask(&a);
    pc.printf("Netmask: %s\n\r", a.get_ip_address());
    wifi->get_gateway(&a);
    pc.printf("Gateway: %s\n\r", a.get_ip_address());
    pc.printf("RSSI: %d\n\n\r", wifi->get_rssi());

    //wifi->disconnect();
    //pc.printf("\nDone\n");    
}

void connect_netpie() {
    socket.open(wifi);
    socket.connect(socketAddr);

    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    
    data.clientID.cstring = (char *)mqtt_client;
    data.username.cstring = (char *)mqtt_username;
    data.password.cstring = (char *)mqtt_password;
    pc.printf("Connecting to NETPIE...\n\r");
    while(!client.isConnected()) 
        client.connect(data);
    pc.printf("Success\n\n\r");

}



int main() {

    pc.printf("Starting\n\r");
    
    connect_wifi();

    connect_netpie();

    char buf[100];
    sprintf(buf, "Hello NETPIE2020");
    message.qos = MQTT::QOS0;
    message.retained = false;
    message.dup = false;
    message.payload = (void*)buf;
    message.payloadlen = strlen(buf)+1;

    while(1) {
        client.publish(topic, message);
        pc.printf("Published topic: %s\tmsg:%s\n\r", topic, buf);
        led = !led;
        ThisThread::sleep_for(500);
    }

    return 0;
}

