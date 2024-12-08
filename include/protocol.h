

typedef enum {

    MSG_HELLO_REQ, MSG_HELLO_RESP

} dbproto_type_e;



typedef struct {

    dbproto_type_e type;

    unsigned int len;

} dbproto_hdr_t;

void handle_client(int fd);

void handle_server(int fd);

