#ifndef VECTOR_H_
#define VECTOR_H_

#include <stdio.h>

enum VectorError {
    VECTOR_OK                  =  0,
    POP_ON_EMPTY_VECTOR        =  1,
    VECTOR_NULL_PTR            =  2,
    VECTOR_EXPANTION_ERR       =  3,
    VECTOR_CONTRACTION_ERR     =  4,
    VECTOR_INIT_ERR            =  5,
    VECTOR_DATA_NULL_PTR       =  6,
    VECTOR_OVERFLOW            =  7,
    VECTOR_POPED_ELEM_NULL_PTR =  8,
    VECTOR_BIRD_ERROR          =  9,
    VECTOR_HANDLER_NULL_PTR    = 10,
    VECTOR_HASH_ERROR          = 11
};

void VectorPrintError(VectorError error);

struct Vector;

typedef void (*VectorHandler)(Vector* vector, const char* file, size_t line);

struct Vector {
    size_t capacity;
    size_t size;
    size_t elem_size;
    void* data;
    size_t hash;
    VectorError last_error_code;
};

static const size_t VECTOR_MIN_CAPACITY = 16;
static const size_t VECTOR_GROW_FACTOR = 2;

#ifndef NOBIRD
static const size_t VECTOR_BIRD_SIZE = 1;
#else
static const size_t VECTOR_BIRD_SIZE = 0;
#endif

static const char BIRD_CHAR_VALUE = (char)222;

static const size_t NULL_VALUE = 0;

VectorError VectorInit(Vector** vector, size_t elem_capacity, size_t elem_size);

VectorError VectorExpantion(Vector* vector);

VectorError VectorContraction(Vector* vector);

VectorError VectorFree(Vector* vector);

VectorError VectorPush(Vector* vector, void* elem);

VectorError VectorPop(Vector* vector, void* poped_elem);

VectorError VectorGet(Vector* vector, size_t i, void* dest);

VectorError VectorSet(Vector* vector, size_t i, void* src);

VectorError VectorVerefy(Vector* vector);

void VectorDump(Vector* vector, const char* file, size_t line);

#define VECTOR_CHECK(vector)                         \
    vector->last_error_code = VectorVerefy(vector); \
    if (vector->last_error_code != VECTOR_OK) {     \
        return vector->last_error_code;             \
    }

#endif // VECTOR_H_