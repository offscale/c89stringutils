FROM alpine

RUN apk add --no-cache gcc cmake make musl-dev linux-headers

COPY . /c89stringutils

WORKDIR /c89stringutils

RUN cmake -DCMAKE_BUILD_TYPE="Debug" \
          -DBUILD_TESTING=1 \
          -DC89STRINGUTILS_BUILD_TESTING=1 \
          -S . -B build && \
    cmake --build build

CMD cd build && ctest .
