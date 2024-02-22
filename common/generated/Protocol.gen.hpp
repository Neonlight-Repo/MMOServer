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
#include "Struct.gen.hpp"

namespace gen {
    class LoginReq
            : public Packet {
    public:
        LoginReq() : Packet(static_cast<unsigned short>(PacketId::LOGIN_REQ)) {
        }
        ~LoginReq() {
    
        }
    protected:
        virtual void Read() override
        {
            Packet::Read();
            
        }
        virtual void Write() override
        {
            
            Finish();
        }
    public:
        
    };
    
    inline Packet& operator>>(Packet& pk, LoginReq& loginReq) {
        
        return pk;
    }

    inline Packet& operator<<(Packet& pk, const LoginReq& loginReq) {
        
        return pk;
    }

	class EnterGameReq
            : public Packet {
    public:
        EnterGameReq() : Packet(static_cast<unsigned short>(PacketId::ENTER_GAME_REQ)) {
        }
        ~EnterGameReq() {
    
        }
    protected:
        virtual void Read() override
        {
            Packet::Read();
            
        }
        virtual void Write() override
        {
            
            Finish();
        }
    public:
        
    };
    
    inline Packet& operator>>(Packet& pk, EnterGameReq& enterGameReq) {
        
        return pk;
    }

    inline Packet& operator<<(Packet& pk, const EnterGameReq& enterGameReq) {
        
        return pk;
    }

	class EnterRoomReq
            : public Packet {
    public:
        EnterRoomReq() : Packet(static_cast<unsigned short>(PacketId::ENTER_ROOM_REQ)) {
        }
        ~EnterRoomReq() {
    
        }
    protected:
        virtual void Read() override
        {
            Packet::Read();
            *this >> nickname >> roomId;
        }
        virtual void Write() override
        {
            *this << nickname << roomId;
            Finish();
        }
    public:
        String nickname;
		uint32 roomId;
	
    };
    
    inline Packet& operator>>(Packet& pk, EnterRoomReq& enterRoomReq) {
        pk >> enterRoomReq.nickname >> enterRoomReq.roomId;
        return pk;
    }

    inline Packet& operator<<(Packet& pk, const EnterRoomReq& enterRoomReq) {
        pk << enterRoomReq.nickname << enterRoomReq.roomId;
        return pk;
    }

	class LoginRes
            : public Packet {
    public:
        LoginRes() : Packet(static_cast<unsigned short>(PacketId::LOGIN_RES)) {
        }
        ~LoginRes() {
    
        }
    protected:
        virtual void Read() override
        {
            Packet::Read();
            *this >> success;
        }
        virtual void Write() override
        {
            *this << success;
            Finish();
        }
    public:
        bool success;
	
    };
    
    inline Packet& operator>>(Packet& pk, LoginRes& loginRes) {
        pk >> loginRes.success;
        return pk;
    }

    inline Packet& operator<<(Packet& pk, const LoginRes& loginRes) {
        pk << loginRes.success;
        return pk;
    }

	class EnterGameRes
            : public Packet {
    public:
        EnterGameRes() : Packet(static_cast<unsigned short>(PacketId::ENTER_GAME_RES)) {
        }
        ~EnterGameRes() {
    
        }
    protected:
        virtual void Read() override
        {
            Packet::Read();
            *this >> roomList;
        }
        virtual void Write() override
        {
            *this << roomList;
            Finish();
        }
    public:
        std::vector<Room> roomList;
	
    };
    
    inline Packet& operator>>(Packet& pk, EnterGameRes& enterGameRes) {
        pk >> enterGameRes.roomList;
        return pk;
    }

    inline Packet& operator<<(Packet& pk, const EnterGameRes& enterGameRes) {
        pk << enterGameRes.roomList;
        return pk;
    }

	class EnterRoomRes
            : public Packet {
    public:
        EnterRoomRes() : Packet(static_cast<unsigned short>(PacketId::ENTER_ROOM_RES)) {
        }
        ~EnterRoomRes() {
    
        }
    protected:
        virtual void Read() override
        {
            Packet::Read();
            *this >> success;
        }
        virtual void Write() override
        {
            *this << success;
            Finish();
        }
    public:
        bool success;
	
    };
    
    inline Packet& operator>>(Packet& pk, EnterRoomRes& enterRoomRes) {
        pk >> enterRoomRes.success;
        return pk;
    }

    inline Packet& operator<<(Packet& pk, const EnterRoomRes& enterRoomRes) {
        pk << enterRoomRes.success;
        return pk;
    }

}
#pragma warning(pop)
