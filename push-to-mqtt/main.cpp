#include <mbed.h>
#include "https_request.h"
#include <MQTTClientMbedOs.h>

DigitalOut led(LED1);
Serial pc(USBTX, USBRX);

InterruptIn button(USER_BUTTON);

WiFiInterface *wifi;
TCPSocket socket;
MQTTClient client(&socket);

Thread thread;
EventQueue queue(5 * EVENTS_EVENT_SIZE);

const char* hostname = "https://api.netpie.io/v2/device/message";
//const char* hostname = "mqtt.netpie.io"; 
uint16_t  port = 1883;

const char* mqtt_client = "4665fab9-4827-40de-a1a6-36e538463bc4";
const char* mqtt_username = "CXhbMLgUwHFZWKdt77AHEVAgio42f3k7";
const char* mqtt_password = "zdsj67RATuG~~QftY+Y8XY_TS2XCtmEE";

const char* topic = "@msg/ICT710";

MQTT::Message message;
const char SSL_CA_PEM[] ="-----BEGIN CERTIFICATE-----\n"
"MIIDdTCCAl2gAwIBAgILBAAAAAABFUtaw5QwDQYJKoZIhvcNAQEFBQAwVzELMAkG\n"
"A1UEBhMCQkUxGTAXBgNVBAoTEEdsb2JhbFNpZ24gbnYtc2ExEDAOBgNVBAsTB1Jv\n"
"b3QgQ0ExGzAZBgNVBAMTEkdsb2JhbFNpZ24gUm9vdCBDQTAeFw05ODA5MDExMjAw\n"
"MDBaFw0yODAxMjgxMjAwMDBaMFcxCzAJBgNVBAYTAkJFMRkwFwYDVQQKExBHbG9i\n"
"YWxTaWduIG52LXNhMRAwDgYDVQQLEwdSb290IENBMRswGQYDVQQDExJHbG9iYWxT\n"
"aWduIFJvb3QgQ0EwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDaDuaZ\n"
"jc6j40+Kfvvxi4Mla+pIH/EqsLmVEQS98GPR4mdmzxzdzxtIK+6NiY6arymAZavp\n"
"xy0Sy6scTHAHoT0KMM0VjU/43dSMUBUc71DuxC73/OlS8pF94G3VNTCOXkNz8kHp\n"
"1Wrjsok6Vjk4bwY8iGlbKk3Fp1S4bInMm/k8yuX9ifUSPJJ4ltbcdG6TRGHRjcdG\n"
"snUOhugZitVtbNV4FpWi6cgKOOvyJBNPc1STE4U6G7weNLWLBYy5d4ux2x8gkasJ\n"
"U26Qzns3dLlwR5EiUWMWea6xrkEmCMgZK9FGqkjWZCrXgzT/LCrBbBlDSgeF59N8\n"
"9iFo7+ryUp9/k5DPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNVHRMBAf8E\n"
"BTADAQH/MB0GA1UdDgQWBBRge2YaRQ2XyolQL30EzTSo//z9SzANBgkqhkiG9w0B\n"
"AQUFAAOCAQEA1nPnfE920I2/7LqivjTFKDK1fPxsnCwrvQmeU79rXqoRSLblCKOz\n"
"yj1hTdNGCbM+w6DjY1Ub8rrvrTnhQ7k4o+YviiY776BQVvnGCv04zcQLcFGUl5gE\n"
"38NflNUVyRRBnMRddWQVDf9VMOyGj/8N7yy5Y0b2qvzfvGn9LhJIZJrglfCm7ymP\n"
"AbEVtQwdpf5pLGkkeB6zpxxxYu7KyJesF12KwvhHhm4qxFYxldBniYUr+WymXUad\n"
"DKqC5JlR3XC321Y9YeRq4VzW9v493kHMB65jUr9TU/Qr6cf9tveCX4XSQRjbgbME\n"
"HMUfpIBvFSDJ3gyICh3WZlXi/EjJKSZp4A==\n"
"-----END CERTIFICATE-----\n";

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
    printf("MAC: %s\n\r", wifi->get_mac_address());
    printf("IP: %s\n\r", wifi->get_ip_address());
    printf("Netmask: %s\n\r", wifi->get_netmask());
    printf("Gateway: %s\n\r", wifi->get_gateway());
    printf("RSSI: %d\n\n\r", wifi->get_rssi());   
    //wifi->disconnect();
    //pc.printf("\n\rDone\n\r");    
}

void connect_mqtt() {
    
    char endpoint[128];
    char auth[128];

    sprintf(endpoint, "%s/%s", hostname, topic);
    sprintf(auth, "Device %s:%s", mqtt_client, mqtt_username);
    
    pc.printf("PUT to %s\n\r", endpoint);
    //pc.printf("Authorization: %s", auth);

    HttpsRequest* request = new HttpsRequest(wifi, SSL_CA_PEM, HTTP_PUT, endpoint);

    request->set_header("Authorization", auth);
    request->set_header("Content-type", "text/plain");

    const char* body = "Hello NETPIE2020";

    HttpResponse* response = request->send(body, strlen(body));

    printf("status is %d - %s\n\r", response->get_status_code(), response->get_status_message());
    printf("body is:\n%s\n\r", response->get_body());

    delete request;// also clears out the response

    //wifi->disconnect();
    //pc.printf("\nDone\n\r");

}


void pressed_handler(){
    // Connect to network interface
    connect_wifi();
    // Put a message to MQTT Broker
    while(true) {
        connect_mqtt();
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
