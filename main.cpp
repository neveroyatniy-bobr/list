#include "list.hpp"

#include <stdlib.h>

int main() {
    List list;
    
    if (ListInit(&list) != LIST_OK) {
        ListPrintError(&list, __FILE__, __LINE__);
        return 1;
    }

    for (int i = 0; i < 11; i++) {
        if (ListInsertAfter(&list, 0, i) != LIST_OK) {
            ListPrintError(&list, __FILE__, __LINE__);
            return 1;
        }
    }
    
    size_t fective_next = 0;
    VectorGet(&list.next, 5, &fective_next);
    for (size_t i = 0; i < 3; i++) {
       if (ListDeleteAt(&list, fective_next) != LIST_OK) {
            ListPrintError(&list, __FILE__, __LINE__);
            VectorDump(&list.prev, __FILE__, __LINE__);
            return 1;
        }
        VectorGet(&list.next, 5, &fective_next);
    }

    for (int i = 0; i < 5; i++) {
        if (ListInsertAfter(&list, 0, i) != LIST_OK) {
            ListPrintError(&list, __FILE__, __LINE__);
            return 1;
        }
    }

    ListGraphDump(&list, __FILE__, __LINE__);

    system("explorer.exe dump_file.html");

    if (ListDestroy(&list) != LIST_OK) {
        ListPrintError(&list, __FILE__, __LINE__);
        return 1;
    }

    return 0;
}