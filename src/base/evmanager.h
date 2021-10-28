#pragma once

#include <memory>
#include "event.h"

namespace Event
{
    using TaskEvPtr = std::shared_ptr<TaskEvent>;
    using TcpListenEvPtr = std::unique_ptr<TcpListenEvent>;
    class EventManager
    {
    public:
        static TaskEvPtr createTaskEvPtr()
        {
            return std::make_shared<TaskEvent>();
        }

        static TaskEvent&& createTaskEv()
        {
            return std::move(TaskEvent());
        }
    };
}

using EvManager = Event::EventManager;