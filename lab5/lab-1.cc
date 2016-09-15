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

#include <fstream>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("FifthScriptExample");

// ===========================================================================
//
//         node 0                 node 1
//   +----------------+    +----------------+
//   |    ns-3 TCP    |    |    ns-3 TCP    |
//   +----------------+    +----------------+
//   |    10.1.1.1    |    |    10.1.1.2    |
//   +----------------+    +----------------+
//   | point-to-point |    | point-to-point |
//   +----------------+    +----------------+
//           |                     |
//           +---------------------+
//                5 Mbps, 2 ms
//
//
// We want to look at changes in the ns-3 TCP congestion window.  We need
// to crank up a flow and hook the CongestionWindow attribute on the socket
// of the sender.  Normally one would use an on-off application to generate a
// flow, but this has a couple of problems.  First, the socket of the on-off 
// application is not created until Application Start time, so we wouldn't be 
// able to hook the socket (now) at configuration time.  Second, even if we 
// could arrange a call after start time, the socket is not public so we 
// couldn't get at it.
//
// So, we can cook up a simple version of the on-off application that does what
// we want.  On the plus side we don't need all of the complexity of the on-off
// application.  On the minus side, we don't have a helper, so we have to get
// a little more involved in the details, but this is trivial.
//
// So first, we create a socket and do the trace connect on it; then we pass 
// this socket into the constructor of our simple application which we then 
// install in the source node.
// ===========================================================================
//
class MyApp : public Application 
{
public:

  MyApp ();//constructor
  virtual ~MyApp();//distructor
    
    // add
    static TypeId GetTypeId (void);

  void Setup (Ptr<Socket> socket, Address address, uint32_t packetSize, uint32_t nPackets, DataRate dataRate);

private:
  virtual void StartApplication (void);//initial bind and connect, starat data flowing by call sende packect
  virtual void StopApplication (void);//cancel any pending send events and close the socket

  void ScheduleTx (void);
  void SendPacket (void);

  Ptr<Socket>     m_socket;
  Address         m_peer;
  uint32_t        m_packetSize;
  uint32_t        m_nPackets;
  DataRate        m_dataRate;
  EventId         m_sendEvent;
  bool            m_running;
  uint32_t        m_packetsSent;
};

MyApp::MyApp ()//constructor
  : m_socket (0), 
    m_peer (), 
    m_packetSize (0), 
    m_nPackets (0), 
    m_dataRate (0), 
    m_sendEvent (), 
    m_running (false), 
    m_packetsSent (0)
{
}

MyApp::~MyApp()//distructor
{
  m_socket = 0;
}

//add here
TypeId MyApp::GetTypeId (void)
{
    static TypeId tid = TypeId ("MyApp")
    .SetParent<Application> ()
    .SetGroupName ("Tutorial")
    .AddConstructor<MyApp> ()
    ;
    return tid;
}

void
MyApp::Setup (Ptr<Socket> socket, Address address, uint32_t packetSize, uint32_t nPackets, DataRate dataRate)
{
  m_socket = socket;
  m_peer = address;
  m_packetSize = packetSize;
  m_nPackets = nPackets;
  m_dataRate = dataRate;
}

void
MyApp::StartApplication (void)//start event
{
  m_running = true;
  m_packetsSent = 0;
  m_socket->Bind ();//bind local
  m_socket->Connect (m_peer);//connect romote and close sock
  SendPacket ();
}

void 
MyApp::StopApplication (void)
{
  m_running = false;

  if (m_sendEvent.IsRunning ())
    {
      Simulator::Cancel (m_sendEvent);//remove it from simulator queue
    }

  if (m_socket)
    {
      m_socket->Close ();
    }
}

void 
MyApp::SendPacket (void)
{
  Ptr<Packet> packet = Create<Packet> (m_packetSize);
  m_socket->Send (packet);

  if (++m_packetsSent < m_nPackets)//m_packetssent initialize 0
    {
      ScheduleTx ();
    }
}

void 
MyApp::ScheduleTx (void)//if the application is runningt it will schedule a new event,which calls send packet again.
{
  if (m_running)
    {
      Time tNext (Seconds (m_packetSize * 8 / static_cast<double> (m_dataRate.GetBitRate ())));
      //m_dataRate is he aoolication produce bits ,not channel
      m_sendEvent = Simulator::Schedule (tNext, &MyApp::SendPacket, this);
    }
}

static void
//add
CwndChange (Ptr<OutputStreamWrapper> stream, uint32_t oldCwnd, uint32_t newCwnd)
{
  NS_LOG_UNCOND (Simulator::Now ().GetSeconds () << "\t" << newCwnd);//
  //log the current simulation time and the new value of congestion window every time it is changed
    //add
    *stream->GetStream () << Simulator::Now ().GetSeconds () << "\t" << oldCwnd << "\t" << newCwnd << std::endl;
}

static void
RxDrop (Ptr<PcapFileWrapper> file, Ptr<const Packet> p)//show where packets are dropped
{
  NS_LOG_UNCOND ("RxDrop at " << Simulator::Now ().GetSeconds ());
    file->Write (Simulator::Now (), p);
}

int 
main (int argc, char *argv[])
{
    
    double BC_BER=0.00001;
    

    
  NodeContainer nodes;
  nodes.Create (4);

  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

  NetDeviceContainer devices1;
  devices1 = pointToPoint.Install (nodes.Get(0), nodes.Get(1));
   NetDeviceContainer devices2;
  devices2 = pointToPoint.Install (nodes.Get(1), nodes.Get(2));
   NetDeviceContainer devices3;
  devices3 = pointToPoint.Install (nodes.Get(2), nodes.Get(3));

///?
  Ptr<RateErrorModel> em = CreateObject<RateErrorModel> ();
  em->SetAttribute ("ErrorRate", DoubleValue (0.000001));
  devices1.Get (1)->SetAttribute ("ReceiveErrorModel", PointerValue (em));
  devices3.Get (1)->SetAttribute ("ReceiveErrorModel", PointerValue (em));
   Ptr<RateErrorModel> ef = CreateObject<RateErrorModel> ();
  ef->SetAttribute ("ErrorRate", DoubleValue (BC_BER)); //0.0025
  devices2.Get (1)->SetAttribute ("ReceiveErrorModel", PointerValue (ef));

  InternetStackHelper stack;
  stack.Install (nodes);

// assign IP
  NS_LOG_INFO ("Assign IP Addresses.");
  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.252");
  Ipv4InterfaceContainer interfaces1 = address.Assign (devices1);
   address.SetBase ("10.1.2.0", "255.255.255.252");
  Ipv4InterfaceContainer interfaces2 = address.Assign (devices2);
  address.SetBase ("10.1.3.0", "255.255.255.252");
  Ipv4InterfaceContainer interfaces3 = address.Assign (devices3);
// for more than 1 nodes,
  Ipv4GlobalRoutingHelper::PopulateRoutingTables();

//sink node
  uint16_t sinkPort = 8080;
  Address sinkAddress (InetSocketAddress (interfaces3.GetAddress (1), sinkPort));
  PacketSinkHelper packetSinkHelper ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), sinkPort));
  ApplicationContainer sinkApps = packetSinkHelper.Install (nodes.Get (3));
  sinkApps.Start (Seconds (0.));
  sinkApps.Stop (Seconds (20.));

//source node at node 0
  Ptr<Socket> ns3TcpSocket = Socket::CreateSocket (nodes.Get (0), TcpSocketFactory::GetTypeId ());
//  ns3TcpSocket->TraceConnectWithoutContext ("CongestionWindow", MakeCallback (&CwndChange));//CREATE SOCKET

  Ptr<MyApp> app = CreateObject<MyApp> ();
  app->Setup (ns3TcpSocket, sinkAddress, 1040, 1000, DataRate ("1Mbps"));
  nodes.Get (0)->AddApplication (app);
  app->SetStartTime (Seconds (1.));
  app->SetStopTime (Seconds (20.));
    
    AsciiTraceHelper asciiTraceHelper;
    Ptr<OutputStreamWrapper> stream = asciiTraceHelper.CreateFileStream ("lab5-2.cwnd");
    ns3TcpSocket->TraceConnectWithoutContext ("CongestionWindow", MakeBoundCallback (&CwndChange, stream));
    
    PcapHelper pcapHelper;
    Ptr<PcapFileWrapper> file = pcapHelper.CreateFile ("lab5.pcap", std::ios::out, PcapHelper::DLT_PPP);

    devices3.Get (1)->TraceConnectWithoutContext ("PhyRxDrop", MakeBoundCallback (&RxDrop, file));

  Simulator::Stop (Seconds (20));
  Simulator::Run ();
  Simulator::Destroy ();

  return 0;
}

