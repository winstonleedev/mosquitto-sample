#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <mosquitto.h>
#include <stdio.h>
#include <unistd.h>

#define MQTT_HOSTNAME "mqtt.sandbox.thingplus.net"
#define MQTT_PORT 8883
#define MQTT_CLIENTID "cc7a10000002"
#define MQTT_USERNAME "cc7a10000002"
#define MQTT_PASSWORD "xuS3BlVVE8Vn2C410y1fs_eOlDs="
#define MQTT_TOPIC "v/a/g/cc7a10000002/s/cc7a10000002-number-1"

char text[100] = "[1543388495584,22]";
static bool connected = true;
static bool disconnect_sent = false;
static int mid_sent = 0;
static int qos = 0;
static int retain = 0;

void my_connect_callback(struct mosquitto *mosq, void *obj, int result)
{
	int rc = MOSQ_ERR_SUCCESS;

	if(!result){
        printf("Sending message...\n");
		rc = mosquitto_publish(mosq, &mid_sent, MQTT_TOPIC, strlen(text), text, qos, retain);
		if(rc){
            switch(rc){
                case MOSQ_ERR_INVAL:
                    fprintf(stderr, "Error: Invalid input. Does your topic contain '+' or '#'?\n");
                    break;
                case MOSQ_ERR_NOMEM:
                    fprintf(stderr, "Error: Out of memory when trying to publish message.\n");
                    break;
                case MOSQ_ERR_NO_CONN:
                    fprintf(stderr, "Error: Client not connected when trying to publish.\n");
                    break;
                case MOSQ_ERR_PROTOCOL:
                    fprintf(stderr, "Error: Protocol error when communicating with broker.\n");
                    break;
                case MOSQ_ERR_PAYLOAD_SIZE:
                    fprintf(stderr, "Error: Message payload is too large.\n");
                    break;
            }
			mosquitto_disconnect(mosq);
		}
	} else {
		if(result){
			fprintf(stderr, "%s\n", mosquitto_connack_string(result));
		}
	}
}

void my_disconnect_callback(struct mosquitto *mosq, void *obj, int rc)
{
    printf("Disconnected!\n");
	connected = false;
}

void my_publish_callback(struct mosquitto *mosq, void *obj, int mid)
{
    printf("Published!\n");
	if(disconnect_sent == false){
		mosquitto_disconnect(mosq);
		disconnect_sent = true;
	}
}

int main(int argc, char *argv[])
{
    struct mosquitto *mosq = NULL;

    //초기화
    mosquitto_lib_init();
    //모스키토 런타임 객체와 클라이언트 랜덤 ID생성
    mosq = mosquitto_new(MQTT_CLIENTID, true, NULL);
    if(!mosq){
        printf("Cant initiallize mosquitto library\n");
        exit(-1);
    }

    mosquitto_connect_callback_set(mosq, my_connect_callback);
	mosquitto_disconnect_callback_set(mosq, my_disconnect_callback);
	mosquitto_publish_callback_set(mosq, my_publish_callback);

    mosquitto_username_pw_set(mosq, MQTT_USERNAME, MQTT_PASSWORD);

    mosquitto_tls_set(mosq, "ca-cert.pem", NULL, NULL, NULL, NULL);

    int ret = mosquitto_connect(mosq, MQTT_HOSTNAME, MQTT_PORT, 0);
    if(ret){
        printf("Cant connect to mosquitto server 1\n");
        exit(-1);
    }

    int rc;
    do {
        //network동작 끝나기 전에 모스키토 동작을 막기위해 잠깐 딜레이가 필요
        rc = mosquitto_loop(mosq, -1, 1);
    } while (rc == MOSQ_ERR_SUCCESS && connected);


    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();

    return 0;
}