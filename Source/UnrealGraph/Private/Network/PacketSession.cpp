// Fill out your copyright notice in the Description page of Project Settings.


#include "Network/PacketSession.h"
// Network
#include "Network/WorkerThread.h"
#include "Network/NetworkManager.h"

// packet
#include "Common/Packet.h"

PacketSession::PacketSession(FSocket* sock, ANetworkManager* networkMgr) : Socket(sock), NetworkMgr(networkMgr)
{

}

PacketSession::~PacketSession()
{
	DisConnect();
}

void PacketSession::RunThread()
{
	RecvWorkerThread = MakeShared<RecvWorker>(Socket, AsShared());
	SendWorkerThread = MakeShared<SendWorker>(Socket, AsShared());
}

void PacketSession::DisConnect()
{
	if (RecvWorkerThread)
	{
		RecvWorkerThread->Destroy();
		RecvWorkerThread = nullptr;
	}

	if (SendWorkerThread)
	{
		SendWorkerThread->Destroy();
		SendWorkerThread = nullptr;
	}
}

void PacketSession::HandleRecvPackets()
{
	while (true)
	{
		TArray<uint8> Packet;
		if (RecvPacketQueue.Dequeue(OUT Packet) == false)
			break;

		if (NetworkMgr)
		{
			stHeader header;

			memcpy(&header, Packet.GetData(), sizeof(stHeader));

			NetworkMgr->Parse(Packet);
		}
	}
}



/*
추후에 추가될 기능을 위한 SendBuffer
*/

CSendBuffer::CSendBuffer(int32 bufferSize)
{
	_buffer.SetNum(bufferSize);
}

CSendBuffer::~CSendBuffer()
{
}

void CSendBuffer::CopyData(void* data, int32 len)
{
	::memcpy(_buffer.GetData(), data, len);
	_writeSize = len;
}

void CSendBuffer::Close(uint32 writeSize)
{
	_writeSize = writeSize;
}
