FROM alpine:latest

ARG BOOST_VERSION=1.81.0

RUN apk add --no-cache -q -f valgrind g++ gcc cmake make ninja git gdb gcov wget tar boost-dev-1.81.0-r1 perf

RUN git clone -q https://github.com/google/googletest.git /googletest \
  && mkdir -p /googletest/build \
  && cd /googletest/build \
  && cmake .. && make && make install \
  && cd /  \
  && rm -rf /googletest

