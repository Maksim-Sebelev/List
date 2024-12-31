#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "List.h"

static size_t       GetFree          (const List_t* List);
static size_t       GetCapacity      (const List_t* List);
static size_t       GetDataSize      (const List_t* List);
static ListElem_t   GetDataElem      (const List_t* List, size_t Data_i);
static size_t       GetNextIndex     (const List_t* List, size_t NowIndex);
static size_t       GetPrevIndex     (const List_t* List, size_t NowIndex);
static size_t       GetNextInCtor      (const List_t* List, size_t Data_i);
static size_t       GetPrevInCtor      (const List_t* List, size_t Data_i);

static void ListElemCtor                  (DataInfo* Elem, const ListElem_t Value, const size_t Next, const size_t Prev);
static bool IsListFull                    (const List_t* List);
static bool IsListEmpty                   (const List_t* List);

static void ErrPlaceCtor    (ListErrorType* Err, const char* File, const int Line, const char* Func);
static void PrintPlace      (const char* File, int Line, const char* Function);

static void GraphicDumpHelper  (const List_t* List, const char* File, const int Line, const char* Func);
static void DotBegin           (FILE* dotFile);
static void DotEnd             (FILE* dotFile);
static void DotCreateDumpPlace (FILE* dotFile, const char* File, const int Line, const char* Func);
static void DotCreateNotFreeNode      (FILE* dotFile, const List_t* List, const size_t node_i);
static void DotCreateAllNodes  (FILE* dotFile, const List_t* List);
static void DotCreateNextEdges (FILE* dotFile, const List_t* List);
static void DotCreatePrevEdges (FILE* dotFile, const List_t* List);
static void DotCreateRestList  (FILE* dotFile, const List_t* list);
static void DotCreateEdges     (FILE* dotFile, const List_t* List);



const ListElem_t Poison = 666;

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ListErrorType ListCtor(List_t* List, size_t Capacity)
{
    assert(List);

    ListErrorType Err = {};
 
    List->Capacity    = Capacity + 1;
    List->Size        = 0;
    List->Data        = (DataInfo*) calloc(Capacity + 1, sizeof(DataInfo));

    if (List->Data == NULL)
    {
        Err.IsFatalError   = 1;
        Err.CtorCallocNull = 1;
        return LIST_VERIF(List, Err);
    }

    List->Data[GetTail(List)].Prev = List->Data[GetTail(List)].Prev;
    List->Data[0]            .Next = List->Data[GetTail(List)].Next;
    List->Data[GetTail(List)].Next = 0;
    List->Free                     = 1;

    ListElemCtor(&List->Data[0], Poison, 0, 0); 

    for (size_t Data_i = 1; Data_i < GetCapacity(List); Data_i++)
    {
        ListElemCtor(&List->Data[Data_i], Poison, GetNextInCtor(List, Data_i), GetPrevInCtor(List, Data_i));
    }

    return LIST_VERIF(List, Err);

}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ListErrorType ListDtor(List_t* List)
{
    assert(List);
    ListErrorType Err = {};
    List->Capacity = 0;

    FREE(List->Data);
    return LIST_VERIF(List, Err);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ListErrorType InsertAfter(List_t* List, const size_t RefElem, const ListElem_t InsertElem, size_t* InsertPlace)
{
    assert(List);

    ListErrorType Err = {};

    if (IsListFull(List))
    {
        Err.IsFatalError = 1;
        Err.ListOverFlow = 1;
        return LIST_VERIF(List, Err);
    }

    List->Size++;
 
    size_t Free       = GetFree(List);
    size_t PrevFree   = GetPrevIndex(List, Free);
    size_t NextFree   = GetNextIndex(List, Free);
    size_t NextRef    = GetNextIndex(List, RefElem);

    *InsertPlace              = Free;
    List->Data[Free]    .Elem = InsertElem;
    List->Data[RefElem] .Next = Free;
    List->Data[NextRef] .Prev = Free;
    List->Data[Free]    .Prev = RefElem;
    List->Data[Free]    .Next = NextRef;
    List->Free                = NextFree;
    List->Data[NextFree].Prev = PrevFree;
    List->Data[PrevFree].Next = NextFree;

    return LIST_VERIF(List, Err);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ListErrorType InsertBefore(List_t* List, const size_t RefElem, const ListElem_t InsertElem, size_t* InsertPlace)
{
    assert(List);

    ListErrorType Err = {};

    if (IsListFull(List))
    {
        Err.IsFatalError = 1;
        Err.ListOverFlow = 1;
        return LIST_VERIF(List, Err);
    }

    List->Size++;

    size_t Free       = GetFree(List);
    size_t PrevFree   = GetPrevIndex(List, Free);
    size_t NextFree   = GetNextIndex(List, Free);
    size_t PrevRef    = GetPrevIndex(List, RefElem);

    *InsertPlace              = Free;
    List->Data[Free]    .Elem = InsertElem;
    List->Data[Free]    .Next = RefElem;
    List->Data[Free]    .Prev = PrevRef;
    List->Data[RefElem] .Prev = Free;
    List->Data[PrevRef] .Next = Free;
    List->Free                = NextFree;
    List->Data[NextFree].Prev = PrevFree;
    List->Data[PrevFree].Next = NextFree;

    return LIST_VERIF(List, Err);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ListErrorType Erase(List_t* List, const size_t ErasePlace, ListElem_t* EraseElem)
{
    assert(List);

    ListErrorType Err = {};

    if (IsListEmpty(List))
    {
        Err.IsFatalError   = 1;
        Err.ListUnderFlow = 1;
        return LIST_VERIF(List, Err);
    }

    List->Size--;

    size_t Free      = GetFree(List);
    size_t PrevFree  = GetPrevIndex(List, Free);
    size_t NextFree  = GetNextIndex(List, Free);
    size_t PrevErase = GetPrevIndex(List, ErasePlace);
    size_t NextErase = GetNextIndex(List, ErasePlace);

    *EraseElem                  = GetDataElem(List, ErasePlace);
    List->Data[ErasePlace].Elem = Poison;

    List->Data[ErasePlace].Next = Free;
    List->Data[ErasePlace].Prev = PrevFree;
    List->Data[PrevErase] .Next = NextErase;
    List->Data[NextErase] .Prev = PrevErase;
    List->Data[Free]      .Prev = Free;
    List->Free                  = ErasePlace;
    List->Data[PrevFree]  .Next = ErasePlace;
    List->Data[NextFree]  .Prev = ErasePlace;
    
    return LIST_VERIF(List, Err);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ListErrorType PushBack(List_t* List, const ListElem_t PushElem, size_t* PushPlace)
{
    assert(List);

    ListErrorType Err = {};

    if (IsListFull(List))
    {
        Err.IsFatalError = 1;
        Err.ListOverFlow = 1;
        return LIST_VERIF(List, Err);
    }


    InsertAfter(List, GetTail(List), PushElem, PushPlace);

    return LIST_VERIF(List, Err);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ListErrorType PushFront(List_t* List, const ListElem_t PushElem, size_t* PushPlace)
{
    assert(List);

    ListErrorType Err = {};

    if (IsListFull(List))
    {
        Err.IsFatalError = 1;
        Err.ListOverFlow = 1;
        return LIST_VERIF(List, Err);
    }

    InsertBefore(List, GetHead(List), PushElem, PushPlace);

    return LIST_VERIF(List, Err);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ListErrorType PopBack(List_t* List, ListElem_t* PopElem)
{
    assert(List);

    ListErrorType Err = {};

    if (IsListEmpty(List))
    {
        Err.IsFatalError = 1;
        Err.ListUnderFlow = 1;
        return LIST_VERIF(List, Err);
    }

    Erase(List, GetTail(List), PopElem);

    return LIST_VERIF(List, Err);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ListErrorType PopFront(List_t* List, ListElem_t* PopElem)
{
    assert(List);

    ListErrorType Err = {};

    if (IsListEmpty(List))
    {
        Err.IsFatalError = 1;
        Err.ListUnderFlow = 1;
        return LIST_VERIF(List, Err);
    }

    Erase(List, GetHead(List), PopElem);

    return LIST_VERIF(List, Err);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void ListElemCtor(DataInfo* Elem, const ListElem_t Value, const size_t Next, const size_t Prev)
{
    assert(Elem);
    Elem->Elem = Value;
    Elem->Next = Next;
    Elem->Prev = Prev;
    return;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static ListElem_t GetDataElem(const List_t* List, size_t Data_i)
{
    assert(List);
    return List->Data[Data_i].Elem;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

size_t GetTail(const List_t* List)
{
    assert(List);
    return List->Data[0].Prev;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static size_t GetNextIndex(const List_t* List, size_t NowIndex)
{
    assert(List);   
    return List->Data[NowIndex].Next;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static size_t GetPrevIndex(const List_t* List, size_t NowIndex)
{
    assert(List);
    return List->Data[NowIndex].Prev;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

size_t GetHead(const List_t* List)
{
    assert(List);
    return List->Data[0].Next;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static size_t GetFree(const List_t* List)
{
    assert(List);
    return List->Free;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static size_t GetCapacity(const List_t* List)
{
    assert(List);
    return List->Capacity;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static size_t GetDataSize(const List_t* List)
{
    assert(List);
    return List->Size;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool IsListFull(const List_t* List)
{
    assert(List);
    return GetCapacity(List) == GetDataSize(List) + 1;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool IsListEmpty(const List_t* List)
{
    assert(List);
    return GetDataSize(List) == 0;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static size_t GetNextInCtor(const List_t* List, size_t Data_i)
{
    assert(List);

    size_t temp_next = (Data_i + 1) % (GetCapacity(List));
    return temp_next ? temp_next : 1;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static size_t GetPrevInCtor(const List_t* List, size_t Data_i)
{
    assert(List);
    
    size_t temp_prev = (Data_i - 1) % (GetCapacity(List)); 
    return temp_prev ? temp_prev : GetCapacity(List) - 1;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ListErrorType Verif(List_t* List, ListErrorType* Err, const char* File, const int Line, const char* Func)
{
    assert(List);
    assert(File);
    assert(Func);

    ErrPlaceCtor(Err, File, Line, Func);

    return *Err;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void ErrPlaceCtor(ListErrorType* Err, const char* File, const int Line, const char* Func)
{
    assert(Err);
    assert(File);
    assert(Func);

    Err->Place.File = File;
    Err->Place.Line = Line;
    Err->Place.Func = Func;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ConsoleDump(const List_t* List, const char* File, const int Line, const char* Func)
{
    assert(List);
    assert(File);
    assert(Func);

    COLOR_PRINT(GREEN, "DUMP BEGIN\n\n");

    COLOR_PRINT(WHITE, "Dump made int:\n");
    PrintPlace(File, Line, Func);
    printf("\n");

    COLOR_PRINT(VIOLET, "Size  = %lu\n",   GetDataSize(List));
    COLOR_PRINT(VIOLET, "Free  = %lu\n\n", GetFree(List));
    COLOR_PRINT(YELLOW, "Head  = %lu\n",   GetHead(List));
    COLOR_PRINT(YELLOW, "Tail  = %lu\n\n", GetTail(List));
    

    COLOR_PRINT(RED,  "    data  next  prev\n" );
    COLOR_PRINT(CYAN, "[ 0] %3d   %3lu  %3lu\n", List->Data[0].Elem, List->Data[0].Next, List->Data[0].Prev);
    for (size_t i = 1; i < GetCapacity(List); i++)
    {
        COLOR_PRINT(WHITE, "[%2lu] %3d   %3lu  %3lu\n", i, List->Data[i].Elem, List->Data[i].Next, List->Data[i].Prev);
    }

    printf("\n\n");

    COLOR_PRINT(GREEN, "\nDUMP END\n\n");

    return;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void PrintList(const List_t* List)
{   
    assert(List);

    COLOR_PRINT(VIOLET, "List:\n");
    size_t data_i = GetHead(List);
    for (size_t i = 0; i < GetDataSize(List); i++)
    {
        COLOR_PRINT(GREEN, "%d ", GetDataElem(List, data_i));
        data_i = GetNextIndex(List, data_i);
    }
    COLOR_PRINT(VIOLET, "\nList end:\n");

    return;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void PrintPlace(const char* File, int Line, const char* Func)
{
    assert(File);
    assert(Func);

    COLOR_PRINT(WHITE, "File [%s]\nLine [%d]\nFunc [%s]\n", File, Line, Func);
    return;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void PrintError(ListErrorType* Err)
{
    assert(Err);

    if (Err->IsFatalError == 0)
    {
        return;
    }

    if (Err->ListOverFlow == 1)
    {
        COLOR_PRINT(RED, "Error: list overflow.\n");
    }

    if (Err->ListUnderFlow == 1)
    {
        COLOR_PRINT(RED, "Error: list underflow.\n");
    }

    if (Err->CtorCallocNull == 1)
    {
        COLOR_PRINT(RED, "Error: calloc in ctor return null.\n");
    }


    return;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ListAssertPrint(ListErrorType* Err, const char* File, const int Line, const char* Func)
{
    COLOR_PRINT(RED, "Assert made in:\n");
    PrintPlace(File, Line, Func);
    COLOR_PRINT(RED, "Error is in:\n");
    PrintPlace(Err->Place.File, Err->Place.Line, Err->Place.Func);
    PrintError(Err);
    COLOR_PRINT(CYAN, "\nabort() in 3, 2, 1...\n");
    return;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void GraphicDump(const List_t* List, const char* File, const int Line, const char* Func)
{
    static size_t ImgQuant = 0;

    static const size_t MaxFileNameLen = 128;
    char outFile[MaxFileNameLen] = {};
    sprintf(outFile, "out%lu.png", ImgQuant);
    ImgQuant++;

    static const size_t MaxCommandLen = 256;
    char command[MaxCommandLen] = {};
    sprintf(command, "dot -Tpng List.dot > %s", outFile);

    GraphicDumpHelper(List, File, Line, Func);
    system(command);

    return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void GraphicDumpHelper(const List_t* List, const char* File, const int Line, const char* Func)
{
    assert(List);

    static const char* dotFileName = "List.dot";
    FILE* dotFile = fopen(dotFileName, "wb");
    assert(dotFile);

    DotBegin(dotFile);
            DotCreateDumpPlace(dotFile, File, Line, Func);

            DotCreateAllNodes(dotFile, List);
            DotCreateEdges(dotFile, List);
            DotCreateNextEdges(dotFile, List);

            DotCreateRestList(dotFile, List);
    
    DotEnd(dotFile);

    fclose(dotFile);
    dotFile = nullptr;

    return;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void DotBegin(FILE* dotFile)
{
    assert(dotFile);
    fprintf(dotFile, "digraph G{\nrankdir=LR;\ngraph [bgcolor=\"#000000\"];\n");
    return;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void DotEnd(FILE* dotFile)
{
    assert(dotFile);
    fprintf(dotFile, "}\n\n");
    return;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void DotCreateNotFreeNode(FILE* dotFile, const List_t* List, const size_t node_i)
{
    assert(dotFile);
    assert(List);

    DataInfo   Data_i = List->Data[node_i];

    ListElem_t Elem   = Data_i.Elem;
    size_t     Next   = Data_i.Next;
    size_t     Prev   = Data_i.Prev;

    fprintf(dotFile, "node%lu", node_i);
    fprintf(dotFile, "[shape=Mrecord, style=filled, fillcolor=\"#15a6bf\"");
    fprintf(dotFile, "label =\"index : %lu   ", node_i);
    fprintf(dotFile, "| data: %d ",       Elem);
    fprintf(dotFile, "| <f0> next: %lu  ",  Next);
    fprintf(dotFile, "| <f1> prev: %lu\"", Prev); 
    fprintf(dotFile, "color = \"#008080\"];\n");

    return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void DotCreateFreeNode(FILE* dotFile, const List_t* List, const size_t node_i)
{
    assert(dotFile);
    assert(List);

    DataInfo   Data_i = List->Data[node_i];

    ListElem_t Elem   = Data_i.Elem;
    size_t     Next   = Data_i.Next;
    size_t     Prev   = Data_i.Prev;

    fprintf(dotFile, "node%lu", node_i);
    fprintf(dotFile, "[shape=Mrecord, style=filled, fillcolor=\"#1775a5\"");
    fprintf(dotFile, "label =\"index : %lu   ", node_i);
    fprintf(dotFile, "| data: %d ",          Elem);
    fprintf(dotFile, "| <f0> next: %lu ",    Next);
    fprintf(dotFile, "| <f1> prev: %lu\", ", Prev); 
    fprintf(dotFile, "color = \"#008080\"];\n");

    return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void DotCreateHeadNode(FILE* dotFile, const List_t* List)
{
    assert(dotFile);
    assert(List);

    DataInfo   Data_i = List->Data[0];

    ListElem_t Elem   = Data_i.Elem;
    size_t     Next   = Data_i.Next;
    size_t     Prev   = Data_i.Prev;

    fprintf(dotFile, "node0");
    fprintf(dotFile, "[shape=Mrecord, style=filled, fillcolor=\"#17a53b\"");
    fprintf(dotFile, "label =\"index : 0 ");
    fprintf(dotFile, "| data: %d ",          Elem);
    fprintf(dotFile, "| <f0> next: %lu ",    Next);
    fprintf(dotFile, "| <f1> prev: %lu\", ", Prev); 
    fprintf(dotFile, "color = \"#008080\"];\n");

    return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void DotCreateAllNodes(FILE* dotFile, const List_t* List)
{
    assert(dotFile);
    assert(List);

    DotCreateHeadNode(dotFile, List);

    size_t size = GetDataSize(List);

    size_t now_i = GetHead(List);

    for (size_t i = 0; i < size; i++)
    {
        DotCreateNotFreeNode(dotFile, List, now_i);
        now_i = GetNextIndex(List, now_i);
    }


    size_t freeSize = GetCapacity(List) - size;
    now_i = GetFree(List);

    for (size_t i = 0; i < freeSize; i++)
    {
        DotCreateFreeNode(dotFile, List, now_i);
        now_i = GetNextIndex(List, now_i);
    }

    return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void DotCreateNextEdges(FILE* dotFile, const List_t* List)
{
    assert(dotFile);
    assert(List);

    fprintf(dotFile, "\nedge[color = \"#c91c14\", fontsize = 12, constraint=false];\n");

    size_t size  = List->Size;
    size_t now_i = GetHead(List);

    for (size_t i = 0; i < size + 1; i++)
    {
        size_t next = GetNextIndex(List, now_i);
        fprintf(dotFile, "node%lu:<f0>->node%lu:<f0>;\n", now_i, next);
        now_i = next;
    }



    fprintf(dotFile, "\nedge[color = \"#17a927\", fontsize = 12, constraint=false];\n");
    
    size_t freeSize = List->Capacity - size;

    COLOR_PRINT(VIOLET, "free size = %lu\n", freeSize);
    now_i = List->Free;

    for (size_t i = 0; i < freeSize - 1; i++)
    {
        size_t next = GetNextIndex(List, now_i);
        fprintf(dotFile, "node%lu:<f0>->node%lu:<f0>;\n", now_i, next);
        now_i = next;
    }


    return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void DotCreatePrevEdges(FILE* dotFile, const List_t* List)
{
    assert(dotFile);
    assert(List);

    fprintf(dotFile, "edge[color=\"red\", fontsize=12, constraint=false];\n");

    for (size_t node_i = 0; node_i < GetCapacity(List); node_i++)
    {
        fprintf(dotFile, "node%lu->node%lu;\n", node_i, GetPrevIndex(List, node_i));
    }

    return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void DotCreateEdges(FILE* dotFile, const List_t* List)
{
    assert(dotFile);
    assert(List);

    fprintf(dotFile, "node0");    
    for (size_t node_i = 1; node_i < List->Capacity; node_i++)
    {
        fprintf(dotFile, "->node%zu", node_i);
    }

    return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void DotCreateRestList(FILE* dotFile, const List_t* List)
{
    assert(dotFile);
    assert(List);
    
    fprintf(dotFile, "node[shape = octagon, style = \"filled\", fillcolor = \"lightgray\"];\n");
    fprintf(dotFile, "edge[color = \"cyan\"];\n");
    fprintf(dotFile, "head->node%zu;\n", GetHead(List));
    fprintf(dotFile, "tail->node%zu;\n", GetTail(List));
    fprintf(dotFile, "free->node%zu;\n", GetFree(List));
    fprintf(dotFile, "nodeInfo[shape = Mrecord, style = filled, fillcolor=\"#70443b\",");
    fprintf(dotFile, "label=\"capacity: %zu ", GetCapacity(List));
    fprintf(dotFile, "| size : %zu\"];\n",     GetDataSize(List));   
    return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void DotCreateDumpPlace(FILE* dotFile, const char* File, const int Line, const char* Func)
{
    assert(dotFile);

    fprintf(dotFile, "place");
    fprintf(dotFile, "[shape=Mrecord, style=filled, fillcolor=\"#70443b\"");
    fprintf(dotFile, "label  =\"Dump place:");
    fprintf(dotFile, "| file: %s ",        File);
    fprintf(dotFile, "| <f0> line: %d ",   Line);
    fprintf(dotFile, "| <f1> func: %s\",", Func); 
    fprintf(dotFile, "color = \"#000000\"];\n"); 
    return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
