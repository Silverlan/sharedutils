/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "sharedutils/netpacket.hpp"
#include <cstring>
#include <array>

#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("",off)
#endif
NetPacket::NetPacket(const NetPacket &o)
	: TDataStream<NWMPacket>(o),m_messageID(o.m_messageID)
{}
NetPacket::NetPacket()
	: TDataStream<NWMPacket>(),m_messageID(0)
{}
NetPacket::NetPacket(std::nullptr_t t)
	: TDataStream<NWMPacket>(t),m_messageID(0)
{}
NetPacket::NetPacket(uint32_t msgID)
	: TDataStream<NWMPacket>(),m_messageID(msgID)
{
	(*this)->Resize(NWM_DEFAULT_PACKET_SIZE);
}
NetPacket::NetPacket(uint32_t msgID,uint32_t size)
	: TDataStream<NWMPacket>(size),m_messageID(msgID)
{}
void NetPacket::SetTimeActivated(uint64_t t) {get()->SetTimeActivated(t);}
uint64_t NetPacket::GetTimeActivated() const {return get()->GetTimeActivated();}
void NetPacket::SetMessageID(uint32_t id) {m_messageID = id;}
uint32_t NetPacket::GetMessageID() const {return m_messageID;}
NetPacket NetPacket::Copy()
{
	auto size = (*this)->GetSize();
	NetPacket cpy(m_messageID,size);
	cpy->Write((*this)->GetData(),size);
	cpy->SetOffset(0);
	cpy.SetTimeActivated(GetTimeActivated());
	return cpy;
}
bool NetPacket::IsClient() const {return get()->IsClient();}

//////////////////////////////////

NWMPacket::NWMPacket()
	: DataStreamBase(),m_bClient(false)
{}

NWMPacket::NWMPacket(uint32_t size)
	: DataStreamBase(size)
{}

void NWMPacket::SetTimeActivated(uint64_t t) {m_tActivated = t;}
uint64_t NWMPacket::GetTimeActivated() const {return m_tActivated;}

void NWMPacket::Resize(uint32_t sz,bool bForceResize)
{
	sz = sz +GetHeaderSize();
	if(sz < NWM_DEFAULT_PACKET_SIZE)
		sz = NWM_DEFAULT_PACKET_SIZE;
	if(bForceResize == false && sz <= m_data->size())
		return;
	m_data->resize(m_data->size() +((uint32_t)((sz -m_data->size()) /NWM_DEFAULT_PACKET_SIZE) +1) *NWM_DEFAULT_PACKET_SIZE);
	m_rawData = m_data->data() +GetHeaderSize();
	if(m_offset +GetHeaderSize() > m_data->size())
		m_offset = m_data->size() -GetHeaderSize();
}

void NWMPacket::SetClient(bool b) {m_bClient = b;}
bool NWMPacket::IsClient() const {return m_bClient;}
#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("",on)
#endif
