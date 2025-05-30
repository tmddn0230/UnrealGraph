// Fill out your copyright notice in the Description page of Project Settings.


#include "Network/WorkerThread.h"
// Socket
#include "Sockets.h"
#include "Common/UdpSocketBuilder.h"
// packet
#include "Common/Packet.h"

DEFINE_LOG_CATEGORY(LogNetworkThread);

//--------------------------------------------------------------------------------------------------------------------------
// RecvWorker
//--------------------------------------------------------------------------------------------------------------------------

RecvWorker::RecvWorker(FSocket* sock, TSharedPtr<PacketSession> session)
{
	Thread = FRunnableThread::Create(this, TEXT("RecvWorkerThread"));
}

RecvWorker::~RecvWorker()
{
	Destroy();
}

bool RecvWorker::Init()
{
	if (Thread) {
		UE_LOG(LogNetworkThread, Log, TEXT("Recv Thread Init"));
		return true;
	}
	else
	{
		UE_LOG(LogNetworkThread, Log, TEXT("Failed Thread Init"));
		return false;
	}
}

uint32 RecvWorker::Run()
{
	while (bRun)
	{
		TArray<uint8> Packet;
		if (ReceivePacket(OUT Packet))
		{
			if (TSharedPtr<PacketSession> pSession = Session.Pin())
			{
				pSession->RecvPacketQueue.Enqueue(Packet);
			}
		}
	}
	return 0;
}

void RecvWorker::Exit()
{
	bRun = false;
}

void RecvWorker::Destroy()
{
	bRun = false;
}

bool RecvWorker::ReceivePacket(TArray<uint8>& OutPacket)
{
	const int32 HeaderSize = HEADSIZE;//sizeof( stHeader );
	TArray<uint8> HeaderBuffer;
	HeaderBuffer.AddZeroed(HeaderSize);

	if (ReceiveDesiredBytes(HeaderBuffer.GetData(), HeaderSize) == false)
		return false;

	stHeader Header;
	{
		FMemoryReader Reader(HeaderBuffer);
		Reader << Header;
		UE_LOG(LogTemp, Log, TEXT("PacketSize : %d"), Header.nSize);
	}
	OutPacket = HeaderBuffer;

	TArray<uint8> PayloadBuffer;
	const int32 PayloadSize = Header.nSize - HeaderSize;
	if (PayloadSize == 0)
		return true;

	OutPacket.AddZeroed(PayloadSize);

	if (ReceiveDesiredBytes(&OutPacket[HeaderSize], PayloadSize))
		return true;

	return false;
}

bool RecvWorker::ReceiveDesiredBytes(uint8* Results, int32 Size)
{
	if (!Socket) return false;

	uint32 PendingDataSize;
	if (Socket->HasPendingData(PendingDataSize) == false || PendingDataSize <= 0)
		return false;

	int32 Offset = 0;

	while (Size > 0)
	{
		int32 NumRead = 0;
		Socket->Recv(Results + Offset, Size, OUT NumRead);
		check(NumRead <= Size);

		if (NumRead <= 0)
			return false;

		Offset += NumRead;
		Size -= NumRead;
	}

	return true;
}

//--------------------------------------------------------------------------------------------------------------------------
// SendWorker
//--------------------------------------------------------------------------------------------------------------------------


SendWorker::SendWorker(FSocket* sock, TSharedPtr<PacketSession> session)
{
	Thread = FRunnableThread::Create(this, TEXT("SendWorkerThread"));
}

SendWorker::~SendWorker()
{
	Destroy();	
}

bool SendWorker::Init()
{
	if (Thread) {
		UE_LOG(LogNetworkThread, Log, TEXT("Send Thread Init"));
		return true;
	}
	else
	{
		UE_LOG(LogNetworkThread, Log, TEXT("Failed Thread Init"));
		return false;
	}
}

uint32 SendWorker::Run()
{
	while (bRun)
	{
		CSendBufferRef SendBuffer;

		if (PacketSessionRef pSession = Session.Pin())
		{
			if (pSession->SendPacketQueue.Dequeue(OUT SendBuffer))
			{
				SendPacket(SendBuffer);
			}
		}
	}

	return 0;
}

void SendWorker::Exit()
{
	bRun = false;
}

bool SendWorker::SendPacket(CSendBufferRef SendBuffer)
{
	if (SendDesiredBytes(SendBuffer->Buffer(), SendBuffer->WriteSize()) == false)
		return false;

	return true;
}

void SendWorker::Destroy()
{
	bRun = false;
}

bool SendWorker::SendDesiredBytes(const uint8* Buffer, int32 Size)
{
	while (Size > 0)
	{
		int32 BytesSent = 0;
		if (Socket->Send(Buffer, Size, BytesSent) == false)
			return false;

		Size -= BytesSent;
		Buffer += BytesSent;
	}

	return true;
}
