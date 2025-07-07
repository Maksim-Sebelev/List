#include "list/list.hpp"
#include "list/err_parse/err_parse.hpp"
#include "list/list_dump/list_gpraphic_dump.hpp"
#include "list/list_dump/list_console_dump.hpp"

#include "lib/lib.hpp"
#include "lib/logger/log.hpp"

int main()
{
    LOG_OPEN();

    List_t list = {};

    LIST_CTOR(&list, 16);

    size_t      data_pointer = 0;
    
    for (int i = 0; i < 10; i++)
        LIST_PUSH_BACK(&list, i + 101, &data_pointer);

    LIST_INSERT_AFTER(&list, 5, 333, &data_pointer);

    LIST_PRINT        (&list);
    LIST_CONSOLE_DUMP (&list);
    LIST_GRAPHIC_DUMP (&list);

    // for (int i = 0; i < 15; i++)
    // {    
    //     list_elem_t value        = 0;
    //     LIST_POP_BACK(&list, &value);
    // }

    // LIST_PRINT        (&list);
    // LIST_CONSOLE_DUMP (&list);
    // LIST_GRAPHIC_DUMP (&list);

    LIST_DTOR(&list);

    LOG_CLOSE();

    return 0;
}


