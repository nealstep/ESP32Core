#pragma once

#include "e32c_messages.hpp"

#define UNIT_LIST(X)  \
    E32C_UNIT_LIST(X) \
    X(Last, "Last")
// Last entry for masking

#define ERROR_LIST(X)                                                  \
    X(NoError, "No Error")                                             \
    X(UnexpectedError, "Unexpected Error")                             \
    X(MallocFailed, "Malloc %S failed")                                \
    X(MallocFailedFreeing, "Malloc failed for buffer freeing entries") \
    E32C_ERROR_LIST(X)

#define NOTICE_LIST(X)          \
    X(NoNotice, "No Notice")    \
    X(Starting, "Starting up")  \
    X(Started, "Started up")    \
    X(Create, "Create")         \
    X(Destroy, "Destroy")       \
    X(SimpleValueStr, "%s: %s") \
    X(SimpleValueInt, "%s: %d") \
    E32C_NOTICE_LIST(X)

#define WORD_LIST(X)      \
    X(Unknown, "Unknown") \
    E32C_WORD_LIST(X)

#define SEVERITY_LIST(X) \
    X(Dbg, "Debug")      \
    X(Inf, "Info")       \
    X(Wrn, "Warning")    \
    X(Err, "Error")      \
    X(All, "All")
