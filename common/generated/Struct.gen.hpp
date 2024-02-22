#pragma once
#pragma warning(push)
#pragma warning(disable: 26495)
#pragma warning(disable: 4100)
#include "Packet.gen.hpp"

#ifdef __UNREAL__
#include "Network/Packet.h"
#elif __SERVER__
#include "Network/Packet.hpp"
#include "Util/Types.hpp"
#endif

#include <vector>

/* Additional headers. */
#include "Enum.gen.hpp"

namespace gen {
    class Room
            : public Packet {
    public:
        Room() : Packet(static_cast<unsigned short>(PacketId::NONE)) {
        }
        ~Room() {
    
        }
    protected:
        virtual void Read() override
        {
            Packet::Read();
            *this >> id >> name >> userCount;
        }
        virtual void Write() override
        {
            *this << id << name << userCount;
            Finish();
        }
    public:
        String id;
		String name;
		uint8 userCount;
	
    };
    
    inline Packet& operator>>(Packet& pk, Room& room) {
        pk >> room.id >> room.name >> room.userCount;
        return pk;
    }

    inline Packet& operator<<(Packet& pk, const Room& room) {
        pk << room.id << room.name << room.userCount;
        return pk;
    }

}
#pragma warning(pop)
