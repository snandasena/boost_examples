//
// Created by sajith.nandasena on 10.07.2024.
//

#ifndef SHARED_STATE_H
#define SHARED_STATE_H


#include <string>
#include <unordered_set>
#include <mutex>

class websocket_session;

class shared_state
{
    std::string doc_root_;
    std::unordered_set<websocket_session *> sessions_;
    std::mutex mutex_;

public:
    explicit shared_state(std::string doc_root);

    std::string const &doc_root() const noexcept
    {
        return doc_root_;
    }

    void join(websocket_session *session);

    void leave(websocket_session *session);

    void send(std::string message);
};

#endif //SHARED_STATE_H
