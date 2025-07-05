#include <stdlib.h>
#include <assert.h>
#include "list/list.h"
#include "lib/lib.hpp"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void GraphicDump(const List_t* List, const char* File, const int Line, const char* Func)
{
    assert(List);
    assert(File);
    assert(Func);

    if (system("mkdir -p dot/")     != 0 ||
        system("mkdir -p dot/dot/") != 0 ||
        system("mkdir -p dot/img")  != 0)
    {
        EXIT(EXIT_FAILURE, "failed creat directory for graphic dump.");
    }

    static size_t ImgQuant = 1;

    static const size_t MaxFileNameLen = 128;
    char outFile[MaxFileNameLen] = {};
    sprintf(outFile, "dot/img/out%lu.png", ImgQuant);
    ImgQuant++;

    static const size_t MaxCommandLen = 256;
    char command[MaxCommandLen] = {};
    sprintf(command, "dot -Tpng dot/dot/list.dot > %s", outFile);

    GraphicDumpHelper(List, File, Line, Func);
    
    if (system(command) != 0)
        EXIT(EXIT_FAILURE, "failed made command\n"WHITE"'%s'", command);

    return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void GraphicDumpHelper(const List_t* List, const char* File, const int Line, const char* Func)
{
    assert(List);

    static const char* dotFileName = "dot/dot/list.dot";
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

    ListElem   list_elem = List->data[node_i];

    list_elem_t value  = list_elem.value;
    size_t      next   = list_elem.next ;
    size_t      prev   = list_elem.prev ;

    fprintf(dotFile, "node%lu", node_i);
    fprintf(dotFile, "[shape=Mrecord, style=filled, fillcolor=\"#15a6bf\"");
    fprintf(dotFile, "label =\"index : %lu ", node_i                      );
    fprintf(dotFile, "| data: %d "          ,  value                      );
    fprintf(dotFile, "| <f0> next: %lu "    ,  next                       );
    fprintf(dotFile, "| <f1> prev: %lu \""  ,  prev                       ); 
    fprintf(dotFile, "color = \"#008080\"];\n"                            );

    return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void DotCreateFreeNode(FILE* dotFile, const List_t* List, const size_t node_i)
{
    assert(dotFile);
    assert(List);

    ListElem   list_elem = List->data[node_i];

    list_elem_t value   = list_elem.value;
    size_t     next   = list_elem.next;
    size_t     prev   = list_elem.prev;

    fprintf(dotFile, "node%lu", node_i);
    fprintf(dotFile, "[shape=Mrecord, style=filled, fillcolor=\"#1775a5\"");
    fprintf(dotFile, "label =\"index : %lu   ", node_i);
    fprintf(dotFile, "| data: %d ",          value);
    fprintf(dotFile, "| <f0> next: %lu ",    next);
    fprintf(dotFile, "| <f1> prev: %lu\", ", prev); 
    fprintf(dotFile, "color = \"#008080\"];\n");

    return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void DotCreateHeadNode(FILE* dotFile, const List_t* List)
{
    assert(dotFile);
    assert(List);

    ListElem   list_elem = List->data[0];

    list_elem_t value   = list_elem.value;
    size_t     next   = list_elem.next;
    size_t     prev   = list_elem.prev;

    fprintf(dotFile, "node0");
    fprintf(dotFile, "[shape=Mrecord, style=filled, fillcolor=\"#17a53b\"");
    fprintf(dotFile, "label =\"index : 0 ");
    fprintf(dotFile, "| data: %d ",          value);
    fprintf(dotFile, "| <f0> next: %lu ",    next);
    fprintf(dotFile, "| <f1> prev: %lu\", ", prev); 
    fprintf(dotFile, "color = \"#008080\"];\n");

    return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void DotCreateAllNodes(FILE* dotFile, const List_t* List)
{
    assert(dotFile);
    assert(List);

    DotCreateHeadNode(dotFile, List);

    const size_t size = GetDataSize(List);

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

    size_t size  = List->size;
    size_t now_i = GetHead(List);

    for (size_t i = 0; i < size + 1; i++)
    {
        size_t next = GetNextIndex(List, now_i);
        fprintf(dotFile, "node%lu:<f0>->node%lu:<f0>;\n", now_i, next);
        now_i = next;
    }



    fprintf(dotFile, "\nedge[color = \"#17a927\", fontsize = 12, constraint=false];\n");
    
    size_t freeSize = List->capacity - size;

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
    for (size_t node_i = 1; node_i < List->capacity; node_i++)
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
