#include <stdio.h>
#include <stdlib.h>
#include "irslinger.h"

#define FOREACH_PROTOCOL(PROTOCOL) \
        PROTOCOL(NEC)   \
        PROTOCOL(SAMSUNG)  \
        PROTOCOL(RC5)   \
        PROTOCOL(RC6)  \

#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,

enum PROTOCOL {
    FOREACH_PROTOCOL(GENERATE_ENUM)
    PROTOCOL_NR
};

static const char *PROTOCOL_STRING[] = {
    FOREACH_PROTOCOL(GENERATE_STRING)
};

static const uint32_t LED_PIN = 4;

void hexToBinStr(const char*, char*);
int hexChar2int(const char);
void int2binStr(int, char *);
int getProtocol(const char**, const int, const char*);
int slingNEC(const char*);
int slingSAMSUNG(const char*);
int slingRC5(const char*);
int slingRC6(const char*);

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Not enough arguments supplied. Example arguments: NEC 0xFFFFFFFF.\n");
        exit(1);
    }

    int protocol = getProtocol(PROTOCOL_STRING, PROTOCOL_NR, argv[1]);

    if (protocol == -1)
    {
	printf("\"%s\" is not a valid protocol. Accepted values are:\n", argv[1]);
	int i;
	for (i = 0; i < PROTOCOL_NR; i++)
	{
	    printf("\t%s\n", PROTOCOL_STRING[i]);
	}

	exit(1);
    }

    if (strlen(argv[2]) != 10)
    {
    	printf("Data to sling is not the right length. Format is 0x<value> where value is 8 hex characters long.\n");
	exit(1);
    }

    char binaryString[33];
    binaryString[0] = '\0';
    hexToBinStr(argv[2], binaryString);

    switch (getProtocol(PROTOCOL_STRING, 4, argv[1]))
    {
        NEC:
            slingNEC(binaryString);
            break;
        SAMSUNG:
            slingSAMSUNG(binaryString);
            break;
        RC5:
            slingRC5(binaryString);
            break;
        RC6:
            slingRC6(binaryString);
            break;
    }
}

int getProtocol(const char** protocols, const int protocolsLength, const char* inputString)
{
    int i;

    for (i = 0; i < protocolsLength; i++)
    {
        if (strcmp(protocols[i], inputString) == 0)
        {
            return i;
        }
    }

    return -1;
}

int slingNEC(const char* binCode)
{
    int frequency = 38222;           // The frequency of the IR signal in Hz
    double dutyCycle = 0.5;          // The duty cycle of the IR signal. 0.5 means for every cycle,
                                     // the LED will turn on for half the cycle time, and off the other half
    int leadingPulseDuration = 9000; // The duration of the beginning pulse in microseconds
    int leadingGapDuration = 4500;   // The duration of the gap in microseconds after the leading pulse
    int onePulse = 562;              // The duration of a pulse in microseconds when sending a logical 1
    int zeroPulse = 562;             // The duration of a pulse in microseconds when sending a logical 0
    int oneGap = 1688;               // The duration of the gap in microseconds when sending a logical 1
    int zeroGap = 562;               // The duration of the gap in microseconds when sending a logical 0
    int sendTrailingPulse = 1;       // 1 = Send a trailing pulse with duration equal to "zeroPulse"
                                     // 0 = Don't send a trailing pulse

    return irSling
    (
        LED_PIN,
        frequency,
        dutyCycle,
        leadingPulseDuration,
        leadingGapDuration,
        onePulse,
        zeroPulse,
        oneGap,
        zeroGap,
        sendTrailingPulse,
        binCode
    );
}

int slingSAMSUNG(const char* binCode)
{
    int frequency = 37900;           // The frequency of the IR signal in Hz
    double dutyCycle = 0.5;          // The duty cycle of the IR signal. 0.5 means for every cycle,
                                     // the LED will turn on for half the cycle time, and off the other half
    int leadingPulseDuration = 4500; // The duration of the beginning pulse in microseconds
    int leadingGapDuration = 4500;   // The duration of the gap in microseconds after the leading pulse
    int onePulse = 590;              // The duration of a pulse in microseconds when sending a logical 1
    int zeroPulse = 590;             // The duration of a pulse in microseconds when sending a logical 0
    int oneGap = 1690;               // The duration of the gap in microseconds when sending a logical 1
    int zeroGap = 590;               // The duration of the gap in microseconds when sending a logical 0
    int sendTrailingPulse = 1;       // 1 = Send a trailing pulse with duration equal to "onePulse"
                                     // 0 = Don't send a trailing pulse

    return irSling
    (
        LED_PIN,
        frequency,
        dutyCycle,
        leadingPulseDuration,
        leadingGapDuration,
        onePulse,
        zeroPulse,
        oneGap,
        zeroGap,
        sendTrailingPulse,
        binCode
    );
}

int slingRC5(const char* binCode)
{
    printf("ERROR! Tried to sling with RC5 protocol but it has yet to be implemented!");
    return -1; // Not implemented
}

int slingRC6(const char* binCode)
{
    printf("ERROR! Tried to sling with RC6 protocol but it has yet to be implemented!");
    return -1; // Not implemented
}

void hexToBinStr(const char *hex, char *result)
{
    int i;

    for (i = 2; i < 10; i++)
    {
        char binaryString[5];
        char hexChar = hex[i];
        int value = hexChar2int(hexChar);
        int2binStr(value, binaryString);
        binaryString[4] = '\0';
        strcat(result, binaryString);
    }
}

int hexChar2int(const char hexChar)
{
    return (hexChar > '9') ? (hexChar &~ 0x20) - 'A' + 10 : (hexChar - '0');
}

void int2binStr(int value, char *result)
{
    int i;
    for (i = 3; i >= 0; --i, value >>= 1)
    {
        result[i] = (value & 1) ? '1' : '0';
    }
}
