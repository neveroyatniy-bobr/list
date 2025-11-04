#ifndef LIST_H_
#define LIST_H_

#include <stdio.h>

#include "vector.hpp"

struct List;

typedef int list_elem_t;

enum ListError {
    LIST_OK                =  0,
    LIST_DATA_VECTOR_ERROR =  1,
    LIST_NEXT_VECTOR_ERROR =  2,
    LIST_PREV_VECTOR_ERROR =  3,
    DELETE_FECTIVE_ELEM    =  4,
    LIST_BAD_FECTIVE_ELEM  =  5,
    LIST_GRAPH_ERROR       =  6,
    LIST_BAD_ID            =  7,
    LIST_MEMORY_LEAK       =  8
};

const char* ListStrError(ListError error);

void ListPrintError(List* list, const char* file, int line);

struct List {
    Vector data; // <int>
    Vector next; // <size_t>
    Vector prev; // <size_t>
    size_t free;
    size_t size;
    ListError last_error;
};

static const list_elem_t FECTIVE_ELEM_VALUE = 1789657045;
static const size_t FECTIVE_ID = 0;

/// @brief ID никакого элемента
static const size_t NULL_ID = 21312326;

static const unsigned int DUMP_FONT_SIZE = 36;

ListError ListInit(List* list);

ListError ListDestroy(List* list);

ListError ListExpand(List* list);

ListError ListVerefy(List* list);

void ListGraphDump(List* list, const char* file, int line);

ListError ListInsertAfter(List* list, size_t id, list_elem_t elem);

ListError ListDeleteAt(List* list, size_t id);

#define LIST_CHECK(list)                    \
    {                                       \
        ListError err_ = ListVerefy(list);  \
        if (err_ != LIST_OK) {              \
            return err_;                    \
        }                                   \
    }

#endif // LIST_H_