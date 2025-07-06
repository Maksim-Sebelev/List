#include <stdlib.h>
#include <assert.h>
#include "list/list.hpp"
#include "lib/lib.hpp"
#include "lib/functions_for_files/files.hpp"


static void GraphicDumpHelper(const List_t* list, const char* File, const int Line, const char* Func);
static void DotBegin(FILE* dot_file);
static void DotEnd(FILE* dot_file);
static void DotCreateNotFreeNode(FILE* dot_file, const List_t* list, const size_t node_i);
static void DotCreateFreeNode(FILE* dot_file, const List_t* list, const size_t node_i);
static void DotCreateHeadNode(FILE* dot_file, const List_t* list);
static void DotCreateAllNodes(FILE* dot_file, const List_t* list);
static void DotCreateNextEdges(FILE* dot_file, const List_t* list);
static void DotCreatePrevEdges(FILE* dot_file, const List_t* list);
static void DotCreateEdges(FILE* dot_file, const List_t* list);
static void DotCreateRestList(FILE* dot_file, const List_t* list);
static void DotCreateDumpPlace(FILE* dot_file, const char* File, const int Line, const char* Func);

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void GraphicDump(const List_t* list, const char* File, const int Line, const char* Func)
{
    assert(list);
    assert(File);
    assert(Func);

    if (system("mkdir -p ../dot/")     != 0 ||
        system("mkdir -p ../dot/dot/") != 0 ||
        system("mkdir -p ../dot/img")  != 0)
    {
        EXIT(EXIT_FAILURE, "failed creat directory for graphic dump.");
    }

    static size_t ImgQuant = 1;

    static const size_t MaxFileNameLen = 128;
    char outFile[MaxFileNameLen] = {};
    sprintf(outFile, "../dot/img/out%lu.png", ImgQuant);
    ImgQuant++;

    static const size_t MaxCommandLen = 256;
    char command[MaxCommandLen] = {};
    sprintf(command, "dot -Tpng ../dot/dot/list.dot > %s", outFile);

    GraphicDumpHelper(list, File, Line, Func);
    
    if (system(command) != 0)
        EXIT(EXIT_FAILURE, "failed made command\n"WHITE"'%s'", command);

    return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void GraphicDumpHelper(const List_t* list, const char* File, const int Line, const char* Func)
{
    assert(list);

    static const char* dotFileName = "../dot/dot/list.dot";
    FILE* dot_file = SafeFopen(dotFileName, "wb");
    assert(dot_file);

    DotBegin          (dot_file);
    DotCreateDumpPlace(dot_file, File, Line, Func);
    DotCreateAllNodes (dot_file, list);
    DotCreateEdges    (dot_file, list);
    DotCreateNextEdges(dot_file, list);
    DotCreateRestList (dot_file, list);
    DotEnd            (dot_file);

    SafeFclose(dot_file);

    return;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void DotBegin(FILE* dot_file)
{
    assert(dot_file);
    fprintf(dot_file, "digraph G{\nrankdir=LR;\ngraph [bgcolor=\"#000000\"];\n");
    return;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void DotEnd(FILE* dot_file)
{
    assert(dot_file);
    fprintf(dot_file, "}\n\n");
    return;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void DotCreateNotFreeNode(FILE* dot_file, const List_t* list, const size_t node_i)
{
    assert(dot_file);
    assert(list);

    ListElem   list_elem = list->data[node_i];

    list_elem_t value  = list_elem.value;
    size_t      next   = list_elem.next ;
    size_t      prev   = list_elem.prev ;

    fprintf(dot_file, "node%lu", node_i);
    fprintf(dot_file, "[shape=Mrecord, style=filled, fillcolor=\"#15a6bf\"");
    fprintf(dot_file, "label =\"index : %lu ", node_i                      );
    fprintf(dot_file, "| data: %d "          ,  value                      );
    fprintf(dot_file, "| <f0> next: %lu "    ,  next                       );
    fprintf(dot_file, "| <f1> prev: %lu \""  ,  prev                       ); 
    fprintf(dot_file, "color = \"#008080\"];\n"                            );

    return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void DotCreateFreeNode(FILE* dot_file, const List_t* list, const size_t node_i)
{
    assert(dot_file);
    assert(list);

    ListElem   list_elem = list->data[node_i];

    list_elem_t value   = list_elem.value;
    size_t      next    = list_elem.next;
    size_t      prev    = list_elem.prev;

    fprintf(dot_file, "node%lu", node_i);
    fprintf(dot_file, "[shape=Mrecord, style=filled, fillcolor=\"#1775a5\"");
    fprintf(dot_file, "label =\"index : %lu   ", node_i);
    fprintf(dot_file, "| data: %d ",          value);
    fprintf(dot_file, "| <f0> next: %lu ",    next);
    fprintf(dot_file, "| <f1> prev: %lu\", ", prev); 
    fprintf(dot_file, "color = \"#008080\"];\n");

    return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void DotCreateHeadNode(FILE* dot_file, const List_t* list)
{
    assert(dot_file);
    assert(list);

    ListElem   list_elem = list->data[0];

    list_elem_t value   = list_elem.value;
    size_t     next   = list_elem.next;
    size_t     prev   = list_elem.prev;

    fprintf(dot_file, "node0");
    fprintf(dot_file, "[shape=Mrecord, style=filled, fillcolor=\"#17a53b\"");
    fprintf(dot_file, "label =\"index : 0 ");
    fprintf(dot_file, "| data: %d ",          value);
    fprintf(dot_file, "| <f0> next: %lu ",    next);
    fprintf(dot_file, "| <f1> prev: %lu\", ", prev); 
    fprintf(dot_file, "color = \"#008080\"];\n");

    return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void DotCreateAllNodes(FILE* dot_file, const List_t* list)
{
    assert(dot_file);
    assert(list);

    DotCreateHeadNode(dot_file, list);

    const size_t size = GetDataSize(list);

    size_t now_i = GetHead(list);

    for (size_t i = 0; i < size; i++)
    {
        DotCreateNotFreeNode(dot_file, list, now_i);
        now_i = GetNextIndex(list, now_i);
    }


    size_t freeSize = GetCapacity(list) - size;
    now_i = GetFree(list);

    for (size_t i = 0; i < freeSize; i++)
    {
        DotCreateFreeNode(dot_file, list, now_i);
        now_i = GetNextIndex(list, now_i);
    }

    return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void DotCreateNextEdges(FILE* dot_file, const List_t* list)
{
    assert(dot_file);
    assert(list);

    fprintf(dot_file, "\nedge[color = \"#c91c14\", fontsize = 12, constraint=false];\n");

    size_t size  = list->size;
    size_t now_i = GetHead(list);

    for (size_t i = 0; i < size + 1; i++)
    {
        size_t next = GetNextIndex(list, now_i);
        fprintf(dot_file, "node%lu:<f0>->node%lu:<f0>;\n", now_i, next);
        now_i = next;
    }



    fprintf(dot_file, "\nedge[color = \"#17a927\", fontsize = 12, constraint=false];\n");
    
    size_t freeSize = list->capacity - size;

    now_i = list->free;

    for (size_t i = 0; i < freeSize - 1; i++)
    {
        size_t next = GetNextIndex(list, now_i);
        fprintf(dot_file, "node%lu:<f0>->node%lu:<f0>;\n", now_i, next);
        now_i = next;
    }


    return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void DotCreatePrevEdges(FILE* dot_file, const List_t* list)
{
    assert(dot_file);
    assert(list);

    fprintf(dot_file, "edge[color=\"red\", fontsize=12, constraint=false];\n");

    for (size_t node_i = 0; node_i < GetCapacity(list); node_i++)
    {
        fprintf(dot_file, "node%lu->node%lu;\n", node_i, GetPrevIndex(list, node_i));
    }

    return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void DotCreateEdges(FILE* dot_file, const List_t* list)
{
    assert(dot_file);
    assert(list);

    fprintf(dot_file, "node0");    
    for (size_t node_i = 1; node_i < list->capacity; node_i++)
    {
        fprintf(dot_file, "->node%zu", node_i);
    }

    return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void DotCreateRestList(FILE* dot_file, const List_t* list)
{
    assert(dot_file);
    assert(list);
    
    fprintf(dot_file, "node[shape = octagon, style = \"filled\", fillcolor = \"lightgray\"];\n");
    fprintf(dot_file, "edge[color = \"cyan\"];\n");
    fprintf(dot_file, "free->node%zu;\n", GetFree(list));
    fprintf(dot_file, "edge[color = \"red\"];\n");
    fprintf(dot_file, "head->node%zu;\n", GetHead(list));
    fprintf(dot_file, "edge[color = \"white\"];\n");
    fprintf(dot_file, "tail->node%zu;\n", GetTail(list));
    fprintf(dot_file, "nodeInfo[shape = Mrecord, style = filled, fillcolor=\"#70443b\",");
    fprintf(dot_file, "label=\"capacity: %zu ", GetCapacity(list));
    fprintf(dot_file, "| size : %zu\"];\n",     GetDataSize(list));   
    return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void DotCreateDumpPlace(FILE* dot_file, const char* File, const int Line, const char* Func)
{
    assert(dot_file);

    fprintf(dot_file, "place");
    fprintf(dot_file, "[shape=Mrecord, style=filled, fillcolor=\"#70443b\"");
    fprintf(dot_file, "label  =\"Dump place:");
    fprintf(dot_file, "| file: %s ",        File);
    fprintf(dot_file, "| <f0> line: %d ",   Line);
    fprintf(dot_file, "| <f1> func: %s\",", Func); 
    fprintf(dot_file, "color = \"#000000\"];\n"); 
    return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
