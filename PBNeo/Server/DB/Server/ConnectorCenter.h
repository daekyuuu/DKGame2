#ifndef __CONNECTOR_CENTER_H__
#define __CONNECTOR_CENTER_H__

class CConnectorCenter : public S2NetConnector
{
	UINT32					m_ui32ServerIdx;

public:
	CConnectorCenter();
	~CConnectorCenter();

	BOOL					Create( UINT32 ui32IP, UINT16 ui16Port, UINT32 ui32ServerIdx );
	void					Destroy();

	BOOL					OnRunning();

	INT32					SendPacket( S2MOPacketBase* pPacket );
	INT32					PacketParsing( char* pPacket, INT32 i32Size );
};

#endif
