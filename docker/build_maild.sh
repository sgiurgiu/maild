#!/bin/bash

pushd maild && rm -rf build/ && mkdir build && pushd build/ && \
cmake -GNinja -DTESTS_ENABLED=OFF -DCMAKE_BUILD_TYPE=Release -DCPACK_GENERATOR=RPM ../ && ninja package && \
mv maild-*.rpm maild.rpm && popd +1

