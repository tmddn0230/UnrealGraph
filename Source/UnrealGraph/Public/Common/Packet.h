#pragma once
#pragma pack(push, 1)

#include "protocol.h"

#define ELECTRO 1234

struct stHeader
{
	// unsigned short  = uint16
	uint16  nProtocol;
	uint16  nSize;
	uint16  nType;
	uint16  nCheckSum;

	stHeader()
	{
		nProtocol = nSize = nType = nCheckSum = 0;
	};
	void SetHeader(int protocol, int len)
	{
		nProtocol = protocol;
		nSize = len;
		nType = ELECTRO;
		nCheckSum = nType + nSize;
	};

#if UE_BUILD_DEBUG + UE_BUILD_DEVELOPMENT + UE_BUILD_TEST + UE_BUILD_SHIPPING >= 1

	friend FArchive& operator<<(FArchive& Ar, stHeader& Header)
	{
		Ar << Header.nProtocol;
		Ar << Header.nSize;
		Ar << Header.nType;
		Ar << Header.nCheckSum;
		return Ar;
	}

#endif
};

struct stConnectAck : public stHeader
{
	// Connect
	uint16 Index;
	stConnectAck()
	{
		Index = 0;

		SetHeader(prConnectAck, sizeof(stConnectAck));
	};
};


#pragma pack(pop)  // ← 여기서 원래대로 정렬 복구

#define HEADSIZE sizeof( stHeader )
