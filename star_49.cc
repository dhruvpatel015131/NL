/*-*-Mode:c++;c-file-style:"gnu";indent-tabs-mode:nil;-*-*/

//Add require Header Files
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h"
#include "ns3/csma-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/point-to-point-layout-module.h"

//Adding namespace declaration
using namespace ns3;

//Specify log component to store log messages
NS_LOG_COMPONENT_DEFINE("Star");

int main(int argc,char *argv[]){
	
	//setting the default values of onOff application when app is in ON state
	Config::SetDefault("ns3::OnOffApplication::PacketSize",UintegerValue(137));
	Config::SetDefault("ns3::OnOffApplication::DataRate",StringValue("14kb/s"));
	
	//specify no. of spoke nodes in topology
	uint32_t nSpokes=8;
	
	//Read and process command line argument
	CommandLine cmd(__FILE__);
	cmd.Parse(argc,argv);
	
	//Configure p2p netdevices and channels to be installed on spoke nodes
	PointToPointHelper pointToPoint;
	
	pointToPoint.SetDeviceAttribute("DataRate",StringValue("5Mbps"));
	pointToPoint.SetChannelAttribute("Delay",StringValue("2ms"));
	
	//installed configured net devices on spoke nodes and connected them to hub node using point to point channels
	PointToPointStarHelper star (nSpokes,pointToPoint);
	
	//display informational messages
	NS_LOG_INFO("Star Topology Created");
	
	//Install Protocol stack
	InternetStackHelper stack;
	star.InstallStack(stack);
	
	NS_LOG_INFO("Installed protocol stack on all nodes in topology");
	
	//Assign IP address to interfaces of spoke nodes and hub
	star.AssignIpv4Addresses(Ipv4AddressHelper("10.0.0.0","255.0.0.0"));
	
	NS_LOG_INFO("Assigned IP addresses to all spoke nodes and interfaces of hub");
	
	NS_LOG_INFO("IP address assign to hub interfaces are");
	
	for(uint32_t h_interfaces=0;h_interfaces<star.SpokeCount();++h_interfaces)
	{
		NS_LOG_INFO("IP Address of interface"<<h_interfaces);
		NS_LOG_INFO("IP Addess"<<star.GetHubIpv4Address(h_interfaces));
		
	}
	
	NS_LOG_INFO("IP Address of Spoke node are");
	
	for(uint32_t j=0;j<star.SpokeCount();++j)
	{
		NS_LOG_INFO("IP Address of spokenodes"<<j);
		NS_LOG_INFO("IP Addess"<<star.GetSpokeIpv4Address(j));
	}
	
	//Configuring packet sink Application on hub
	uint16_t port=50000; //specifying port no of hub
	
	//configuring socket address of hub
	Address hubLocalAddress(InetSocketAddress(Ipv4Address::GetAny(),port));
	
	PacketSinkHelper packetSinkHelper("ns3::TcpSocketFactory",hubLocalAddress);
	
	//Install packet sink appliacation on hub
	ApplicationContainer hubApp=packetSinkHelper.Install(star.GetHub());
	
	//Configuring start and stop time of packet sink application
	hubApp.Start(Seconds(1.0));
	hubApp.Stop(Seconds(10.0));
	
	//Configuring on Off application on spoke nodes
	OnOffHelper onOffHelper("ns3::TcpSocketFactory",Address());
	onOffHelper.SetAttribute("OnTime",StringValue("ns3::ConstantRandomVariable[Constant=1]"));
	onOffHelper.SetAttribute("OffTime",StringValue("ns3::ConstantRandomVariable[Constant=0]"));
	
	ApplicationContainer spokeApps;
	
	//Install On-Off application on all the spoke nodes
	for(uint32_t i=0;i<star.SpokeCount();++i){
		AddressValue remoteAddress(InetSocketAddress(star.GetHubIpv4Address(i),port));
		onOffHelper.SetAttribute("Remote",remoteAddress);
		spokeApps.Add(onOffHelper.Install(star.GetSpokeNode(i)));
	}
	
	spokeApps.Start(Seconds(1.0));
	spokeApps.Stop(Seconds(10.0));
	
	//Enable routing so that packet wll be routed across star topology
	Ipv4GlobalRoutingHelper::PopulateRoutingTables();
	
	//Enable packet capture on all nodes
	pointToPoint.EnablePcapAll("star");
	
	//Animate the star topology
	AnimationInterface anim("hus_star.xml");
	star.BoundingBox(1,1,100,100);
	
	Simulator::Run();
	Simulator::Destroy();
	
	NS_LOG_INFO("Done");
	
	return 0;
	// export NS_LOG=star=info
	//./waf          
	// ./waf --run scratch/star.cc
	
}
