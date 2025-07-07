#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "lib/lib.hpp"
#include "list/list.hpp"
#include "list/err_parse/err_parse.hpp"

#include "list/list_dump/list_gpraphic_dump.hpp"
#include "list/list_dump/list_console_dump.hpp"
#include "lib/logger/log.hpp"


//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static const size_t increase_realloc_coef = 2;
static const size_t decrease_realloc_coef = 4;

static const size_t min_capacity = 16;
static const size_t max_capacity = (size_t) 1 << (sizeof(max_capacity) * 8 - 1); // max 2^n in size_t // 8 = bits im byte

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static_assert(decrease_realloc_coef > increase_realloc_coef);
static_assert(min_capacity >= decrease_realloc_coef, "");
static_assert(max_capacity >= min_capacity, "");

static_assert(increase_realloc_coef > 1, "");
static_assert(decrease_realloc_coef > 1, "");

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static ListElem     ListElemCtor                 (const list_elem_t value, const size_t next, const size_t prev);
static bool         IsListFull                   (const List_t* list);
static bool         IsListEmpty                  (const List_t* list);
static bool         IsListReadyForDecreaseRealloc(const List_t* list);
static ListError_t  ListIncreaseRealloc          (List_t* list);
static ListError_t  ListDecreaseRealloc          (List_t* list);

static ListError_t  UpdateFreeAfterIncreaseRealloc(List_t* list, size_t old_capacity, size_t new_capacity);
static ListError_t  UpdateFreeAfterDecreaseRealloc(List_t* list, size_t new_capcity);

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ListError_t ListCtor(List_t* list, size_t capacity)
{
    LOG_PLACE(Green);  LOG_NS(); assert(list);

    list->capacity = (capacity > min_capacity) ? capacity : min_capacity;

    list->size     = 0;
    list->data     = (ListElem*) calloc(capacity + 1, sizeof(ListElem)); // +1 for null element

    if (!list->data)
        return GET_STATUS_ERR(ListErrorType::FAILED_ALLOCATE_MEMORY_IN_CTOR);

    // list->data[GetTail(list)].prev = list->data[GetTail(list)].prev;
    list->data[0]            .next = list->data[GetTail(list)].next;
    list->data[GetTail(list)].next = 0;
    list->free                     = 1;

    list->data[0       ]           = ListElemCtor(0, 0, 0           );
    list->data[1       ]           = ListElemCtor(0, 2, capacity    );
    list->data[capacity]           = ListElemCtor(0, 1, capacity - 1);

    for (size_t i = 2; i < capacity; i++)
        list->data[i] = ListElemCtor(0, i + 1, i - 1);

    return GET_STATUS_OK();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ListError_t ListDtor(List_t* list)
{
    LOG_PLACE(Green);  LOG_NS(); assert(list);

    ListElem* data = list->data;

    if (!data)
        return GET_STATUS_ERR(ListErrorType::TRY_TO_DTOR_NULLPTR_DATA);

    FREE(list->data);

    *list = {};

    return GET_STATUS_OK();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ListError_t ListInsertAfter(List_t* list, const size_t ref_elem, const list_elem_t insert_elem, size_t* insert_place)
{
    LOG_PLACE(Green);  LOG_NS(); assert(list);
    assert(insert_place);

    ListError_t err = {};

    if (IsListFull(list))
        RETURN_IF_ERR_OR_WARN(ListIncreaseRealloc(list));

    list->size++;
 
    size_t free        = GetFree     (list          );
    size_t free_prev   = GetPrevIndex(list, free    );
    size_t free_next   = GetNextIndex(list, free    );
    size_t ref_next    = GetNextIndex(list, ref_elem);

    *insert_place               = free       ;

    list->data[free     ].value = insert_elem;
    list->data[ref_elem ].next  = free       ;
    list->data[ref_next ].prev  = free       ;
    list->data[free     ].prev  = ref_elem   ;
    list->data[free     ].next  = ref_next   ;

    list->free                  = free_next  ;
    list->data[free_next].prev  = free_prev  ;
    list->data[free_prev].next  = free_next  ;

    return GET_STATUS_OK();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ListError_t ListInsertBefore(List_t* list, const size_t ref_elem, const list_elem_t insert_elem, size_t* insert_place)
{
    LOG_PLACE(Green);  LOG_NS(); assert(list);
    assert(insert_place);

    ListError_t err = {};

    if (IsListFull(list))
        RETURN_IF_ERR_OR_WARN(ListIncreaseRealloc(list));

    list->size++;

    size_t free       = GetFree(list);
    size_t free_prev   = GetPrevIndex(list, free);
    size_t free_next   = GetNextIndex(list, free);
    size_t PrevRef    = GetPrevIndex(list, ref_elem);

    *insert_place              = free;
    list->data[free]    .value = insert_elem;
    list->data[free]    .next = ref_elem;
    list->data[free]    .prev = PrevRef;
    list->data[ref_elem] .prev = free;
    list->data[PrevRef] .next = free;
    list->free                = free_next;
    list->data[free_next].prev = free_prev;
    list->data[free_prev].next = free_next;

    return GET_STATUS_OK();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ListError_t ListErase(List_t* list, size_t erase_place, list_elem_t* erase_elem)
{
    LOG_PLACE(Green);  LOG_NS(); assert(list);
    assert(erase_elem);

    ListError_t err = {};

    if (IsListEmpty(list))
        return GET_STATUS_WARN(ListWarningType::ERASE_IN_EMPTY_LIST);


    if (IsListReadyForDecreaseRealloc(list))
        RETURN_IF_ERR_OR_WARN(ListDecreaseRealloc(list));
    
    // LIST_CONSOLE_DUMP(list);
    // LIST_PRINT(list);
    // LIST_GRAPHIC_DUMP(list);

    list->size--;

    size_t free       = GetFree(list);
    size_t free_prev  = GetPrevIndex(list, free);
    size_t free_next  = GetNextIndex(list, free);
    size_t erase_prev = GetPrevIndex(list, erase_place);
    size_t erase_next = GetNextIndex(list, erase_place);

    *erase_elem                   = GetDataElemValue(list, erase_place);
    list->data[erase_place].value = 0;

    list->data[erase_place].next = free;
    list->data[erase_place].prev = free_prev;

    list->data[erase_prev ].next = erase_next;
    list->data[erase_next ].prev = erase_prev;

    list->data[free       ].prev = free;
    list->free                   = erase_place;
    list->data[free_prev  ].next = erase_place;
    list->data[free_next  ].prev = erase_place;
    
    return GET_STATUS_OK();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ListError_t ListPushBack(List_t* list, const list_elem_t push_elem, size_t* push_place)
{
    LOG_PLACE(Green);  LOG_NS(); assert(list);
    assert(push_place);
    
    ListError_t err = ListInsertAfter(list, GetTail(list), push_elem, push_place);

    return TRANSFER_ERROR(&err);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ListError_t ListPushFront(List_t* list, const list_elem_t push_elem, size_t* push_place)
{
    LOG_PLACE(Green);  LOG_NS(); assert(list);
    assert(push_place);

    ListError_t err = ListInsertBefore(list, GetHead(list), push_elem, push_place);

    return TRANSFER_ERROR(&err);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ListError_t ListPopBack(List_t* list, list_elem_t* pop_elem)
{
    LOG_PLACE(Green);  LOG_NS(); assert(list);
    assert(pop_elem);

    ListError_t err = ListErase(list, GetTail(list), pop_elem);
    
    return TRANSFER_ERROR(&err);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ListError_t ListPopFront(List_t* list, list_elem_t* pop_elem)
{
    LOG_PLACE(Green);  LOG_NS(); assert(list);
    assert(pop_elem);

    ListError_t err = ListErase(list, GetHead(list), pop_elem);

    return TRANSFER_ERROR(&err);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static ListError_t ListIncreaseRealloc(List_t* list)
{
    LOG_PLACE(Green);  LOG_NS(); assert(list);

    const size_t now_capacity = GetCapacity(list);

    if (now_capacity > max_capacity / increase_realloc_coef)
        return GET_STATUS_WARN(ListWarningType::TO_BIG_CAPACITY);

    const size_t new_capacity  = increase_realloc_coef * now_capacity; 
    const size_t realloc_size  = (new_capacity + 1)  * sizeof(ListElem);

    list->data = (ListElem*) realloc(list->data, realloc_size);

    if (!list->data)
        return GET_STATUS_WARN(ListWarningType::FAILED_REALLOCATE_DATA_AFTER_INSTERT);
    
    list->capacity = new_capacity;

    RETURN_IF_ERR_OR_WARN(
    UpdateFreeAfterIncreaseRealloc(list, now_capacity, new_capacity)
    );

    return GET_STATUS_OK();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static ListError_t UpdateFreeAfterIncreaseRealloc(List_t* list, size_t old_capacity, size_t new_capacity)
{
    LOG_PLACE(Green);  LOG_NS(); assert(list);

    list->free = old_capacity;

    ListElem* data = list->data; assert(data);

    data[old_capacity] = ListElemCtor(0, old_capacity + 1, new_capacity    );
    data[new_capacity] = ListElemCtor(0, old_capacity    , new_capacity - 1);

    for (size_t i = old_capacity + 1; i < new_capacity; i++)
        data[i] = ListElemCtor(0, i + 1, i - 1);

    return GET_STATUS_OK();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static ListError_t ListDecreaseRealloc(List_t* list)
{
    LOG_PLACE(Green);  LOG_NS(); assert(list);

    const size_t old_capacity     = GetCapacity(list);
    const size_t tmp_new_capacity = old_capacity / decrease_realloc_coef;
    const size_t new_capacity     = (tmp_new_capacity > min_capacity) ?
                                     tmp_new_capacity : min_capacity;

    if (old_capacity == new_capacity)
        return GET_STATUS_OK();

    const size_t realloc_size = (new_capacity) * sizeof(list->data[0]);

    list->data = (ListElem*) realloc(list->data, realloc_size);

    if (!list->data)
        return GET_STATUS_WARN(ListWarningType::FAILED_REALLOCATE_DATA_AFTER_ERASE);

    // RETURN_IF_ERR_OR_WARN(
    // UpdateFreeAfterDecreaseRealloc(list, new_capacity);
    // );

    return GET_STATUS_OK();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static ListError_t UpdateFreeAfterDecreaseRealloc(List_t* list, size_t new_capcity)
{
    LOG_PLACE(Green);  LOG_NS(); assert(list);

    ListElem*    data = list->data;
    const size_t size = list->size;

    const size_t capacity = new_capcity;

    size_t data_pointer = list->free;

    for (size_t i = size; i < capacity; i++)
    {
        data[data_pointer] = ListElemCtor(0, GetNextIndex(list, data_pointer), 0);
        data_pointer = GetNextIndex(list, data_pointer);
    }

    return GET_STATUS_OK();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static ListElem ListElemCtor(const list_elem_t value, const size_t next, const size_t prev)
{
    return (ListElem)
    {
        .value = value,
        .next  = next ,
        .prev  = prev ,
    };
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

list_elem_t GetDataElemValue(const List_t* list, size_t data_i)
{
    LOG_PLACE(Green);  LOG_NS(); assert(list);
    return list->data[data_i].value;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

size_t GetTail(const List_t* list)
{
    LOG_PLACE(Green);  LOG_NS(); assert(list);
    return list->data[0].prev;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

size_t GetNextIndex(const List_t* list, size_t NowIndex)
{
    LOG_PLACE(Green);  LOG_NS(); assert(list);
    return list->data[NowIndex].next;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

size_t GetPrevIndex(const List_t* list, size_t NowIndex)
{
    LOG_PLACE(Green);  LOG_NS(); assert(list);
    return list->data[NowIndex].prev;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

size_t GetHead(const List_t* list)
{
    LOG_PLACE(Green);  LOG_NS(); assert(list);
    return list->data[0].next;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

size_t GetFree(const List_t* list)
{
    LOG_PLACE(Green);  LOG_NS(); assert(list);
    return list->free;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

size_t GetCapacity(const List_t* list)
{
    LOG_PLACE(Green);  LOG_NS(); assert(list);
    return list->capacity;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

size_t GetDataSize(const List_t* list)
{
    LOG_PLACE(Green);  LOG_NS(); assert(list);
    return list->size;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool IsListFull(const List_t* list)
{
    LOG_PLACE(Green);  LOG_NS(); assert(list);
    return GetCapacity(list) == GetDataSize(list) + 1;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool IsListEmpty(const List_t* list)
{
    LOG_PLACE(Green);  LOG_NS(); assert(list);
    return GetDataSize(list) == 0;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool IsListReadyForDecreaseRealloc(const List_t* list)
{
    LOG_PLACE(Green);  LOG_NS(); assert(list);

    const size_t size     = GetDataSize(list); assert(size >= 1);
    const size_t capacity = GetCapacity(list);

    return size <= capacity / decrease_realloc_coef;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
