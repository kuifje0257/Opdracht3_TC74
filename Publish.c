#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "math.h"
#include "MQTTClient.h"

#define ADDRESS     "tcp://localhost:1883"
#define CLIENTID    "Branko"
#define TOPIC       "Opdracht3_TC74"

#define QOS         1
#define TIMEOUT     1000L

int sysCommand(){
   char command[50];
   char hex[100];

   strcpy(command, "sudo i2cget -y 1 0x48 0 b" );
   system(command);

    return (0);
} 


int main(int argc, char* argv[])
{

    MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    MQTTClient_deliveryToken token;
    int rc;
    
    MQTTClient_create(&client, ADDRESS, CLIENTID,
        MQTTCLIENT_PERSISTENCE_NONE, NULL);
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;

    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to connect, return code %d\n", rc);
        exit(-1);
    }
 
    int c = sysCommand();

    //nergens kan ik het resultaat van system(...) opvangen om om te zetten naar decimaal

    printf("...\n");
    printf("...\n");
    char result[50];
 
    sprintf(result,"%f", c);
    pubmsg.payload = result;
    printf("result = %s\n",result);
    
    
    //testbuf omdat mqtt werkt
    //sprintf(result,"hello");
    //pubmsg.payload = result;

    pubmsg.payloadlen = 50;
    pubmsg.qos = QOS;
    pubmsg.retained = 0;

    MQTTClient_publishMessage(client, TOPIC, &pubmsg, &token);
    printf("Waiting for up to %d seconds for publication of %s\n"
            "on topic %s for client with ClientID: %s\n",
            (int)(TIMEOUT/1000), result, TOPIC, CLIENTID);
    rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
    printf("Message with delivery token %d delivered\n", token);
    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);
    return rc;
    
}