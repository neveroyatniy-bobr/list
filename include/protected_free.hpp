#ifndef PROTECTED_FREE_H_
#define PROTECTED_FREE_H_

#define PROTECTED_FREE(ptr) free(ptr); ptr = NULL;

#endif // PROTECTED_FREE_H_