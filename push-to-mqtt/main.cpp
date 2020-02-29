#include <mbed.h>
#include <MQTTClientMbedOs.h>

DigitalOut led(LED1);
Serial pc(USBTX, USBRX);

InterruptIn button(USER_BUTTON);

WiFiInterface *wifi;

TCPSocket socket;

MQTTClient client(&socket);

Thread thread;
EventQueue queue(5 * EVENTS_EVENT_SIZE);

const char* hostname = "mqtt.netpie.io"; 
uint16_t  port = 1883;

const char* mqtt_client = "4665fab9-4827-40de-a1a6-36e538463bc4";
const char* mqtt_username = "CXhbMLgUwHFZWKdt77AHEVAgio42f3k7";
const char* mqtt_password = "zdsj67RATuG~~QftY+Y8XY_TS2XCtmEE";

const char* topic = "@msg/ict710";
MQTT::Message message;

void pressed_handler(){

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
    printf("MAC: %s\n\r", wifi->get_mac_address());
    printf("IP: %s\n\r", wifi->get_ip_address());
    printf("Netmask: %s\n\r", wifi->get_netmask());
    printf("Gateway: %s\n\r", wifi->get_gateway());
    printf("RSSI: %d\n\n\r", wifi->get_rssi());         

    pc.printf("\nConnecting to broker...\n\r");
    socket.open(wifi);
    socket.connect(hostname, port);
    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;       
    data.MQTTVersion = 3;
    data.clientID.cstring = (char*)mqtt_client;
    data.username.cstring = (char*)mqtt_username;
    client.connect(data);
    pc.printf("Success\n\n\r");
  
    char buf[128];
    sprintf(buf, "DAMN  NETPIE!");
    message.retained = false;
    message.dup = false;
    message.payload = (void*)buf;
    message.payloadlen = strlen(buf)+1;

    while(1){
        client.publish(topic, message);
        pc.printf("Published topic: %s\tmessage: %s\n\r", topic, buf);
        ThisThread::sleep_for(1000);
    }

}

int main() {
    thread.start(callback(&queue, &EventQueue::dispatch_forever));
    button.fall(queue.event(pressed_handler));
    pc.printf("Starting\n\r");
    while(1) {
        led = !led;
        ThisThread::sleep_for(500);
    }
}
