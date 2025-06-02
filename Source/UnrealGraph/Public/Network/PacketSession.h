// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class FSocket;
class CSendBuffer;
class UNetworkManager;
class RecvWorker;
class SendWorker;

#define USING_SHARED_PTR(name)	using name##Ref = TSharedPtr<class name>;

// TShared 
USING_SHARED_PTR(Session);
USING_SHARED_PTR(PacketSession);
USING_SHARED_PTR(CSendBuffer);

class UNREALGRAPH_API PacketSession : public TSharedFromThis< PacketSession >	// TShared 
{
public:
	FSocket* Socket;
	
	TSharedPtr< RecvWorker > RecvWorkerThread;
	TSharedPtr< SendWorker > SendWorkerThread;

	TQueue<TArray<uint8>> RecvPacketQueue;
	TQueue<CSendBufferRef> SendPacketQueue;



public:
	PacketSession(FSocket* sock);
	~PacketSession();

	void RunThread();
	void DisConnect();


	UFUNCTION(BlueprintCallable)
	void HandleRecvPackets();

};

class CSendBuffer : public TSharedFromThis<CSendBuffer>
{
public:
	CSendBuffer(int32 bufferSize);
	~CSendBuffer();


	BYTE* Buffer() { return _buffer.GetData(); }
	int32 WriteSize() { return _writeSize; }
	int32 Capacity() { return static_cast<int32>(_buffer.Num()); }

	void CopyData(void* data, int32 len);
	void Close(uint32 writeSize);


private:
	TArray<BYTE>	_buffer;
	int32			_writeSize = 0;
};


/**
 * 
 */
