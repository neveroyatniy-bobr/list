#include "list.hpp"

int main() {
    List list;
    
    if (ListInit(&list) != LIST_OK) {
        ListPrintError(&list, __FILE__, __LINE__);
        return 1;
    }

    for (int i = 0; i < 32; i++) {
        if (ListInsert(&list, 0, i) != LIST_OK) {
            ListPrintError(&list, __FILE__, __LINE__);
            return 1;
        }
    }

    {
        size_t i = 0;
        size_t next_i = 0;
        do {
            i = next_i;
            int data = 0;
            VectorGet(&list.data, i, &data);
            printf("%d\n", data);
            
            VectorGet(&list.next, i, &next_i);
        }  while (next_i != 0);
    }
    
    size_t fective_next = 0;
    VectorGet(&list.next, 0, &fective_next);
    for (size_t i = 0; i < 17; i++) {
       if (ListDelete(&list, fective_next) != LIST_OK) {
            ListPrintError(&list, __FILE__, __LINE__);
            VectorDump(&list.prev, __FILE__, __LINE__);
            return 1;
        }
        VectorGet(&list.next, 0, &fective_next);
    }


    printf("\n\n");
    {
        size_t i = 0;
        size_t next_i = 0;
        do {
            i = next_i;
            int data = 0;
            VectorGet(&list.data, i, &data);
            printf("%d\n", data);
            
            VectorGet(&list.next, i, &next_i);
        }  while (next_i != 0);
    }

    if (ListDestroy(&list) != LIST_OK) {
        ListPrintError(&list, __FILE__, __LINE__);
        return 1;
    }

    return 0;
}