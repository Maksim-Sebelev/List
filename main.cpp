#include "List.h"
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>



int main()
{
    List_t List = {};
    size_t ImgQuant = 0;

    ListCtor(&List, 16);
    size_t     InsertPlace = 0;
    ListElem_t PopElem     = 0;

    for (ListElem_t i = 0; i < 10; i++)
    {
        LIST_ASSERT(PushBack(&List, i + 1, &InsertPlace));
    }

    // for (ListElem_t i = 0; i < 0; i++)
    // {
    //     LIST_ASSERT(PushFront(&List, i + 101, &InsertPlace));

    // }

    InsertAfter(&List, GetTail(&List), 66, &InsertPlace);

    InsertBefore(&List, GetHead(&List), 66, &InsertPlace);

    InsertBefore(&List, 5, 66, &InsertPlace);

    InsertAfter(&List, 5, 66, &InsertPlace);

    // DUMP(List);
    // PrintList(&List);

    for (ListElem_t i = 0; i < 1; i++)
    {
        PopBack(&List, &PopElem);
    }

    for (ListElem_t i = 0; i < 2; i++)
    {
        PopFront(&List, &PopElem);
        printf("PopElem = %d\n", PopElem);
    }

    InsertAfter(&List, GetTail(&List), 66, &InsertPlace);
    InsertBefore(&List, 5, 66, &InsertPlace);
    InsertBefore(&List, GetHead(&List), 66, &InsertPlace);


    for (ListElem_t i = 5; i < 10; i++)
    {
        PushBack(&List, i + 101, &InsertPlace);
    }

    // Erase(&List, GetHead(&List), &PopElem);

    // DUMP(List);


    // PopBack(&List, &PopElem);
    // PushBack(&List, 17);

    // PrintList(&List);
    // DUMP(List);


    // printf("\n");

    // for (size_t i = 0; i < 5; i++)
    // {
    //     PopBack(&List, &PopElem);
    //     // printf("Pop elem = %d\n", PopElem);
    // }

    // GraphicDump(&List);

    DUMP(List);
    MakeDump(&List, &ImgQuant);

    PrintList(&List);

    ListDtor(&List);
    return 0;
}



