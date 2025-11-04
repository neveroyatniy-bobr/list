#include "list.hpp"

#include <assert.h>
#include <stdlib.h>

#include "vector.hpp"
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
    case LIST_BAD_FECTIVE_ELEM:
        return "Нверный фективный элемент\n";
        break;
    case LIST_GRAPH_ERROR:
        return "Ошибка в связях графа листа\n";
        break;
    case LIST_BAD_ID:
        return "Неправильный id элемента\n";
        break;
    case LIST_MEMORY_LEAK:
        return "Потеря памяти в листе\n";
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
        VectorFree(&list->data);
        return list->last_error = LIST_NEXT_VECTOR_ERROR;
    }
    
    if (VectorInit(&list->prev, 0, sizeof(size_t)) != VECTOR_OK) {
        VectorFree(&list->data);
        VectorFree(&list->next);
        return list->last_error = LIST_PREV_VECTOR_ERROR;
    }

    if (VectorPush(&list->data, &FECTIVE_ELEM_VALUE) != VECTOR_OK) {
        ListDestroy(list);
        return list->last_error = LIST_DATA_VECTOR_ERROR;
    }

    if (VectorPush(&list->next, &FECTIVE_ID) != VECTOR_OK) {
        ListDestroy(list);
        return list->last_error = LIST_NEXT_VECTOR_ERROR;
    }

    if (VectorPush(&list->prev, &FECTIVE_ID) != VECTOR_OK) {
        ListDestroy(list);
        return list->last_error = LIST_PREV_VECTOR_ERROR;
    }

    list->free = NULL_PTR;

    list->size = 0;

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

ListError ListExpand(List* list) {
    int pushed_elem = 0;
    if (VectorPush(&list->data, &pushed_elem) != VECTOR_OK) {
        return list->last_error = LIST_DATA_VECTOR_ERROR;
    }

    if (VectorPush(&list->next, &NULL_PTR) != VECTOR_OK) {
        return list->last_error = LIST_NEXT_VECTOR_ERROR;
    }

    if (VectorPush(&list->prev, &NULL_PTR) != VECTOR_OK) {
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

    int fective_value = 0;
    if (VectorGet(&list->data, 0, &fective_value) != VECTOR_OK) {
        return list->last_error = LIST_DATA_VECTOR_ERROR;
    }
    if (fective_value != FECTIVE_ELEM_VALUE) {
        return list->last_error = LIST_BAD_FECTIVE_ELEM;
    }

    size_t curent_id = 0;
    size_t next_id   = 0;
    for (size_t elem_i = 0; elem_i < list->size + 1; elem_i++) {
        if (VectorGet(&list->next, curent_id, &next_id)) {
            return list->last_error = LIST_NEXT_VECTOR_ERROR;
        }
        curent_id = next_id;

        if (curent_id >= list->data.size) {
            return list->last_error = LIST_BAD_ID;
        }
    }
    if (curent_id != 0) {
        return list->last_error = LIST_GRAPH_ERROR;
    }

    size_t free_cnt = 0;
    size_t curent_free_id = list->free;
    size_t next_free_id   = 0;
    while (curent_free_id != NULL_PTR) {
        free_cnt++;

        if (curent_free_id >= list->data.size) {
            return list->last_error = LIST_BAD_ID;
        }

        if (VectorGet(&list->next, curent_free_id, &next_free_id)) {
            return list->last_error = LIST_NEXT_VECTOR_ERROR;
        }
        curent_free_id = next_free_id;
    }
    if (list->size + 1 + free_cnt != list->data.size) {
        return list->last_error = LIST_MEMORY_LEAK;
    }

    return list->last_error = LIST_OK;
}

void ListGraphDump(List* list, const char* file, int line) {
    FILE* build_dump_file = fopen("build_dump_file.dot", "w");

    if (build_dump_file == NULL) {
        fprintf(stderr, "Ошибка в создании файла дампа\n");
        return;
    }

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

        if (next != NULL_PTR && next <= list->data.size) {
            fprintf(build_dump_file, "    %lu -> %lu [label = \"next\"];\n", elem_i, next);
        }

        if (prev != NULL_PTR && prev <= list->data.size) {
            fprintf(build_dump_file, "    %lu -> %lu [label = \"prev\"];\n", elem_i, prev);
        }
    }

    fprintf(build_dump_file, "\n    free -> %lu;\n", list->free);

    fprintf(build_dump_file, "    free [label=\"FREE\"];\n");

    fprintf(build_dump_file, "}");

    fclose(build_dump_file);

    system("dot -Tsvg build_dump_file.dot -o dump_file.html");

    FILE* dump_file = fopen("dump_file.html", "a");

    if (dump_file == NULL) {
        fprintf(stderr, "Ошибка в создании файла дампа\n");
        return;
    }

    fprintf(dump_file, "<table border=\"1\" style=\"font-size: %u; width: 50%%;\">\n", DUMP_FONT_SIZE);

    fprintf(dump_file, "    <tr>\n");
    fprintf(dump_file, "        <th>id</th>\n");
    fprintf(dump_file, "        <th>value</th>\n");
    fprintf(dump_file, "        <th>next</th>\n");
    fprintf(dump_file, "        <th>prev</th>\n");
    fprintf(dump_file, "    <tr>\n");

    for (size_t elem_i = 0; elem_i < list->data.size; elem_i++) {
        int value = 0;
        VectorGet(&list->data, elem_i, &value);

        size_t next = 0;
        VectorGet(&list->next, elem_i, &next);

        size_t prev = 0;
        VectorGet(&list->prev, elem_i, &prev);

        fprintf(dump_file, "    <tr>\n");

        fprintf(dump_file, "        <th>%lu</th>\n", elem_i);

        if (value == FECTIVE_ELEM_VALUE) {
            fprintf(dump_file, "        <th>fective elem</th>\n");
        }
        else {
            fprintf(dump_file, "        <th>%d</th>\n", value);
        }

        if (next == NULL_PTR) {
            fprintf(dump_file, "        <th>null ptr</th>\n");
        }
        else {
            fprintf(dump_file, "        <th>%lu</th>\n", next);
        }

        if (prev == NULL_PTR) {
            fprintf(dump_file, "        <th>null ptr</th>\n");
        }
        else {
            fprintf(dump_file, "        <th>%lu</th>\n", prev);
        }

        fprintf(dump_file, "    <tr>\n");
    }

    fprintf(dump_file, "</table>\n");

    fprintf(dump_file, "\n<p style=\"font-size: %upx;\">\n    size = %lu\n</p>\n", DUMP_FONT_SIZE, list->size);

    fprintf(dump_file, "<p style=\"font-size: %upx;\">\n    ERROR in %s:%d: %s</p>", DUMP_FONT_SIZE, file, line, ListStrError(list->last_error));

    fclose(dump_file);
}

ListError ListInsertAfter(List* list, size_t id, list_elem_t elem) {
    assert(list != NULL);
    assert(id < list->data.size);

    LIST_CHECK(list);

    if (list->free == NULL_PTR) {
        ListExpand(list);
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

    list->size++;

    LIST_CHECK(list);

    return list->last_error = LIST_OK;
}

ListError ListDeleteAt(List* list, size_t id) {
    assert(list != NULL);
    assert(id < list->data.size);

    LIST_CHECK(list);

    if (id == 0) {
        return list->last_error = DELETE_FECTIVE_ELEM;
    }

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

    if (VectorSet(&list->prev, id, &NULL_PTR) != VECTOR_OK) {
        return list->last_error = LIST_NEXT_VECTOR_ERROR;
    }

    list->free = id;

    list->size--;

    LIST_CHECK(list);

    return list->last_error = LIST_OK;
}