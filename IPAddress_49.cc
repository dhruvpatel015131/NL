/*-*-Mode:C++;c-file-style:"gnu";indent-tabs-mode:nil;-*-*/

//Add required header files
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h"

//adding namespace declareation namespace ns3
using namespace ns3;

//define log
NS_LOG_COMPONENT_DEFINE ("FIRSTScriptExample");

//Main Function
int main (int argc, char *argv[])
{
//read command line arguments
CommandLine cmd (__FILE__);

//process the arguments
cmd.Parse(argc,argv);

//setting the time resolution to 1 Nanosecond
Time::SetResolution(Time::NS);

//enabbling logging for application in your program
LogComponentEnable("UdpEchoClientApplication",LOG_LEVEL_INFO);
LogComponentEnable("UdpEchoServerApplication",LOG_LEVEL_INFO);

//create node container object to store 2 nodes
NodeContainer nodes;

//create two nodes in conatiner
nodes.Create(2);

//create a object of pointToPointHelper class to configure net device and channel on nodes
PointToPointHelper pointToPoint;

//set attributes of net device
pointToPoint.SetDeviceAttribute("DataRate",StringValue ("5Mbps"));

//set channel attribute
pointToPoint.SetChannelAttribute("Delay",StringValue ("2ms"));

//isntall netDevices on nodes and connnect them with channel
NetDeviceContainer devices;
devices = pointToPoint.Install (nodes);

//Create and install protocol stack on nodes
InternetStackHelper stack;
stack.Install(nodes);

//configure network IP address and surnet mask
Ipv4AddressHelper address;
address.SetBase("20.10.30.32","255.255.255.224");

//Create Ipv4 interfaces and assign IP address to interfaces of net devices
Ipv4InterfaceContainer interfaces = address.Assign(devices);

//Configure sever application first
UdpEchoServerHelper echoServer(9); //set port number of server application

//Create object to store server application and Install the configured server application on node 1
ApplicationContainer serverApps = echoServer.Install(nodes.Get(0));

//configure start and stop time of server application
serverApps.Start(Seconds(1.0));
serverApps.Stop(Seconds(10.0));

//Configure Client Applicaion
UdpEchoClientHelper echoClient (interfaces.GetAddress (0) , 9);

//set Attributes of client application

echoClient.SetAttribute("MaxPackets", UintegerValue(1));
echoClient.SetAttribute("Interval", TimeValue(Seconds(1.0)));
echoClient.SetAttribute("PacketSize", UintegerValue(1024));

//install client application on node 1
ApplicationContainer clientApps = echoClient.Install(nodes.Get(1));

//configure start time and stop time for client application
clientApps.Start(Seconds(2.0)); //client should start after server
clientApps.Stop(Seconds(10.0));

//animations
AnimationInterface anim("p2pAnimation.xml");
anim.SetConstantPosition(nodes.Get(0),20.0,20.0);//set position of node 0
anim.SetConstantPosition(nodes.Get(1),40.0,20.0);//set postition of node 1
//run the simulator
Simulator::Run();

//destory or release the resources
Simulator::Destroy();

return 0;





}