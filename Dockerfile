FROM ekidd/rust-musl-builder as build

COPY ./src/* ./src/
COPY ./Cargo.* ./

RUN ["ls", "-lAr"]

RUN ["cargo", "build", "--target", "x86_64-unknown-linux-musl", "--release"]

RUN ["strip", "target/x86_64-unknown-linux-musl/release/nesubst"]

FROM alpine

COPY --from=build /home/rust/src/target/x86_64-unknown-linux-musl/release/nesubst /usr/bin/

CMD ["nesubst"]
