#ifndef __S2MO_BASE_H__
#define __S2MO_BASE_H__

class S2MOBase
{
	INT16					m_i16PacketRandSeed;

public:
	S2MOBase();
	~S2MOBase();

	bool					Create();
	void					Destory();

	// Packet Rand Seed
	void					SetPacketRandSeed( INT16 i16PacketRandSeed )
	{
		m_i16PacketRandSeed = i16PacketRandSeed;
	}

	INT16					GetPacketRandSeed()
	{
		m_i16PacketRandSeed = (((m_i16PacketRandSeed * 214013L + 2531011L) >> 16) & 0x7fff);
		return m_i16PacketRandSeed;
	}
};
#endif