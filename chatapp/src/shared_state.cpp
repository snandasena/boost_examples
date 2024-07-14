//
// Created by sajith.nandasena on 10.07.2024.
//

#include <boost/shared_ptr.hpp>

#include "shared_state.h"
#include "websocket_session.h"

shared_state::shared_state(std::string doc_root): doc_root_(std::move(doc_root))
{
}


void shared_state::join(websocket_session *session)
{
    std::lock_guard<std::mutex> lock(mutex_);
    sessions_.insert(session);
}

void shared_state::leave(websocket_session *session)
{
    std::lock_guard<std::mutex> lock(mutex_);
    sessions_.erase(session);
}

void shared_state::send(std::string message)
{
    const auto ss = boost::make_shared<std::string const>(std::move(message));

    std::vector<boost::weak_ptr<websocket_session> > v; {
        std::lock_guard<std::mutex> lock(mutex_);
        v.reserve(sessions_.size());

        for (auto session: sessions_)
            v.emplace_back(session->weak_from_this());
    }
}
