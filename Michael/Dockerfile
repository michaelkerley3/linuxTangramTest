FROM ubuntu:20.04
RUN apt update
RUN apt install -y make g++ build-essential autoconf libssl-dev libtool pkgconf
ENV LD_LIBRARY_PATH=/working/Michael/code-gen/build/libs:/working/Michael/code-gen/local_install/lib
WORKDIR /working
COPY code-gen /working/code-gen
RUN make -C code-gen
COPY Michael/src /working/src
COPY Michael/Makefile /working/
RUN make
CMD ["/working/michael"]

