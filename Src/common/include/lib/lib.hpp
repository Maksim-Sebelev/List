#ifndef __LIB_HPP__
#define __LIB_HPP__

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define FLF __FILE__, __LINE__, __PRETTY_FUNCTION__

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifdef _DEBUG
    #define ON_DEBUG(...) __VA_ARGS__
    #define OFF_DEBUG(...)

    #define PRINT_PLACE() PrintPlace(FLF)
#else // _DEBUG
    #define ON_DEBUG(...)
    #define OFF_DEBUG(...) __VA_ARGS__

    #define PRINT_PLACE()
#endif // _DEBUG


//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifdef _DEBUG
#define _FUNC_CALLS_LOG
#endif

#ifdef _FUNC_CALLS_LOG
#define LOG_FUNC_ENTRY() LOG_PRINT(Green, "Entry: "); LOG_PLACE(White); LOG_NS()
#define LOG_FUNC_EXIT()  LOG_PRINT(Red  , "Exit: ") ; LOG_PLACE(White); LOG_NS()
#else
#define LOG_FUNC_ENTRY()
#define LOG_FUNC_EXIT()
#endif

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define RETURN_WHEN_FUNC_CALLS_LOG(return_elem) do \
{                                                   \
    LOG_FUNC_EXIT();                                 \
    return return_elem;                               \
} while (0)                                            \

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define WHITE   "\e[0;37m"
#define BLUE    "\e[0;34m"
#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define VIOLET  "\e[0;35m"
#define CYAN    "\e[0;36m"
#define YELLOW  "\e[0;33m"
#define RESET   "\x1b[0m"

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define COLOR_PRINT(COLOR, ...) do  \
{                                    \
    printf(COLOR);                    \
    printf(__VA_ARGS__);               \
    printf(RESET);                      \
} while (0)                              \

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

struct CodePlace
{
    const char*  file;
    unsigned int line;
    const char*  func;
};

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void      PrintPlace       (                        const char* const file, unsigned int line, const char* const func);
void      PrintStructPlace (const CodePlace* place                                                                   );
CodePlace CodePlaceCtor    (                        const char* const file, unsigned int line, const char* const func);

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define EXIT(exit_status, ...) do     \
{                                      \
    if (exit_status != EXIT_SUCCESS)    \
    {                                    \
        COLOR_PRINT(RED, __VA_ARGS__);    \
        printf("\n");                      \
        PRINT_PLACE();                      \
        printf("\n");                        \
    }                                         \
    exit(exit_status);                         \
} while(0)                                      \


//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define RETURN_IF_FALSE(flag, returnElem, ...) \
if (!(flag))                                    \
{                                                \
    __VA_ARGS__;                                  \
    return returnElem;                             \
}                                                   \

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define RETURN_IF_TRUE(flag, returnElem, ...) \
if ((flag))                                    \
{                                               \
    __VA_ARGS__;                                 \
    return returnElem;                            \
} 

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define FREE(ptr) do                                                \
{                                                                    \
    if (!(ptr))                                                       \
        EXIT(EXIT_FAILURE, "trying to free nullptr"                    \
                            ON_DEBUG("\nvariable name: '"#ptr"'")       \
            );                                                           \
                                                                          \
    free((char*)(ptr));                                                    \
    ptr = nullptr;                                                          \
} while (0)                                                                  \

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define __builtin_unreachable__(msg) \
assert(0 && msg);                     \
__builtin_unreachable()                \

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#endif // __LIB_HPP__
