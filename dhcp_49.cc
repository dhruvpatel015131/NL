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
#include "ns3/internet-apps-module.h"

//adding namespace declareation namespace ns3
using namespace ns3;

//define log
NS_LOG_COMPONENT_DEFINE ("dhcpTopology");

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
LogComponentEnable("DhcpServer", LOG_LEVEL_ALL);
LogComponentEnable("DhcpClient", LOG_LEVEL_ALL);
LogComponentEnable("UdpEchoClientApplication",LOG_LEVEL_INFO);
LogComponentEnable("UdpEchoServerApplication",LOG_LEVEL_INFO);

//create dhcp client nodes and 2 router nodes
NodeContainer nodes; //dhcp client nodes
NodeContainer router; //router nodes


//create three nodes in dhcp nodes
nodes.Create(3);
//create 2 nodes in router
router.Create(2);

//combine nodes and router objects into a single object name
NodeContainer net(nodes,router);

//Configure csma on nodes and routers and connect them with channel
CsmaHelper csma;
csma.SetChannelAttribute("DataRate",StringValue ("5Mbps"));
csma.SetChannelAttribute("Delay", StringValue ("2ms"));
csma.SetDeviceAttribute("Mtu",UintegerValue (1500));

//install net on device on router and connect them with channel
NetDeviceContainer devices = csma.Install(net);

//Create pointToPoint Topology
//create p2p nodes
NodeContainer p2pNodes;
p2pNodes.Add(net.Get(4)); //adding r1 to p2p topology
p2pNodes.Create(1); //create new node

//Configure Point to point net devices
PointToPointHelper pointToPoint;

//set attributes of net device
pointToPoint.SetDeviceAttribute("DataRate",StringValue ("5Mbps"));

//set channel attribute
pointToPoint.SetChannelAttribute("Delay",StringValue ("2ms"));

//install p2p Nodes on router r1 and Node A
NetDeviceContainer p2pDevices = pointToPoint.Install(p2pNodes);

//Configure and install protocol stack on nodes
InternetStackHelper stack;
stack.Install(net); //install protocol stack on net
stack.Install(p2pNodes.Get(1));//install protocol stack on node A


//configure network IP address and surnet mask
Ipv4AddressHelper address;

//Configure Network IP Address for p2p Topology
address.SetBase("20.0.0.0","255.0.0.0");

Ipv4InterfaceContainer p2pInterface;
p2pInterface = address.Assign(p2pDevices);

//dhcpHelper class configure
DhcpHelper dhcpHelper;

//assign fix IP Address
Ipv4InterfaceContainer fixedInterface= dhcpHelper.InstallFixedAddress(devices.Get(4),Ipv4Address ("10.0.0.17"),Ipv4Mask("/8"));

fixedInterface.Get(0).first->SetAttribute ("IpForward",BooleanValue (true));

//Enable routing between 2 networks
Ipv4GlobalRoutingHelper::PopulateRoutingTables();


//install dhcpServer on router r0
ApplicationContainer dhcpServerApp = dhcpHelper.InstallDhcpServer(devices.Get(3),Ipv4Address ("10.0.0.12"),Ipv4Address("10.0.0.0"), Ipv4Mask ("/8"),Ipv4Address("10.0.0.10"), Ipv4Address ("10.0.0.15"), Ipv4Address ("10.0.0.17"));

//configure start time and stop time
dhcpServerApp.Start(Seconds(0.0));
dhcpServerApp.Stop(Seconds(20.0));

//dhcp client configure
NetDeviceContainer dhcpClientNetDevices;
dhcpClientNetDevices.Add (devices.Get(0));
dhcpClientNetDevices.Add (devices.Get(1));
dhcpClientNetDevices.Add (devices.Get(2));

//install dhcpClinet Application
ApplicationContainer dhcpClient = dhcpHelper.InstallDhcpClient(dhcpClientNetDevices);

//configure start and stop time of dhcp Client application
dhcpClient.Start(Seconds(1.0));
dhcpClient.Stop(Seconds(20.0));

//configure UDP echo server app
UdpEchoServerHelper echoServer(9);

ApplicationContainer serverApps = echoServer.Install(p2pNodes.Get(1));

serverApps.Start(Seconds(0.0));
serverApps.Stop(Seconds(20.0));

//configure UDP EchoClient App on Node 1
UdpEchoClientHelper echoClient (p2pInterface.GetAddress(1),9);

echoClient.SetAttribute("MaxPackets", UintegerValue(1));
echoClient.SetAttribute("Interval", TimeValue(Seconds(1.0)));
echoClient.SetAttribute("PacketSize", UintegerValue(1024));

ApplicationContainer clientApps = echoClient.Install(net.Get(1));

//configure start time and stop time
clientApps.Start(Seconds(10.0));
clientApps.Stop(Seconds(20.0));

//configure stop time of simulator
Simulator::Stop(Seconds(30.0));

//run the simulator
Simulator::Run();

//destory or release the resources
Simulator::Destroy();

return 0;

//./waf          
// ./waf --run scratch/dcp.cc
}
