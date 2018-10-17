FROM alpine as build

COPY ./src/main.c /workspace/

WORKDIR /workspace

RUN ["apk", "add", "build-base"]

RUN ["gcc", "-o", "nesubst", "-O3", "-static", "main.c"]

RUN ["strip", "nesubst"]

FROM alpine

COPY --from=build /workspace/nesubst /usr/bin/

CMD ["nesubst"]
