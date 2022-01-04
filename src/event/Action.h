#ifndef ACTION_H
#define ACTION_H

#include "Event.h"
#include <memory>

namespace aerend {

template <typename E>
class Action {
public:
    virtual void execute(std::shared_ptr<E> event);
};

}

#include "Action.tpp"

#endif // ACTION_H

