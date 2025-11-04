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

static const char DUMP_FILE_NAME[] = "dump_file.html";
static const char BUILD_DUMP_FILE_NAME[] = "build_dump_file.dot";

ListError ListInit(List* list);

ListError ListDestroy(List* list);

ListError ListExpand(List* list);

ListError ListVerefy(List* list);

void ListGraphDump(List* list, const char* file, int line);

ListError ListInsertAfter(List* list, size_t id, list_elem_t elem);

ListError ListDeleteAt(List* list, size_t id);

ListError ListGetNext(List* list, size_t i, size_t* next);

ListError ListGetPrev(List* list, size_t i, size_t* prev);

ListError ListGetData(List* list, size_t i, list_elem_t* data);

ListError ListSetData(List* list, size_t i, const list_elem_t* data);

ListError ListGetFront(List* list, size_t i, size_t* front_id);

ListError ListGetBack(List* list, size_t i, size_t* back_id);

ListError ListInsertFront(List* list, list_elem_t elem);

ListError ListInsertBack(List* list, list_elem_t elem);

ListError ListDeleteFront(List* list);

ListError ListDeleteBsck(List* list);

size_t ListSize(List* list);

bool ListIsEmpty(List* list);

#define LIST_CHECK(list)                    \
    {                                       \
        ListError err_ = ListVerefy(list);  \
        if (err_ != LIST_OK) {              \
            return err_;                    \
        }                                   \
    }

#endif // LIST_H_