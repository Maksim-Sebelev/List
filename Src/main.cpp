#include "list/list.hpp"
#include "list/err_parse/err_parse.hpp"
#include "list/list_dump/list_gpraphic_dump.hpp"
#include "list/list_dump/list_console_dump.hpp"

int main()
{
    List_t list = {};

    LIST_CTOR(&list, 16);

    size_t      data_pointer = 0;
    list_elem_t value        = 0;

    for (int i = 0; i < 16; i++)
        LIST_PUSH_BACK(&list, i + 101, &data_pointer);

    // LIST_PUSH_FRONT(&list, 111, &data_pointer);

    // LIST_ERASE(&list, 5, &value);

    LIST_PRINT        (&list);
    LIST_CONSOLE_DUMP (&list);
    LIST_GRAPHIC_DUMP (&list);

    LIST_DTOR(&list);

    return 0;
}


