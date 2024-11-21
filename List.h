#ifndef LIST_H
#define LIST_H

#include <stdio.h>
#include <stdlib.h>
#include "ColorPrint.h"

typedef int ListElem_t;


struct CodePlace
{
    const char* File;
    int         Line;
    const char* Func;
};


struct ListErrorType
{
    unsigned int IsFatalError           : 1;
    unsigned int ListOverFlow           : 1;
    unsigned int ListUnderFlow          : 1;
    unsigned int CtorCallocNull         : 1;

    CodePlace Place;

};

struct DataInfo
{
    ListElem_t Elem;
    size_t     Next;
    size_t     Prev;
};


struct List_t
{
    DataInfo*  Data;
    size_t     Capacity;
    size_t     Size;
    size_t     Free;
};


ListErrorType ListCtor         (List_t* List, size_t Capacity);
ListErrorType ListDtor         (List_t* List);
ListErrorType InsertAfter      (List_t* List, const size_t RefElem, const ListElem_t InsertElem, size_t* InsertPlace);
ListErrorType InsertBefore     (List_t* List, const size_t RefElem, const ListElem_t InsertElem, size_t* InsertPlace);
ListErrorType PushBack         (List_t* List, const ListElem_t PushElem,   size_t* PushPlace);
ListErrorType PushFront        (List_t* List, const ListElem_t PushElem, size_t* PushPlace);
ListErrorType Erase            (List_t* List, const size_t EraseElemPlace, ListElem_t* EraseELem);
ListErrorType PopBack          (List_t* List, ListElem_t* PopElem);
ListErrorType PopFront         (List_t* List, ListElem_t* PopElem);
void          PrintList        (const List_t* List);
size_t        GetHead          (const List_t* List);
size_t        GetTail          (const List_t* List);


ListErrorType Verif            (List_t* List, ListErrorType* Err, const char* File, const int Line, const char* Func);
void          ListAssertPrint  (ListErrorType* Err, const char* File, const int Line, const char* Func);

void          GraphicDump      (const List_t* List, const char* File, const int Line, const char* Func);
void          ConsoleDump      (const List_t* List, const char* File, int Line, const char* Func);


#define COMPILER_RETURN_IF_ERR(Err) do   \
{                                         \
    CompilerErrorType ErrCopy = Err;       \
    if (ErrCopy.IsFatalError == 1)          \
    {                                        \
        return ErrCopy;                       \
    }                                          \
} while (0)                                     \


#define LIST_ASSERT(Err) do                                      \
{                                                                 \
    ListErrorType ErrCopy = Err;                                   \
    if (ErrCopy.IsFatalError == 1)                                  \
    {                                                                \
        ListAssertPrint(&ErrCopy, __FILE__, __LINE__, __func__);      \
        COLOR_PRINT(CYAN, "abort() in 3, 2, 1...");                    \
        abort();                                                        \
    }                                                                    \
} while (0)                                                               \

    
#define LIST_VERIF(ListPtr, Err) Verif(ListPtr, &Err, __FILE__, __LINE__, __func__)

#define FREE(ptr) free(ptr); ptr = NULL;

#define CONSOLE_DUMP(List) ConsoleDump(&List, __FILE__, __LINE__, __func__)

#define GRAPHIC_DUMP(List) GraphicDump(&List, __FILE__, __LINE__, __func__)

#endif
