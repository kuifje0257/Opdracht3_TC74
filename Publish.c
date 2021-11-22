#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "unistd.h"
#include <assert.h>
#include "MQTTClient.h"

#define ADDRESS     "tcp://localhost:1883"
#define CLIENTID    "Branko"
#define TOPIC       "Opdracht3_TC74"

#define QOS         1
#define TIMEOUT     1000L

//weghalen van "0x" in "0x??"
size_t chopN(char *str, size_t n)
{
    assert(n != 0 && str != 0);
    size_t len = strlen(str);
    if (n > len)
        n = len;
    memmove(str, str+n, len - n + 1);
    return(len - n);
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
    sleep(2);
    
    FILE *p;
    char buf[50];

    p = popen("sudo i2cget -y 1 0x48 0 b","r");

    if( p == NULL)

    {
        puts("Unable to open process");
        return(1);
    }

    fgets(buf,50,p);  
    printf("temperature is %s",buf);

    chopN(buf,2);

    char leftHalf[100], rightHalf[100];
    int length, mid, i, k;
    char *ptr;
    int hex1, hex2, decimal;

    char result[5];

    /* Find length of string using strlen function */
    length = strlen(buf);
  
    mid = length/2;
    /* Copy left half of inputString to leftHalf */
    for(i = 0; i < mid; i++) {
        leftHalf[i]= buf[i];
    }
    leftHalf[i] = '\0';
  
    /* Copy right half of inputString to rightHalf  */
    for(i = mid, k = 0; i <= length; i++, k++) {
        rightHalf[k]= buf[i];
    }

    //decimaal maken
    hex1 = strtol(leftHalf, &ptr, 10)*16;
    hex2 = strtol(rightHalf, &ptr, 10);
    
    decimal = hex1+hex2;

    sprintf(result,"%d",decimal);
    printf("result : %s\n",result);


    pubmsg.payload = result;

    pubmsg.payloadlen = 50;
    pubmsg.qos = QOS;
    pubmsg.retained = 0;

    MQTTClient_publishMessage(client, TOPIC, &pubmsg, &token);
    printf("Waiting for up to %d seconds for publication of %s\n"
            "on topic %s for client with ClientID: %s\n",
            (int)(TIMEOUT/1000), result, TOPIC, CLIENTID);
    rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
    printf("Message with delivery token %d delivered\n", token);
      
    return rc;
    
    pclose(p);
}