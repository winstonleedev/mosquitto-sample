#include <signal.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <mosquitto.h>

//#define mqtt_host "192.168.3.99"//선철님 서버
#define mqtt_host "mqtt.testpa.thingbine.com"
#define mqtt_port 8889
#define MQTT_TOPIC "v/a/g/0060645d1506/s/0060645d1506-v-string-1"//1개 센서값전송

//#define mqtt_host "localhost"
//#define mqtt_port 8889
//#define MQTT_TOPIC "myTopic"

static int run = 1;
void handle_signal(int s)
{
    run = 0;
}
void connect_callback(struct mosquitto *mosq, void *obj, int result)
{
    printf("connect callback, rc = %d\n", result);
}
void message_callback(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message)
{
   bool match = 0;
   /*printf("got message '%.*s' for topic '%s'\n", message->payloadlen, (char*) message->payload, message->topic);*/
	printf("receive message(%s) : %s",message->topic, message->payload);
   //뭐하는 코드인지 모름 작동 안함
   //mosquitto_topic_matches_sub("/devices/wb-adc/controls/+", message->topic, &match);
   if (match) {
      printf("got message for ADC topic\n");
   }

}


int main(void)
{
    uint8_t reconnect = true;
    //char clientid[24]; //id사용하는 경우
    struct mosquitto *mosq;
    int rc = 0;

    signal(SIGINT, handle_signal);
    signal(SIGINT, handle_signal);

    mosquitto_lib_init();
    //메모리 초기화
    //memset(clientid, 0, 24);//맨 앞부터 0을 24개 삽입 (초기화)
    //snprintf(clientid, 23, "mysql_log_%d", getpid());//23길이의 clientid에 pid를 가진 문자열 삽입 
    // mosq = mosquitto_new(clientid, true, 0);//mosquitto 구조체 생성 <-
    
    //mosquitto 구조체 생성
    mosq = mosquitto_new(NULL, true, 0);
    if(mosq){
        mosquitto_connect_callback_set(mosq, connect_callback);
        mosquitto_message_callback_set(mosq, message_callback);

        //mosqutiio 서버와 연결
        rc = mosquitto_connect(mosq, mqtt_host, mqtt_port, 60);

        //subscribe
        mosquitto_subscribe(mosq, NULL, MQTT_TOPIC, 0);
        
        //Sublish(packetId, topic1 + qos1 )
        //packetId: 0보다 큰 Qos레벨만 관련~
        //topicName
        //qos = {0, 1, 2}

        while(run){
            rc = mosquitto_loop(mosq, -1, 1);
            if(run && rc){
                printf("connection error!\n");
                sleep(10);
                mosquitto_reconnect(mosq);
            }
        }
        mosquitto_destroy(mosq);
    }
    mosquitto_lib_cleanup();


    return rc;
}

