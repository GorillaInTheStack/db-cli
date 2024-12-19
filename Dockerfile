FROM alpine:latest AS builder

RUN apk update && apk add --no-cache \
    build-base \
    cmocka-dev \
    git \
    linux-headers

WORKDIR /app
COPY . .

RUN make all

RUN make test

FROM alpine:latest AS runtime

WORKDIR /app

COPY --from=builder /app/bin/db_cli /app/bin/db_cli
COPY --from=builder /app/bin/db_server /app/bin/db_server
COPY --from=builder /app/bin/db_client /app/bin/db_client

EXPOSE 1337

RUN addgroup -S dbcli_grp && adduser -S dbcli_user -G dbcli_grp
RUN chown -R dbcli_user:dbcli_grp /app
RUN chmod u+x /app/bin/*
USER dbcli_user

CMD ["/app/bin/db_server"]
