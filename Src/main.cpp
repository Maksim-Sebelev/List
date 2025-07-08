#include "list_public/list_public.hpp"
#include "lib/lib.hpp"

int main()
{
    ON_DEBUG(
    LOG_OPEN();
    )

    List_t list = {};

    LIST_CTOR(&list, 16);

    size_t      data_pointer = 0;
    list_elem_t value = 0;

    // const size_t a = ~ (size_t) 0; 
    const size_t a = 17;

    for (size_t i = 0; i < a; i++)
        LIST_PUSH_BACK(&list, (int) i + 101, &data_pointer);

    // LIST_INSERT_AFTER(&list, 40, 333, &data_pointer);
    // LIST_ERASE(&list, 10, &value);
    // LIST_ERASE(&list, 9, &value);
    // LIST_ERASE(&list, 8, &value);
    // LIST_ERASE(&list, 7, &value);
    // LIST_ERASE(&list, 6, &value);
    // LIST_ERASE(&list, 5, &value);

    // ON_DEBUG(
    // LIST_PRINT        (&list);
    // LIST_CONSOLE_DUMP (&list);
    // LIST_GRAPHIC_DUMP (&list);
    // )

    for (int i = 0; i < 10; i++)
        LIST_POP_FRONT(&list, &value);

    ON_DEBUG(
    LIST_PRINT        (&list);
    LIST_CONSOLE_DUMP (&list);
    LIST_GRAPHIC_DUMP (&list);
    )

    LIST_DTOR(&list);

    ON_DEBUG(
    LOG_CLOSE();
    )

    return 0;
}


