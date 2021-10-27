#pragma once

#include <memory>
#include "event.h"

namespace Event
{
    using taskEvPtr = std::shared_ptr<TaskEvent>;
    class EventManager
    {
    public:
        static taskEvPtr createTaskEv()
        {
            return std::make_shared<TaskEvent>();
        }
    };
}

using EventManager = Event::EventManager;