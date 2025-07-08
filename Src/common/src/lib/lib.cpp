#include <stdio.h>
#include <assert.h>
#include "lib/lib.hpp"

#ifdef _DEBUG
#include "lib/logger/log.hpp"
#endif

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void PrintPlace(const char* file, unsigned int line, const char* func)
{
    assert(file);
    assert(func);

    COLOR_PRINT(WHITE, "%s:%u::(%s)", file, line, func);

    return;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void PrintStructPlace(const CodePlace* place)
{
    assert(place);
    assert(place->file);
    assert(place->func);

    COLOR_PRINT(WHITE, "%s:%u::(%s)", place->file, place->line, place->func);

    return;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

CodePlace CodePlaceCtor(const char* const file, unsigned int line, const char* const func)
{
    assert(file);
    assert(func);

    return (CodePlace)
    {
        .file = file,
        .line = line,
        .func = func,
    };
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
