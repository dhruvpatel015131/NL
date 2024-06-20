/*-*-Mode:C++;c-file-style:"gnu";indent-tabs-mode:nil;-*-*/

//Add required Header Files
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h"
#include "ns3/csma-module.h"
#include "ns3/ipv4-global-routing-helper.h"

//adding namespace declareation namespace ns3
using namespace ns3;

//define log
NS_LOG_COMPONENT_DEFINE ("BusTopology");

//Main Function
int main (int argc, char *argv[])
{
uint32_t extra_nodes =3; //declare no of nodes in bus topology

//read command line arguments
CommandLine cmd (__FILE__);

//process the arguments
cmd.Parse(argc,argv);

//setting the time resolution to 1 Nanosecond
Time::SetResolution(Time::NS);

//enabbling logging for application in your program
LogComponentEnable("UdpEchoClientApplication",LOG_LEVEL_INFO);
LogComponentEnable("UdpEchoServerApplication",LOG_LEVEL_INFO);

//creating p2p topology
//create node container object to store 2 nodes
NodeContainer p2pNodes;

//create two nodes in conatiner
p2pNodes.Create(2);

//create a object of pointToPointHelper class to configure net device and channel on nodes
PointToPointHelper pointToPoint;

//set attributes of net device
pointToPoint.SetDeviceAttribute("DataRate",StringValue ("5Mbps"));

//set channel attribute
pointToPoint.SetChannelAttribute("Delay",StringValue ("2ms"));

//isntall netDevices on nodes and connnect them with channel
NetDeviceContainer p2pDevices;
p2pDevices = pointToPoint.Install (p2pNodes);

//Create bus topology
//Create nodes using bus topology
NodeContainer bus_nodes;
bus_nodes.Add(p2pNodes.Get(1)); //Adding Node n1 to bus topology
bus_nodes.Create(extra_nodes); //Creating extra nodes in bus topology

//Configure csma net devices and channel
CsmaHelper csma;

csma.SetChannelAttribute("DataRate",StringValue ("100Mbps"));
csma.SetChannelAttribute("Delay",TimeValue(NanoSeconds(6560)));

//install configured csma net devices and channel on bus nodes
NetDeviceContainer bus_devices;
bus_devices=csma.Install(bus_nodes);

//Configure and install protocol stack on nodes
InternetStackHelper stack;

//install internet stack on node 0
stack.Install(p2pNodes.Get(0));

//install protocol stack on all bus nodes
stack.Install(bus_nodes);

//configure network IP address and surnet mask
Ipv4AddressHelper address;

//Configure Network IP Address for p2p Topology
address.SetBase("10.0.0.0","255.0.0.0");

//Create Ipv4 interfaces and assign IP address to interfaces of net devices
Ipv4InterfaceContainer p2pInterfaces = address.Assign(p2pDevices);

//Configure Network IP Address for bus topology
address.SetBase("20.0.0.0","255.0.0.0");

//assign IP Address to interface of net devices in bus topology
Ipv4InterfaceContainer bus_interfaces = address.Assign(bus_devices);

//Configure UdpEchoServerApplication
UdpEchoServerHelper echoServer(9); //set port number of server application

//Install server application on node n4
ApplicationContainer serverApps = echoServer.Install(bus_nodes.Get(3));

//configure start and stop time of server application
serverApps.Start(Seconds(1.0));
serverApps.Stop(Seconds(10.0));

////Configure Client Applicaion
UdpEchoClientHelper echoClient (bus_interfaces.GetAddress (3) , 9);

//set Attributes of client application

echoClient.SetAttribute("MaxPackets", UintegerValue(1));
echoClient.SetAttribute("Interval", TimeValue(Seconds(1.0)));
echoClient.SetAttribute("PacketSize", UintegerValue(1024));

//install client application on node 1
ApplicationContainer clientApps = echoClient.Install(p2pNodes.Get(0));

//configure start time and stop time for client application
clientApps.Start(Seconds(2.0)); //client should start after server
clientApps.Stop(Seconds(10.0));

//Enable routing between 2 networks
Ipv4GlobalRoutingHelper::PopulateRoutingTables();

//capture packets on node n1 in p2p topology
pointToPoint.EnablePcapAll("p2p_Packet");

//capture packets on node n2 in bus topology
csma.EnablePcap("bus_packet",bus_devices.Get(1),true);

AnimationInterface anim("BusToplogy.xml");
anim.SetConstantPosition(p2pNodes.Get(0),20.0,20.0);//set position of p2pNode
anim.SetConstantPosition(bus_nodes.Get(0),40.0,20.0);//set postition of bus_nodes 0
anim.SetConstantPosition(bus_nodes.Get(1),60.0,20.0);//set postition of bus_nodes 1
anim.SetConstantPosition(bus_nodes.Get(2),80.0,20.0);//set postition of bus_nodes 2
anim.SetConstantPosition(bus_nodes.Get(3),100.0,20.0);//set postition of bus_nodes 3

//run the simulator
Simulator::Run();

//destory or release the resources
Simulator::Destroy();

return 0;
//./waf          
// ./waf --run scratch/bus.cc
}
