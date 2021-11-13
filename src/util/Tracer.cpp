#include "Tracer.h"
#include <iostream>

namespace tracing {
    Tracer::Tracer(const std::string& func, const std::string& file, const int line): func(func), file(file), line(line) {
        std::cerr << "TRACE: Entering " << func << " (" << file << ":" << line << ")" << std::endl;
    }

    Tracer::~Tracer() {
        std::cerr << "TRACE: Leaving " << func << std::endl;
    }
}
