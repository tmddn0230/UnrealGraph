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

// Sets default values
ANetworkManager::ANetworkManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void ANetworkManager::Connect()
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
		GetWorld()->GetTimerManager().SetTimer(ReceiveTimerHandle, this, &ANetworkManager::HandleRecvPackets, 0.01f, true);
		GameServerPacketSession = MakeShared<PacketSession>(Socket, this);
		GameServerPacketSession->RunThread();
	}
	else
	{
		UE_LOG(LogNetworkManager, Log, TEXT("Connection Failed"));
	}
}

void ANetworkManager::Disconnect()
{
	Socket->Close();
}

void ANetworkManager::HandleRecvPackets()
{
	if (Socket == nullptr || GameServerPacketSession == nullptr)
		return;

	GameServerPacketSession->HandleRecvPackets();
}

void ANetworkManager::Parse(int32 protocol, TArray<uint8> packet)
{
	switch (protocol)
	{
	case prConnectAck:	RecvConnect(packet);	 break;
	case prData:		RecvDatas(packet);		 break;
	}
}

void ANetworkManager::SendPacket(stHeader* packetData)
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

void ANetworkManager::RecvConnect(TArray<uint8> packet)
{
	// user(signalMaker 가 접속할 때 마다 호출 됨)
	// 호출 될 때 마다 특정 UI 를 추가하거나 할듯
	stConnectAck ConnectAck;
	memcpy(&ConnectAck, packet.GetData(), sizeof(stConnectAck));
}

void ANetworkManager::RecvDatas(TArray<uint8> packet)
{
	
}

// Called when the game starts or when spawned
void ANetworkManager::BeginPlay()
{
	Super::BeginPlay();
	
	// SET DATAS
	IpAddress = "127.0.0.1";
	Port = 25001;
}

// Called every frame
void ANetworkManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

