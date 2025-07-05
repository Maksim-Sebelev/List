#include "list/list.hpp"
#include "list/err_parse/err_parse.hpp"

int main()
{
    List_t list = {};

    LIST_ASSERT(&list, ListCtor(&list, 19));



    return 0;
}


