#ifndef ACTION_TPP
#define ACTION_TPP

#include "Action.h"
#include <iostream>

namespace aerend {

template <typename E>
void Action<E>::execute(std::shared_ptr<E> event) {}

}

#endif // ACTION_TPP
