#pragma once
#ifndef TRACING_H
#define TRACING_H

#define TRACING_ENABLED

#include <string>

// Simple structure to handle function-call tracing.

// Define a preprocessor macro to help with the tracing
#ifdef TRACING_ENABLED
	#define TRACE() tracing::Tracer _tracer_object__ ## __COUNTER__ {__func__, __FILE__, __LINE__}
#else
	#define TRACE()
#endif

#ifdef TRACING_ENABLED
namespace aerend {

class Tracer {
public:
    Tracer() = delete;  // Disallow default construction
    Tracer(const Tracer&) = delete;  // Disallow copy construction
    Tracer(Tracer&&) = delete;  // Disallow move construction
    Tracer& operator=(const Tracer&) = delete;  // Disallow copy assignment
    Tracer& operator=(Tracer&&) = delete;  // Disallow move assignment
    Tracer(const std::string& func, const std::string& file, const int line);
    ~Tracer();
private:
    std::string func, file;
    int line;
};

}
#endif // TRACING_ENABLED

#endif // TRACING_H

