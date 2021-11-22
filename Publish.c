#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "unistd.h"
#include <math.h>
#include <assert.h>
#include "MQTTClient.h"

#define ADDRESS     "tcp://localhost:1883"
#define CLIENTID    "Branko"
#define TOPIC       "Opdracht3_TC74"

#define QOS         1
#define TIMEOUT     1000L

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
    char ph[50];
    
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

    //

    long long decimal, place;
    int i = 0, val, len;

    
    decimal = 0;
    place = 1;


    /* Find the length of total number of hex digit */
    len = strlen(buf);
    len--;

    /*
     * Iterate over each hex digit
     */
    for(i=0; buf[i]!='\0'; i++)
    {
 
        /* Find the decimal representation of hex[i] */
        if(buf[i]>='0' && buf[i]<='9')
        {
            val = buf[i] - 48;
        }
        else if(buf[i]>='a' && buf[i]<='f')
        {
            val = buf[i] - 97 + 10;
        }
        else if(buf[i]>='A' && buf[i]<='F')
        {
            val = buf[i] - 65 + 10;
        }

        decimal += val * pow(16, len);
        len--;
    }

    printf("Hexadecimal number = %s\n", buf);
    printf("Decimal number = %lld\n", decimal);
    //
    pubmsg.payload = buf;

    if(ph != buf){
        pubmsg.payloadlen = 50;
        pubmsg.qos = QOS;
        pubmsg.retained = 0;

        MQTTClient_publishMessage(client, TOPIC, &pubmsg, &token);
        printf("Waiting for up to %d seconds for publication of %s\n"
                "on topic %s for client with ClientID: %s\n",
                (int)(TIMEOUT/1000), buf, TOPIC, CLIENTID);
        rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
        printf("Message with delivery token %d delivered\n", token);
        
        return rc;
        sprintf(ph, buf);

    }
    else{
        printf("no changes");
    }
    pclose(p);
}