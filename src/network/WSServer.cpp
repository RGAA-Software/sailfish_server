//
// Created by RGAA on 2023/8/11.
//

#include "WSServer.h"

#include "rgaa_common/RLog.h"
#include "rgaa_common/RTime.h"
#include "rgaa_common/RData.h"
#include "rgaa_common/RThread.h"
#include "MessageProcessor.h"
#include "messages.pb.h"

namespace rgaa {

    WSServer::WSServer(const std::shared_ptr<Context>& ctx, const std::string& ip, int port) {
        this->context_ = ctx;
        this->ip_ = ip;
        this->port_ = port;
        msg_processor_ = std::make_shared<MessageProcessor>(ctx);
    }

    WSServer::~WSServer() {

    }

    void WSServer::Start() {
        ws_server_ = std::make_shared<server>();

        auto task = [this](){
            try {
                ws_server_->set_access_channels(websocketpp::log::alevel::none);
                //ws_server_->clear_access_channels(websocketpp::log::alevel::frame_payload);

                ws_server_->init_asio();
                ws_server_->set_message_handler([=, this](websocketpp::connection_hdl hdl, message_ptr msg) {
                    ProcessMessage(hdl, msg);
                });

                ws_server_->set_open_handler([=, this](websocketpp::connection_hdl hdl) {
                    AddSession(hdl);
                    LOGI("Open...");
                });

                ws_server_->set_close_handler([=, this](websocketpp::connection_hdl hdl) {
                    RemoveSession(hdl);
                    LOGI("Close...");
                });

                ws_server_->set_fail_handler([=, this](websocketpp::connection_hdl hdl) {
                    RemoveSession(hdl);
                    LOGI("Failed...");
                });

                ws_server_->listen(port_);
                ws_server_->start_accept();
                ws_server_->run();

            }
            catch (websocketpp::exception const& e) {
                LOGE("The port : {} may already used, error : {}", port_, e.what());
            }
            catch (...) {
                LOGE("The port : %d may already used, error : {}", port_);
            }
        };

        ws_thread_ = std::make_shared<Thread>(task, "", false);
    }

    void WSServer::Exit() {
        if (ws_thread_ && ws_thread_->IsJoinable()) {
            ws_thread_->Join();
        }
    }

    void WSServer::PostTextMessage(const std::string& msg) {
        if (!ws_server_) {
            return;
        }

        std::lock_guard<std::mutex> guard(session_mtx_);
        for (auto& [handle, session] : sessions_) {
            try {
                ws_server_->send(handle, msg, text);
            }
            catch (std::exception &e) {
                LOGE("Send text error : {}", e.what());
            }
        }
    }

    void WSServer::PostBinaryMessage(const std::shared_ptr<Data>& data) {
        if (!ws_server_) {
            return;
        }

        std::lock_guard<std::mutex> guard(session_mtx_);
        for (auto& [handle, session] : sessions_) {
            try {
                ws_server_->send(handle, data->CStr(), data->Size(), binary);
            }
            catch (std::exception &e) {
                LOGE("Send binary error : {}", e.what());
            }
        }
    }

    void WSServer::PostBinaryMessage(const std::string& data) {
        if (!ws_server_) {
            return;
        }

        std::lock_guard<std::mutex> guard(session_mtx_);
        if (sessions_.empty()) {
            return;
        }
        for (auto& [handle, session] : sessions_) {
            try {
                ws_server_->send(handle, data, binary);
            }
            catch (std::exception &e) {
                LOGE("Send binary error : {}", e.what());
            }
        }
    }

    void WSServer::ProcessMessage(websocketpp::connection_hdl hdl, message_ptr msg) {
        std::lock_guard<std::mutex> guard(session_mtx_);
        auto session = GetSession(hdl);
        if (!session) {
            LOGE("Not find session !");
            return;
        }

        if (msg->get_opcode() == text) {
            std::string value = msg->get_payload();
            LOGI("Text Msg: {}", value);
        }
        else if (msg->get_opcode() == binary) {

        }
        std::string value = msg->get_payload();
        msg_processor_->ProcessMessage(value);

//
//        this->msg_proc_thread->Post(SimpleThreadTask::Make([=]() {
//            if (message->has_mouse_info() || message->has_keyboard_info()) {
//                replayer->Replay(message);
//            }
//            else if (message->has_message_ack()) {
//                MessageACK ack = message->message_ack();
//                auto duration = GetCurrentTimestamp() - ack.send_time();
//                //std::cout << "ack , type : " << ack.type() << " frame index : " << ack.frame_index()
//                //	<< " duration : " << duration/2 << std::endl;
//            }
//        }));

    }

    void WSServer::AddSession(websocketpp::connection_hdl hdl) {
        std::lock_guard<std::mutex> guard(session_mtx_);
        auto session = WSSession::Make(hdl);
        sessions_.insert(std::pair<websocketpp::connection_hdl, WSSessionPtr>(hdl, session));

    }

    void WSServer::RemoveSession(websocketpp::connection_hdl hdl) {
        std::cout << "before move , size : " << sessions_.size() << std::endl;
        std::lock_guard<std::mutex> guard(session_mtx_);
        auto it = sessions_.find(hdl);
        if (it != sessions_.end()) {
            sessions_.erase(it);
        }
        std::cout << "after move , size : " << sessions_.size() << std::endl;
    }

    std::shared_ptr<WSSession> WSServer::GetSession(websocketpp::connection_hdl hdl) {
        if (sessions_.find(hdl) != sessions_.end()) {
            return sessions_[hdl];
        }
        return nullptr;
    }

    std::shared_ptr<WSSession> WSServer::GetSessionLocked(websocketpp::connection_hdl hdl) {
        std::lock_guard<std::mutex> guard(session_mtx_);
        if (sessions_.find(hdl) != sessions_.end()) {
            return sessions_[hdl];
        }
        return nullptr;
    }

}