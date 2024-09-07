#include <enet/enet.h>
#include <vjoy/public.h>
#include <vjoy/vjoyinterface.h>
#include <iostream>
#include <string>
#include <stdio.h>

uint32_t buttonFlags[7] = {            
   (1 << 0), (1 << 1), (1 << 2), (1 << 3), (1 << 4), (1 << 5), (1 << 6)
};

bool buttonState[7] = {
    false, false, false, false, false, false, false
};

float knobState[2] = {
    0.f
};

JOYSTICK_POSITION_V2 controller;
UINT iInterface = 1;

void updateVjoy(std::string packet) {
    switch (packet[0]) {
    case 'q':
        buttonState[0] = true; // 1 down
        puts("1 down");
        break;
    case 'a':
        buttonState[0] = false; // 1 up
        puts("1 up");
        break;
    case 'w':
        buttonState[1] = true; // 2 down
        puts("2 down");
        break;
    case 's':
        buttonState[1] = false; // 2 up
        puts("2 up");
        break;
    case 'e':
        buttonState[2] = true; // 3 down
        puts("3 down");
        break;
    case 'd':
        buttonState[2] = false; // 3 up
        puts("3 up");
        break;
    case 'r':
        buttonState[3] = true; // 4 down
        puts("4 down");
        break;
    case 'f':
        buttonState[3] = false; // 4 up
        puts("4 up");
        break;
    case 't':
        buttonState[4] = true; // 5 down
        puts("5 down");
        break;
    case 'g':
        buttonState[4] = false; // 5 up
        puts("5 up");
        break;
    case 'y':
        buttonState[5] = true; // 6 down
        puts("6 down");
        break;
    case 'h':
        buttonState[5] = false; // 6 up
        puts("6 up");
        break;
    case 'u':
        buttonState[6] = true; // 7 down
        puts("7 down");
        break;
    case 'j':
        buttonState[6] = false; // 7 up
        puts("7 up");
        break;
    default:
        float turntable = std::stof(packet);
        controller.wAxisX = int16_t(float(INT16_MAX) * turntable);
        break;
    }
    controller.lButtons = 0;
    // set every button individually like a caveman
    for (int i = 0; i < 7; i++) {
        if (buttonState[i]) {
            controller.lButtons |= 1 << i;
        }
    }
    if (!UpdateVJD(iInterface, &controller)) {
        printf("Failed updating vjoy device %d\n", iInterface);
    }
}

int main(int argc, char** argv)
{
    // Get the driver attributes (Vendor ID, Product ID, Version Number)
    if (!vJoyEnabled())
    {
        printf("Failed Getting vJoy attributes.\n");
        return -2;
    }
    else
    {
        printf("vjoy works\n");
    };

    // Get the state of the requested device
    VjdStat status = GetVJDStatus(iInterface);
    switch (status)
    {
    case VJD_STAT_OWN:
        printf("vJoy Device %d is already owned by this feeder\n", iInterface);
        break;
    case VJD_STAT_FREE:
        printf("vJoy Device %d is free\n", iInterface);
        break;
    case VJD_STAT_BUSY:
       printf("vJoy Device %d is already owned by another feeder\nCannot continue\n",
            iInterface);
    case VJD_STAT_MISS:
        printf("vJoy Device %d is not installed or disabled\nCannot continue\n",
            iInterface);
    default:
        printf("vJoy Device %d general error\nCannot continue\n", iInterface);
    };

    // Check which axes are supported
    BOOL AxisX = GetVJDAxisExist(iInterface, HID_USAGE_X);
    int nButtons = GetVJDButtonNumber(iInterface);
    printf("\nvJoy Device %d capabilities\n", iInterface);
    printf("Numner of buttons\t\t%d\n", nButtons);
    printf("Axis X\t\t%s\n", AxisX ? "Yes" : "No");

    // Acquire the target
    if ((status == VJD_STAT_OWN) || ((status == VJD_STAT_FREE) && (!
        AcquireVJD(iInterface))))
    {
        printf("Failed to acquire vJoy device number %d\n", iInterface);
        return -1;
    }
    else
    {
        printf("Acquired: vJoy device number %d.\n", iInterface);
    }

    memset(&controller, 0, sizeof(controller));

    controller.bDevice = iInterface;

    if (enet_initialize() != 0) {
        std::cout << "ENet initialization failed!" << std::endl;
        return EXIT_FAILURE;
    }
    ENetAddress address;
    ENetHost* server;


    // Set the port and ip
    address.host = ENET_HOST_ANY;
    address.port = 42069;


    ENetHost* client = enet_host_create(&address, 32, 2, 0, 0);
    if (client == NULL) {
        std::cerr << "Failed to create client!" << std::endl;
        return EXIT_FAILURE;
    }

    enet_host_compress_with_range_coder(client);


    ENetEvent event;
    int result;

    puts("listening for connections on all available IPs");

    while (true) {
        result = enet_host_service(client, &event, 1000);
        if (result < 0) {
            std::cerr << "Error during enet_host_service: " << result << std::endl;
            break;
        }
        else if (result > 0) {
            // Handle ENet events
            switch (event.type) {
            case ENET_EVENT_TYPE_CONNECT:
                std::cout << "Connection established." << std::endl;
                break;
            case ENET_EVENT_TYPE_RECEIVE:        
                // puts(std::string(event.packet->data, event.packet->data + event.packet->dataLength).c_str());
                updateVjoy(std::string(event.packet->data, event.packet->data + event.packet->dataLength));
                break;
            case ENET_EVENT_TYPE_DISCONNECT:
                std::cout << "Disconnected." << std::endl;
                break;
            }
            enet_packet_destroy(event.packet);
        }
    }

    enet_deinitialize();

}