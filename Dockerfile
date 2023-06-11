# Kaynak kodlarını "builder" içerisinde derleyip, "runner" içerisinde çalıştırıyoruz.
# Böylece imajımızın boyutunu küçültmüş oluyoruz ve gereksiz dosyaları imajımıza dahil etmemiş oluyoruz.
# Ayrıca imajımızın güvenliğini de arttırmış oluyoruz çünkü kaynak kodları imajımızda bulundurmuyoruz.
FROM alpine:latest as builder

RUN apk add --no-cache alpine-sdk clang \
    libstdc++ \
    libstdc++-dev \
    cmake

WORKDIR /app/include

RUN wget https://github.com/jbeder/yaml-cpp/archive/refs/tags/yaml-cpp-0.6.3.tar.gz && \
    tar -xzf yaml-cpp-0.6.3.tar.gz && \
    cd yaml-cpp-yaml-cpp-0.6.3 && \
    mkdir build && \
    cd build && \
    cmake .. && \
    make && \
    make install

WORKDIR /app

COPY Makefile ./Makefile

COPY ./src/ ./src/

RUN [ "make", "-d" ]
# CMD ["/bin/sh"]
# runner imajımızda sadece derlenmiş olan dosyaları bulunduruyoruz ve çalıştırıyoruz.
FROM alpine:latest as runner

RUN apk add --no-cache \
    libstdc++ 

WORKDIR /app/bin

COPY --from=builder /app/bin/ ./

ENTRYPOINT [ "./app" ]
# CMD ["./app"]
# CMD [ "ls", "-la" ]
# CMD ["/bin/sh"]