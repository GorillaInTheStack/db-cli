#include <stdbool.h>

#include "server_poll.h"
#include "context.h"
#include "file.h"
#include "parse.h"

StatusCode start_server(int argc, char *argv[])
{
    DBContext ctx = {0};
    StatusCode status = -1;
    char *server_db_file_name = "server_side.db";

    status = create_db_file(server_db_file_name, &ctx.db_fd);
    if (status == STATUS_OK)
    {
        status = create_db_header(&ctx);
        status = output_file(&ctx);
    }
    if (status == STATUS_FILE_EXISTS)
    {
        printf("File already exists\n");
        status = open_db_file(server_db_file_name, &ctx.db_fd);
        if (status == STATUS_OK)
        {
            status = validate_db_header(&ctx);
        }
        if (status == STATUS_OK)
        {
            status = read_employees(&ctx);
        }
    }

    status = poll_loop(&ctx);

    cleanup_context(&ctx);
    return status;
}
