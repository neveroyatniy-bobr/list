#include "list.hpp"

#include <assert.h>

#include "color.hpp"

const char* ListStrError(ListError error) {
    switch (error)
    {
    case LIST_OK:
        return "Выполенено без ошибок\n";
        break;
    case LIST_DATA_VECTOR_ERROR:
        return "Ошибка в векторе data\n";
        break;
    case LIST_NEXT_VECTOR_ERROR:
        return "Ошибка в векторе next\n";
        break;
    case LIST_PREF_VECTOR_ERROR:
        return "Ошибка в векторе pref\n";
        break;
    case DELETE_FECTIVE_ELEM:
        return "Попытка удалить фективный элемент\n";
        break;
    default:
        return "Непредвиденная ошибка\n";
        break;
    }
}

void ListPrintError(List* list, const char* file, int line) {
    assert(list);

    fprintf(stderr, RED "ERROR IN %s:%d:\n%s" reset, file, line, ListStrError(list->last_error));
}

ListError ListInit(List* list) {
    assert(list != NULL);

    if (VectorInit(&list->data, 0, sizeof(list_elem_t)) != VECTOR_OK) { 
        return list->last_error = LIST_DATA_VECTOR_ERROR;
    }

    if (VectorInit(&list->next, 0, sizeof(size_t)) != VECTOR_OK) {
        return list->last_error = LIST_NEXT_VECTOR_ERROR;
    }
    
    if (VectorInit(&list->pref, 0, sizeof(size_t)) != VECTOR_OK) {
        return list->last_error = LIST_PREF_VECTOR_ERROR;
    }

    if (VectorPush(&list->data, (void*)const_cast<list_elem_t*>(&FECTIVE_ELEM_VALUE)) != VECTOR_OK) {
        return LIST_DATA_VECTOR_ERROR;
    }

    size_t fective_id = 0;

    if (VectorPush(&list->next, &fective_id) != VECTOR_OK) {
        return LIST_NEXT_VECTOR_ERROR;
    }

    if (VectorPush(&list->pref, &fective_id) != VECTOR_OK) {
        return LIST_PREF_VECTOR_ERROR;
    }

    list->free = NO_FREE;

    LIST_CHECK(list);

    return list->last_error = LIST_OK;
}

ListError ListDestroy(List* list) {
    assert(list != NULL);

    LIST_CHECK(list);

    if (VectorFree(&list->data) != VECTOR_OK) {
        return list->last_error = LIST_DATA_VECTOR_ERROR;
    }

    if (VectorFree(&list->next) != VECTOR_OK) {
        return list->last_error = LIST_NEXT_VECTOR_ERROR;
    }

    if (VectorFree(&list->pref) != VECTOR_OK) {
        return list->last_error = LIST_PREF_VECTOR_ERROR;
    }

    return list->last_error = LIST_OK;
}

ListError ListExpansion(List* list) {
    int pushed_elem = 0;
    if (VectorPush(&list->data, &pushed_elem) != VECTOR_OK) {
        return list->last_error = LIST_DATA_VECTOR_ERROR;
    }

    if (VectorPush(&list->next, (void*)const_cast<size_t*>(&NO_FREE)) != VECTOR_OK) {
        return list->last_error = LIST_NEXT_VECTOR_ERROR;
    }

    if (VectorPush(&list->pref, (void*)const_cast<size_t*>(&NO_FREE)) != VECTOR_OK) {
        return list->last_error = LIST_PREF_VECTOR_ERROR;
    }

    list->free = list->data.size - 1;

    return list->last_error = LIST_OK;
}

ListError ListVerefy(List* list) {
    assert(list != NULL);

    if (list->last_error != LIST_OK) {
        return list->last_error;
    }

    if (VectorVerefy(&list->data)!= VECTOR_OK) {
        return list->last_error = LIST_DATA_VECTOR_ERROR;
    }

    if (VectorVerefy(&list->next)!= VECTOR_OK) {
        return list->last_error = LIST_NEXT_VECTOR_ERROR;
    }

    if (VectorVerefy(&list->pref)!= VECTOR_OK) {
        return list->last_error = LIST_PREF_VECTOR_ERROR;
    }

    return list->last_error = LIST_OK;
}

ListError ListInsert(List* list, size_t id, list_elem_t elem) {
    assert(list != NULL);
    assert(id < list->data.size);

    LIST_CHECK(list);

    if (list->free == NO_FREE) {
        ListExpansion(list);
    }

    size_t next_free = 0;
    if (VectorGet(&list->next, list->free, &next_free) != VECTOR_OK) {
        return list->last_error = LIST_NEXT_VECTOR_ERROR;
    }

    size_t id_next = 0;
    if (VectorGet(&list->next, id, &id_next) != VECTOR_OK) {
        return list->last_error = LIST_NEXT_VECTOR_ERROR;
    }

    if (VectorSet(&list->data, list->free, &elem) != VECTOR_OK) {
        return list->last_error = LIST_DATA_VECTOR_ERROR;
    }

    if (VectorSet(&list->next, list->free, &id_next) != VECTOR_OK) {
        return list->last_error = LIST_NEXT_VECTOR_ERROR;
    }

    if (VectorSet(&list->pref, list->free, &id) != VECTOR_OK) {
        return list->last_error = LIST_PREF_VECTOR_ERROR;
    }

    if (VectorSet(&list->next, id, &list->free) != VECTOR_OK) {
        return list->last_error = LIST_NEXT_VECTOR_ERROR;
    }

    if (VectorSet(&list->pref, id_next, &list->free) != VECTOR_OK) {
        return list->last_error = LIST_PREF_VECTOR_ERROR;
    }

    list->free = next_free;

    LIST_CHECK(list);

    return list->last_error = LIST_OK;
}

ListError ListDelete(List* list, size_t id) {
    assert(list != NULL);
    assert(id < list->data.size);

    LIST_CHECK(list);

    if (id == 0) {
        return list->last_error = DELETE_FECTIVE_ELEM;
    }

    // FIXME function?
    size_t delete_next = 0;
    if (VectorGet(&list->next, id, &delete_next) != VECTOR_OK) {
        return list->last_error = LIST_NEXT_VECTOR_ERROR;
    }

    size_t delete_pref = 0;
    if (VectorGet(&list->pref, id, &delete_pref) != VECTOR_OK) {
        return list->last_error = LIST_PREF_VECTOR_ERROR;
    }

    if (VectorSet(&list->next, delete_pref, &delete_next) != VECTOR_OK) {
        return list->last_error = LIST_NEXT_VECTOR_ERROR;
    }

    if (VectorSet(&list->pref, delete_next, &delete_pref) != VECTOR_OK) {
        return list->last_error = LIST_PREF_VECTOR_ERROR;
    }

    if (VectorSet(&list->next, id, &list->free) != VECTOR_OK) {
        return list->last_error = LIST_NEXT_VECTOR_ERROR;
    }

    list->free = id;

    LIST_CHECK(list);

    return list->last_error = LIST_OK;
}