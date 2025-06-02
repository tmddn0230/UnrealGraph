// Fill out your copyright notice in the Description page of Project Settings.


#include "Network/NetworkManager.h"
// Socket
#include "Sockets.h"
#include "Common/TcpSocketBuilder.h"
// UE
#include "Serialization/ArrayWriter.h"
// Network
#include "Network/PacketSession.h"
#include "Network/WorkerThread.h"
// packet
#include "Common/Packet.h"

DEFINE_LOG_CATEGORY(LogNetworkManager);



void UNetworkManager::Connect()
{
	Socket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(TEXT("Stream"), TEXT("Client Socket"));

	FIPv4Address Ip;
	FIPv4Address::Parse(IpAddress, Ip);

	TSharedRef<FInternetAddr> InternetAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	InternetAddr->SetIp(Ip.Value);
	InternetAddr->SetPort(Port);

	UE_LOG(LogNetworkManager, Log, TEXT("Connecting To Server"));

	bool Connected = Socket->Connect(*InternetAddr);

	if (Connected)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Conneted!"));
		UE_LOG(LogNetworkManager, Log, TEXT("Connection Success"));

		// Session
		GetWorld()->GetTimerManager().SetTimer(ReceiveTimerHandle, this, &UNetworkManager::HandleRecvPackets, 0.01f, true);
		GameServerPacketSession = MakeShared<PacketSession>(Socket);
		GameServerPacketSession->RunThread();
	}
	else
	{
		UE_LOG(LogNetworkManager, Log, TEXT("Connection Failed"));
	}
}

void UNetworkManager::Disconnect()
{
	GameServerPacketSession->DisConnect();
	
	Socket->Close();
}

void UNetworkManager::HandleRecvPackets()
{
	if (Socket == nullptr || GameServerPacketSession == nullptr)
		return;

	GameServerPacketSession->HandleRecvPackets();
}

void UNetworkManager::Parse(int32 protocol, TArray<uint8> packet)
{
	switch (protocol)
	{
	case prConnectAck:	RecvConnect(packet);	 break;
	case prData:		RecvDatas(packet);		 break;
	}
}

void UNetworkManager::SendPacket(stHeader* packetData)
{
	if (packetData)
	{
		int32 packetSize = packetData->nSize;

#if UE_BUILD_DEBUG + UE_BUILD_DEVELOPMENT + UE_BUILD_TEST + UE_BUILD_SHIPPING >= 1
		CSendBufferRef sendBuffer = MakeShared<CSendBuffer>(packetSize);
#else
		SendBuffer_TRef sendBuffer = nake_shared<SendBuffer_T>(packetSize);
#endif
		packetData->nSize = packetSize;
		sendBuffer->CopyData(packetData, packetSize);
		sendBuffer->Close(packetSize);

		if (GameServerPacketSession) {
			GameServerPacketSession->SendPacketQueue.Enqueue(sendBuffer);
		}
	}
}

void UNetworkManager::RecvConnect(TArray<uint8> packet)
{
	// user(signalMaker �� ������ �� ���� ȣ�� ��)
	// ȣ�� �� �� ���� Ư�� UI �� �߰��ϰų� �ҵ�
	stConnectAck ConnectAck;
	memcpy(&ConnectAck, packet.GetData(), sizeof(stConnectAck));

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Conneted! %d"), ConnectAck.Index));
}

void UNetworkManager::RecvDatas(TArray<uint8> packet)
{
	stData DataAck;
	memcpy(&DataAck, packet.GetData(), sizeof(stData));
	// json �о�� ��

    // json ������ ���� ��ġ ������
	uint8* jsonDataPtr = packet.GetData() + sizeof(stData);

	// FString ���� ��ȯ (UTF-8 ����)
	FString jsonStr = FString(UTF8_TO_TCHAR(reinterpret_cast<const char*>(jsonDataPtr)));

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, jsonStr);
}

void UNetworkManager::Initialize(FSubsystemCollectionBase& Collection)
{
	// SET DATAS
	IpAddress = "127.0.0.1";
	Port = 25001;
}

void UNetworkManager::Deinitialize()
{
	Disconnect();
}


