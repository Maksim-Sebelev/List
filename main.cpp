#include "List.h"
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>


int main()
{
    List_t List = {};

    size_t Capacity = 16;
    LIST_ASSERT(ListCtor(&List, Capacity));


    ListElem_t InsertElem  = 10;
    ListElem_t EraseElem   = 0;
    size_t     InsertPlace = 0;
    size_t     RefElem     = 2;


    LIST_ASSERT(PushBack    (&List,          InsertElem, &InsertPlace));
    LIST_ASSERT(PushFront   (&List,          InsertElem, &InsertPlace));
    LIST_ASSERT(InsertAfter (&List, RefElem, InsertElem, &InsertPlace));
    LIST_ASSERT(InsertBefore(&List, RefElem, InsertElem, &InsertPlace));

    LIST_ASSERT(PopBack (&List,          &EraseElem));
    LIST_ASSERT(PopFront(&List,          &EraseElem));
    LIST_ASSERT(Erase   (&List, RefElem, &EraseElem));


    size_t Head = GetHead(&List);   // begin of list
    size_t Tail = GetTail(&List);   // end   of list

    LIST_ASSERT(InsertBefore(&List, Head, InsertElem, &InsertPlace));  // <=> LIST_ASSERT(PushFront(&List, InsertElem, &InsertPlace));
    LIST_ASSERT(InsertAfter (&List, Tail, InsertElem, &InsertPlace));  // <=> LIST_ASSERT(PushBack (&List, InsertElem, &InsertPlace));
    LIST_ASSERT(Erase       (&List, Head, &EraseElem));                // <=> LIST_ASSERT(PopFront (&List, InsertElem, &InsertPlace));
    LIST_ASSERT(Erase       (&List, Tail, &EraseElem));                // <=> LIST_ASSERT(PopBack  (&List, InsertElem, &InsertPlace));


    CONSOLE_DUMP(List);
    GRAPHIC_DUMP(List);
    PrintList(&List);
    

    LIST_ASSERT(ListDtor(&List));

    return 0;
}
