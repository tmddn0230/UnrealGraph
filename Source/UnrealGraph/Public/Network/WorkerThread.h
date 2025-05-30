// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PacketSession.h"

// Log
UNREALGRAPH_API DECLARE_LOG_CATEGORY_EXTERN(LogNetworkThread, Log, All);

class FSocket;

///////////////////////////////////////////////////////////
/////////////         TCP Network            /////////////
//////////////////////////////////////////////////////////

class UNREALGRAPH_API RecvWorker : public FRunnable
{
protected:
	FRunnableThread* Thread = nullptr;
	bool bRun = true;
	FSocket* Socket;
	TWeakPtr< PacketSession > Session;

public:
	RecvWorker(FSocket* sock, TSharedPtr< PacketSession > session);
	~RecvWorker();

	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Exit() override;

	void Destroy();

private:
	bool ReceivePacket(TArray<uint8>& OutPacket);
	bool ReceiveDesiredBytes(uint8* Results, int32 Size);
};


class UNREALGRAPH_API SendWorker : public FRunnable
{
protected:
	FRunnableThread* Thread = nullptr;
	bool bRun = true;
	FSocket* Socket;
	TWeakPtr< PacketSession > Session;

public:
	SendWorker(FSocket* sock, TSharedPtr< PacketSession > session);
	~SendWorker();

	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Exit() override;

	bool SendPacket(CSendBufferRef SendBuffer);

	void Destroy();

private:
	bool SendDesiredBytes(const uint8* Buffer, int32 Size);


};