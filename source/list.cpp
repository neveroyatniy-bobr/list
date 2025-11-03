#include "list.hpp"

#include <assert.h>
#include <stdlib.h>

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
    case LIST_PREV_VECTOR_ERROR:
        return "Ошибка в векторе prev\n";
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
    
    if (VectorInit(&list->prev, 0, sizeof(size_t)) != VECTOR_OK) {
        return list->last_error = LIST_PREV_VECTOR_ERROR;
    }

    if (VectorPush(&list->data, (void*)const_cast<list_elem_t*>(&FECTIVE_ELEM_VALUE)) != VECTOR_OK) {
        return LIST_DATA_VECTOR_ERROR;
    }

    size_t fective_id = 0;

    if (VectorPush(&list->next, &fective_id) != VECTOR_OK) {
        return LIST_NEXT_VECTOR_ERROR;
    }

    if (VectorPush(&list->prev, &fective_id) != VECTOR_OK) {
        return LIST_PREV_VECTOR_ERROR;
    }

    list->free = NULL_PTR;

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

    if (VectorFree(&list->prev) != VECTOR_OK) {
        return list->last_error = LIST_PREV_VECTOR_ERROR;
    }

    return list->last_error = LIST_OK;
}

ListError ListExpansion(List* list) {
    int pushed_elem = 0;
    if (VectorPush(&list->data, &pushed_elem) != VECTOR_OK) {
        return list->last_error = LIST_DATA_VECTOR_ERROR;
    }

    if (VectorPush(&list->next, (void*)const_cast<size_t*>(&NULL_PTR)) != VECTOR_OK) {
        return list->last_error = LIST_NEXT_VECTOR_ERROR;
    }

    if (VectorPush(&list->prev, (void*)const_cast<size_t*>(&NULL_PTR)) != VECTOR_OK) {
        return list->last_error = LIST_PREV_VECTOR_ERROR;
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

    if (VectorVerefy(&list->prev)!= VECTOR_OK) {
        return list->last_error = LIST_PREV_VECTOR_ERROR;
    }

    return list->last_error = LIST_OK;
}

void ListGraphDump(List* list) {
    FILE* build_dump_file = fopen("build_dump_file.dot", "w");

    fprintf(build_dump_file, "digraph G {\n    rankdir=LR;\n    node [shape=record];\n\n");

    for (size_t elem_i = 0; elem_i < list->data.size; elem_i++) {
        int value = 0;
        VectorGet(&list->data, elem_i, &value);

        size_t next = 0;
        VectorGet(&list->next, elem_i, &next);

        size_t prev = 0;
        VectorGet(&list->prev, elem_i, &prev);

        fprintf(build_dump_file, "    %lu [label=\"value = %d\\nid = %lu\\nnext = %lu\\nprev = %lu\"];\n", elem_i, value, elem_i, next, prev);
    }

    fprintf(build_dump_file, "\n");

    for (size_t elem_i = 0; elem_i < list->data.size; elem_i++) {
        size_t next = 0;
        VectorGet(&list->next, elem_i, &next);

        size_t prev = 0;
        VectorGet(&list->prev, elem_i, &prev);

        if (next != NULL_PTR) {
            fprintf(build_dump_file, "    %lu -> %lu [label = \"next\"];\n", elem_i, next);
        }

        if (prev != NULL_PTR) {
            fprintf(build_dump_file, "    %lu -> %lu [label = \"prev\"];\n", elem_i, prev);
        }
    }

    fprintf(build_dump_file, "\n    free -> %lu;\n", list->free);

    fprintf(build_dump_file, "    free [label=\"FREE\"];\n");

    fprintf(build_dump_file, "}");

    fclose(build_dump_file);

    system("dot -Tpng build_dump_file.dot -o dump_file.png");

    system("explorer.exe dump_file.png");
}

ListError ListInsert(List* list, size_t id, list_elem_t elem) {
    assert(list != NULL);
    assert(id < list->data.size);

    LIST_CHECK(list);

    if (list->free == NULL_PTR) {
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

    if (VectorSet(&list->prev, list->free, &id) != VECTOR_OK) {
        return list->last_error = LIST_PREV_VECTOR_ERROR;
    }

    if (VectorSet(&list->next, id, &list->free) != VECTOR_OK) {
        return list->last_error = LIST_NEXT_VECTOR_ERROR;
    }

    if (VectorSet(&list->prev, id_next, &list->free) != VECTOR_OK) {
        return list->last_error = LIST_PREV_VECTOR_ERROR;
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

    size_t delete_prev = 0;
    if (VectorGet(&list->prev, id, &delete_prev) != VECTOR_OK) {
        return list->last_error = LIST_PREV_VECTOR_ERROR;
    }

    if (VectorSet(&list->next, delete_prev, &delete_next) != VECTOR_OK) {
        return list->last_error = LIST_NEXT_VECTOR_ERROR;
    }

    if (VectorSet(&list->prev, delete_next, &delete_prev) != VECTOR_OK) {
        return list->last_error = LIST_PREV_VECTOR_ERROR;
    }

    if (VectorSet(&list->next, id, &list->free) != VECTOR_OK) {
        return list->last_error = LIST_NEXT_VECTOR_ERROR;
    }

    if (VectorSet(&list->prev, id, (void*)const_cast<size_t*>(&NULL_PTR)) != VECTOR_OK) {
        return list->last_error = LIST_NEXT_VECTOR_ERROR;
    }

    list->free = id;

    LIST_CHECK(list);

    return list->last_error = LIST_OK;
}