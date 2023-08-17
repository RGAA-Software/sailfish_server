//
// Created by RGAA on 2023/8/11.
//

#ifndef SAILFISH_SERVER_WSSERVER_H
#define SAILFISH_SERVER_WSSERVER_H
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <utility>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

typedef websocketpp::server<websocketpp::config::asio> server;

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;
using websocketpp::frame::opcode::value::text;
using websocketpp::frame::opcode::value::binary;

typedef server::message_ptr message_ptr;

#include <iostream>
#include <memory>
#include <string>
#include <map>

#include "Connection.h"

namespace rgaa {

    class Data;
    class Thread;
    class Context;
    class MessageProcessor;

    class WSSession {
    public:

        static std::shared_ptr<WSSession> Make(const websocketpp::connection_hdl& hdl) {
            return std::make_shared<WSSession>(hdl);
        }

        explicit WSSession(websocketpp::connection_hdl hdl) : handle_(std::move(hdl)) {}
        ~WSSession() = default;

        websocketpp::connection_hdl GetHandle() {
            return handle_;
        }

    private:
        websocketpp::connection_hdl handle_;

    };
    typedef std::shared_ptr<WSSession> WSSessionPtr;


    class WSServer : public Connection {
    public:

        WSServer(const std::shared_ptr<Context>& ctx, const std::shared_ptr<MessageProcessor>& processor, const std::string& ip, int port);
        ~WSServer() override;

        void Start() override;
        void Exit() override;

        void PostTextMessage(const std::string& msg) override;
        void PostBinaryMessage(const std::shared_ptr<Data>& data) override;
        void PostBinaryMessage(const std::string& data) override;

        virtual int GetConnectionPeerCount() override;

    private:

        void AddSession(websocketpp::connection_hdl hdl);
        void RemoveSession(websocketpp::connection_hdl hdl);
        std::shared_ptr<WSSession> GetSession(websocketpp::connection_hdl hdl);
        std::shared_ptr<WSSession> GetSessionLocked(websocketpp::connection_hdl hdl);
        void ProcessMessage(websocketpp::connection_hdl hdl, message_ptr msg);

    private:

        std::shared_ptr<server> ws_server_ = nullptr;
        std::mutex session_mtx_;
        std::map<websocketpp::connection_hdl,
                std::shared_ptr<WSSession>,
                std::owner_less<websocketpp::connection_hdl>> sessions_;

        std::shared_ptr<Thread> ws_thread_ = nullptr;
    };

}

#endif //SAILFISH_SERVER_WSSERVER_H
