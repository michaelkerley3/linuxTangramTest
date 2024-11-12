#include <unistd.h>
#include <stdio.h>
#include <vector>
#include <thread>
#include <chrono>
#if defined(WRITE_TO_FILE) || defined(READ_FROM_FILE)
#include <fstream>
#include <sys/stat.h>
#endif

// Message of interest
#include "hi/ethanToMichael.hpp"
#include "hi/michaelToEthan.hpp"
#include "hi/messageStruct.hpp"

// Generic transport
#include "TangramTransport.hpp"
static tangram::transport::TangramTransport *transport = nullptr;
// static tangram::transport::TangramTransport *alt_transport = nullptr;

#include "hi_DerivedEntityFactory.hpp"

#include "LMCPSerializer.hpp"
#define BUF_SIZE 65536


//******************************************************************************
/**
 * @brief This function initializes whatever the generic transport is and
 *        configures it statically (without the config file).  Note that a lot
 *        of what's in this function can go away if file-based configuration is
 *        used instead.
 *
 * @return int 0 on success or -1 on error.
 */
static int initTransport(uint64_t flags)
{
    // create the transport object
    transport = tangram::transport::TangramTransport::createTransport();
    if (nullptr == transport)
    {
        fprintf(stderr, "Could not create transport!\n");
        return -1;
    }
    // open it

    std::string hostname = "127.0.0.1";

    char *hn = std::getenv("TANGRAM_TRANSPORT_zeromq_transport_HOSTNAME");
    printf("Hostname: %s\n", hn);
    if (hn)
    {

        hostname.assign(hn);
        printf("hostname: %s\n", hostname.c_str());
    }
    transport->resetTransportOptions();
    transport->setOption("PublishIP", hostname);
    transport->setOption("PublishPort", "6668");
    transport->setOption("SubscribeIP", hostname);
    transport->setOption("SubscribePort", "6667");
    transport->setOption("PublishID", "0");
    if (transport->open(flags) == -1)
    {
        fprintf(stderr, "Could not open transport!\n");
        return -1;
    }

    transport->subscribe("e2m");
    //transport->subscribe("AirVehicleState");
    //transport->subscribe("EntityState");

    return 0;
}


//******************************************************************************
int main(int argc, char *argv[])
{

    uint8_t buffer[BUF_SIZE];
    // setup the transport
    if (initTransport(12) != 0)
    {
        fprintf(stderr, "Transport initialization failed!\n");
        exit(1);
    }


    // some transports require a brief delay between initialization and sending
    // of data
    sleep(1);

    // initialize the serializer
    tangram::serializers::Serializer *serializer;
    hi::DerivedEntityFactory derivedEntityFactory;

    tangram::serializers::LMCPSerializer lmcpSerializer(&derivedEntityFactory);
    serializer = &lmcpSerializer;

    // Mock Nav will be jammed every 20 seconds for 10 seconds
    int bytesRead = 0;





    hi::ethanToMichael e2m;

    std::cout << "started" << std::endl;
    
    while (true)
    {
        std::string topic;
        bytesRead = transport->recv(buffer, BUF_SIZE, topic);
        if(bytesRead == -1)
        {
            fprintf(stderr, "error");
        }

        auto msg = dynamic_cast<hi::ethanToMichael *>(serializer->deserialize(buffer, bytesRead, "e2m"));

        if(!msg)
        {
            fprintf(stderr, "asdfadsfasd");
            continue;
        }

        auto x = msg->getWaypoint()->getNum();
        if(x >= 1.0)
        {
            fprintf(stderr, "working");
            continue;
        }
        fprintf(stderr, "working");
        /*
        std::string topic;
        bytesRead = transport->recv(buffer, BUF_SIZE, topic);

        auto satNav_msg = dynamic_cast<hi::ethanToMichael *>(serializer->deserialize(buffer, bytesRead, "e2m"));


        if (bytesRead > 0)
        {
            if (topic == "e2m")
            {
                auto satNav_msg = dynamic_cast<hi::ethanToMichael *>(serializer->deserialize(buffer, bytesRead, "e2m"));
                if (!satNav_msg)
                {
                    fprintf(stderr, "Could not determine or deserialize the message\n");
                    continue;
                }
                fprintf(stderr, "SentMessage\n");
            }
            //fprintf(stderr, "SentMessage\n");
        }

        bytesRead = 0;
        */
    }
    sleep(2);

    printf("Closing transport.\n");
    transport->close();
    delete transport;

    printf("Done\n");
    return 0;
}
