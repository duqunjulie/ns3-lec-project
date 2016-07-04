/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"

// Default Network Topology
//默认网络拓扑
// Number of wifi or csma nodes can be increased up to 250
//                          |
//                 Rank 0   |   Rank 1
// -------------------------|----------------------------
//   Wifi 10.1.3.0
//                          AP
// *   *   *   *   *    *   *
// |   |   |   |   |    |   |    10.1.1.0
//n8  n9  n10  n5  n6  n7  n0------------------ n1   n2   n3   n4   n5  n6   n7
//                        point-to-point         |    |    |    |    |   |    |
//                                               *    *    *    *    *   *    *
//                                                    Wifi 10.1.2.0
//                                              AP

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("ThirdScriptExample");		//定义记录组件

int 
main (int argc, char *argv[])
{
  bool verbose = true;
  uint32_t nWifi = 6;				//wifi节点数量
   bool tracing = false;


  CommandLine cmd;
  cmd.AddValue ("nWifi", "Number of wifi STA devices", nWifi);
  cmd.AddValue ("verbose", "Tell echo applications to log if true", verbose);
  cmd.AddValue ("tracing", "Enable pcap tracing", tracing);

  cmd.Parse (argc,argv);

  // Check for valid number of csma or wifi nodes
  // 250 should be enough, otherwise IP addresses 
  // soon become an issue		//判断是否超过了250个，超过报错 , 原因？
  if (nWifi > 250 )
    {
      std::cout << "Too many wifi or csma nodes, no more than 250 each." << std::endl;
      return 1;
    }

  if (verbose)
    {
      LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
      LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);	//启动记录组件
    }


  //创建2个节点，p2p链路两端
  NodeContainer p2pNodes;
  p2pNodes.Create (2);

  //创建信道，设置信道参数，在设备安装到节点上
  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("10Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

  NetDeviceContainer p2pDevices;
  p2pDevices = pointToPoint.Install (p2pNodes);

//创建wifista无线终端，AP接入点
  NodeContainer wifiStaNodes1;
  wifiStaNodes1.Create (nWifi);
  NodeContainer wifiApNode1 = p2pNodes.Get (0);

  NodeContainer wifiStaNodes2;
  wifiStaNodes2.Create (nWifi);
  NodeContainer wifiApNode2 = p2pNodes.Get (1);

  //创建无线设备于无线节点之间的互联通道，并将通道对象与物理层对象关联
  //确保所有物理层对象使用相同的底层信道，即无线信道
  YansWifiChannelHelper channel1 = YansWifiChannelHelper::Default ();
  channel1.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
  YansWifiPhyHelper phy1 = YansWifiPhyHelper::Default ();
  phy1.SetChannel (channel1.Create ());

  //配置速率控制算法，AARF算法
  WifiHelper wifi1 = WifiHelper::Default ();
  wifi1.SetRemoteStationManager ("ns3::AarfWifiManager");

  NqosWifiMacHelper mac1 = NqosWifiMacHelper::Default ();

  //配置mac类型为sta模式，不发送探测请求
  mac1.SetType ("ns3::AdhocWifiMac",
               "QosSupported", BooleanValue (false));

  //创建无线设备，将mac层和phy层安装到设备上
  NetDeviceContainer staDevices1;
  staDevices1 = wifi1.Install (phy1, mac1, wifiStaNodes1);

  //配置AP节点的mac层为AP模式，创建AP设备
  mac1.SetType ("ns3::AdhocWifiMac",
               "QosSupported", BooleanValue (false));

  NetDeviceContainer apDevices1;
  apDevices1 = wifi1.Install (phy1, mac1, wifiApNode1);

  //配置移动模型，起始位置
  MobilityHelper mobility1;

  mobility1.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (0.0),
                                 "MinY", DoubleValue (0.0),
                                 "DeltaX", DoubleValue (5.0),
                                 "DeltaY", DoubleValue (10.0),
                                 "GridWidth", UintegerValue (3),
                                 "LayoutType", StringValue ("RowFirst"));

  //配置STA移动方式，RandomWalk2dMobilityModel，随机游走模型
  mobility1.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                             "Bounds", RectangleValue (Rectangle (0, 100, -50, 50)));
  mobility1.Install (wifiStaNodes1);
   Ptr<RandomWalk2dMobilityModel>mob1=wifiStaNodes1.Get(0)->GetObject<RandomWalk2dMobilityModel>();
mob1->SetPosition(Vector(0,10,0));
   Ptr<RandomWalk2dMobilityModel>mob2=wifiStaNodes1.Get(1)->GetObject<RandomWalk2dMobilityModel>();
mob2->SetPosition(Vector(20,10,0));
   Ptr<RandomWalk2dMobilityModel>mob3=wifiStaNodes1.Get(2)->GetObject<RandomWalk2dMobilityModel>();
mob3->SetPosition(Vector(5,18,0));
   Ptr<RandomWalk2dMobilityModel>mob4=wifiStaNodes1.Get(3)->GetObject<RandomWalk2dMobilityModel>();
mob4->SetPosition(Vector(15,18,0));
   Ptr<RandomWalk2dMobilityModel>mob5=wifiStaNodes1.Get(4)->GetObject<RandomWalk2dMobilityModel>();
mob5->SetPosition(Vector(5,2,0));
   Ptr<RandomWalk2dMobilityModel>mob6=wifiStaNodes1.Get(5)->GetObject<RandomWalk2dMobilityModel>();
mob6->SetPosition(Vector(15,2,0));

//配置AP移动方式，ConstantPositionMobilityModel，固定位置模型
  mobility1.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility1.Install (wifiApNode1);
   Ptr<ConstantPositionMobilityModel>mob8=wifiApNode1.Get(0)->GetObject<ConstantPositionMobilityModel>();
mob8->SetPosition(Vector(10,10,0));
//创建无线设备于无线节点之间的互联通道，并将通道对象与物理层对象关联
  //确保所有物理层对象使用相同的底层信道，即无线信道
  YansWifiChannelHelper channel2 = YansWifiChannelHelper::Default ();
  channel2.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
  YansWifiPhyHelper phy2 = YansWifiPhyHelper::Default ();
  phy2.SetChannel (channel2.Create ());

  //配置速率控制算法，AARF算法
  WifiHelper wifi2 = WifiHelper::Default ();
  wifi2.SetRemoteStationManager ("ns3::AarfWifiManager");

  NqosWifiMacHelper mac2 = NqosWifiMacHelper::Default ();

  //配置mac类型为sta模式，不发送探测请求
  mac2.SetType ("ns3::AdhocWifiMac",
               "QosSupported", BooleanValue (false));

  //创建无线设备，将mac层和phy层安装到设备上
  NetDeviceContainer staDevices2;
  staDevices2 = wifi2.Install (phy2, mac2, wifiStaNodes2);

  //配置AP节点的mac层为AP模式，创建AP设备
  mac2.SetType ("ns3::AdhocWifiMac",
               "QosSupported", BooleanValue (false));

  NetDeviceContainer apDevices2;
  apDevices2 = wifi2.Install (phy2, mac2, wifiApNode2);

  //配置移动模型，起始位置
  MobilityHelper mobility2;

  mobility2.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (0.0),
                                 "MinY", DoubleValue (0.0),
                                 "DeltaX", DoubleValue (5.0),
                                 "DeltaY", DoubleValue (10.0),
                                 "GridWidth", UintegerValue (3),
                                 "LayoutType", StringValue ("RowFirst"));

  //配置STA移动方式，RandomWalk2dMobilityModel，随机游走模型
  mobility2.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                             "Bounds", RectangleValue (Rectangle (0, 100, -50, 50)));
  mobility2.Install (wifiStaNodes2);
  Ptr<RandomWalk2dMobilityModel>mob9=wifiStaNodes2.Get(0)->GetObject<RandomWalk2dMobilityModel>();
mob9->SetPosition(Vector(50,10,0));
   Ptr<RandomWalk2dMobilityModel>mob10=wifiStaNodes2.Get(1)->GetObject<RandomWalk2dMobilityModel>();
mob10->SetPosition(Vector(70,10,0));
   Ptr<RandomWalk2dMobilityModel>mob11=wifiStaNodes2.Get(2)->GetObject<RandomWalk2dMobilityModel>();
mob11->SetPosition(Vector(55,18,0));
   Ptr<RandomWalk2dMobilityModel>mob12=wifiStaNodes2.Get(3)->GetObject<RandomWalk2dMobilityModel>();
mob12->SetPosition(Vector(65,18,0));
   Ptr<RandomWalk2dMobilityModel>mob13=wifiStaNodes2.Get(4)->GetObject<RandomWalk2dMobilityModel>();
mob13->SetPosition(Vector(55,2,0));
   Ptr<RandomWalk2dMobilityModel>mob14=wifiStaNodes2.Get(5)->GetObject<RandomWalk2dMobilityModel>();
mob14->SetPosition(Vector(65,2,0));
//配置AP移动方式，ConstantPositionMobilityModel，固定位置模型
  mobility2.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility2.Install (wifiApNode2);
   Ptr<ConstantPositionMobilityModel>mob7=wifiApNode2.Get(0)->GetObject<ConstantPositionMobilityModel>();
mob7->SetPosition(Vector(60,10,0));
  //已经创建了节点，设备，信道和移动模型，接下来配置协议栈
  InternetStackHelper stack;
  stack.Install (wifiApNode1);
  stack.Install (wifiStaNodes1);
  stack.Install (wifiApNode2);
  stack.Install (wifiStaNodes2);

  //分配IP地址
  Ipv4AddressHelper address;
 //P2P信道
  address.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces;
  p2pInterfaces = address.Assign (p2pDevices);
 //wifi信道
  address.SetBase ("10.1.3.0", "255.255.255.0");
  address.Assign (staDevices1);
  address.Assign (apDevices1);
  address.SetBase ("10.1.2.0", "255.255.255.0");
  address.Assign (staDevices2);
  address.Assign (apDevices2);

  //放置echo服务端程序在最右边的csma节点,端口为9
  UdpEchoServerHelper echoServer (9);

  ApplicationContainer serverApps = echoServer.Install (p2pNodes.Get (0));
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (10.0));

  //回显客户端放在最后的STA节点，指向CSMA网络的服务器，上面的节点地址，端口为9
  UdpEchoClientHelper echoClient (p2pInterfaces.GetAddress (0), 9);
  echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (0.1)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (512));
  //安装其他节点应用程序
 ApplicationContainer clientApps = 
    echoClient.Install (wifiStaNodes2.Get (nWifi - 1));
  clientApps.Start (Seconds (2.0));
  clientApps.Stop (Seconds (10.0));

  //启动互联网络路由
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  Simulator::Stop (Seconds (10.0));


      pointToPoint.EnablePcapAll ("project4",false);
      phy1.EnablePcap ("project4", apDevices1.Get (0));
      phy2.EnablePcap ("project4", apDevices2.Get (0), true);

AsciiTraceHelper ascii1;
pointToPoint.EnableAsciiAll(ascii1.CreateFileStream("project4p2p.tr"));
AsciiTraceHelper ascii2;
phy1.EnableAsciiAll(ascii2.CreateFileStream("project4wifi.tr"));
  

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
