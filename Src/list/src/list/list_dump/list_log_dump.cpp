
void ConsoleDump(const List_t* List, const char* File, const int Line, const char* Func)
{
    assert(List);
    assert(File);
    assert(Func);

    COLOR_PRINT(GREEN, "DUMP BEGIN\n\n");

    COLOR_PRINT(WHITE, "Dump made int:\n");
    PrintPlace(File, Line, Func);
    printf("\n");

    COLOR_PRINT(VIOLET, "size  = %lu\n",   GetDataSize(List));
    COLOR_PRINT(VIOLET, "Free  = %lu\n\n", GetFree(List));
    COLOR_PRINT(YELLOW, "Head  = %lu\n",   GetHead(List));
    COLOR_PRINT(YELLOW, "Tail  = %lu\n\n", GetTail(List));
    

    COLOR_PRINT(RED,  "    data  next  prev\n" );
    COLOR_PRINT(CYAN, "[ 0] %3d   %3lu  %3lu\n", List->data[0].Elem, List->data[0].Next, List->data[0].Prev);
    for (size_t i = 1; i < GetCapacity(List); i++)
    {
        COLOR_PRINT(WHITE, "[%2lu] %3d   %3lu  %3lu\n", i, List->data[i].Elem, List->data[i].Next, List->data[i].Prev);
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
