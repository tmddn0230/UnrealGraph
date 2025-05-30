// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
// packet
#include "Common/Packet.h"

#include "NetworkManager.generated.h"
/*
	TCP 통신용 매니저 
*/

// Log
UNREALGRAPH_API DECLARE_LOG_CATEGORY_EXTERN(LogNetworkManager, Log, All);

class PacketSession;

UCLASS()
class UNREALGRAPH_API ANetworkManager : public AActor
{
	GENERATED_BODY()
	
public:
	class FSocket* Socket;

	// TCP
	FString IpAddress;
	int16	Port;
	TSharedPtr<PacketSession> GameServerPacketSession;
	FTimerHandle ReceiveTimerHandle;
public:	
	// Sets default values for this actor's properties
	ANetworkManager();

	// TCP
	UFUNCTION(BlueprintCallable)
	void Connect();
	void Disconnect();
	void HandleRecvPackets();
	void Parse(int32 protocol, TArray<uint8> packet);

	// Basic Send
	void SendPacket(stHeader* packetData);

	// Recv
	void RecvConnect(TArray<uint8> packet);
	void RecvDatas(TArray<uint8> packet);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
