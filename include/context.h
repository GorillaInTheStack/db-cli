#ifndef CONTEXT_H
#define CONTEXT_H
#include <stdio.h>

#include "common.h"

typedef struct {
    int db_fd;                          
    DBHeader *header;          
    Employee *employees;    
} DBContext;

void cleanup_context(DBContext *ctx);
StatusCode validate_context(const DBContext *ctx, bool check_employees);

#endif
