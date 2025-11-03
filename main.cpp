#include "list.hpp"

int main() {
    List list;
    
    if (ListInit(&list) != LIST_OK) {
        ListPrintError(&list, __FILE__, __LINE__);
        return 1;
    }

    for (int i = 0; i < 11; i++) {
        if (ListInsert(&list, 0, i) != LIST_OK) {
            ListPrintError(&list, __FILE__, __LINE__);
            return 1;
        }
    }
    
    size_t fective_next = 0;
    VectorGet(&list.next, 0, &fective_next);
    for (size_t i = 0; i < 5; i++) {
       if (ListDelete(&list, fective_next) != LIST_OK) {
            ListPrintError(&list, __FILE__, __LINE__);
            VectorDump(&list.prev, __FILE__, __LINE__);
            return 1;
        }
        VectorGet(&list.next, 0, &fective_next);
    }

    ListGraphDump(&list, __FILE__, __LINE__);

    if (ListDestroy(&list) != LIST_OK) {
        ListPrintError(&list, __FILE__, __LINE__);
        return 1;
    }

    return 0;
}