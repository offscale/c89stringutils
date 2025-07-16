FROM debian

RUN apt-get update -qq && \
    apt-get install -qy cmake gcc make pkg-config libc-dev && \
    rm -rf /var/lib/apt/lists/* /var/cache/apt

COPY . /c89stringutils

WORKDIR /c89stringutils

RUN cmake -DCMAKE_BUILD_TYPE="Debug" \
          -DBUILD_TESTING=1 \
          -DC89STRINGUTILS_BUILD_TESTING=1 \
          -S . -B build && \
    cmake --build build

CMD cd /c89stringutils/build && ctest .
