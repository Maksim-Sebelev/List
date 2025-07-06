#include "list/list.hpp"
#include "list/err_parse/err_parse.hpp"

int main()
{
    List_t list = {};


    ListError_t err = ListCtor(&list, 19);

    TRANSFER_ERROR(&err);

    TRANSFER_ERROR(&err);


    LIST_ASSERT(&list, err);

    LIST_ASSERT(&list, ListDtor(&list));

    return 0;
}


