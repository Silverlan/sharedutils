// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include "sharedutils/utildefinitions.h"
#include <string>
#include <memory>
#include <vector>

export module pragma.util:net_packet;

export import :data_stream;

/*#ifdef _WIN32
#define NWM_MAX_PACKET_SIZE SO_MAX_MSG_SIZE
#else
// MTU = 1500 bytes; UDP Header = 28 bytes
//#define NWM_MAX_PACKET_SIZE 1'500 -28
#define NWM_MAX_PACKET_SIZE 0x2003
#endif*/

export {
	constexpr size_t NWM_PACKET_HEADER_SIZE = 4;
	constexpr size_t NWM_PACKET_HEADER_EXTENDED_SIZE = (NWM_PACKET_HEADER_SIZE + 2);
	constexpr size_t NWM_DEFAULT_PACKET_SIZE = 32;
	constexpr size_t NWM_MAX_PACKET_SIZE = (0x2003 - NWM_PACKET_HEADER_EXTENDED_SIZE);

	class DLLSHUTIL NWMPacket : public DataStreamBase {
	private:
		bool m_bClient = false;
		uint64_t m_tActivated = 0;

		NWMPacket(void *, uint32_t) {};
		friend TDataStream<NWMPacket>;
	public:
		NWMPacket();
		NWMPacket(uint32_t size);
		virtual void Resize(uint32_t sz, bool bForceResize = false) override;
		void SetClient(bool b); // Not used internally; Can be used to distinguish packets if necessary
		bool IsClient() const;

		void SetTimeActivated(uint64_t t);
		// The time at which the packet was eithe sent or received
		uint64_t GetTimeActivated() const;
	};

	class DLLSHUTIL NetPacket : public TDataStream<NWMPacket> {
	protected:
		uint32_t m_messageID;
	public:
		NetPacket(const NetPacket &o);
		NetPacket(std::nullptr_t t);
		NetPacket();
		NetPacket(uint32_t msgID);
		NetPacket(uint32_t msgID, uint32_t size);

		void SetMessageID(uint32_t id);
		uint32_t GetMessageID() const;
		NetPacket Copy();
		bool IsClient() const;

		void SetTimeActivated(uint64_t t);
		// The time at which the packet was eithe sent or received
		uint64_t GetTimeActivated() const;
	};
	DLLSHUTIL std::ostream &operator<<(std::ostream &out, const NetPacket &o);
}
