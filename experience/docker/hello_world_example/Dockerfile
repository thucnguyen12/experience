FROM ubuntu:20.04
ARG DEBIAN_FRONTEND=noninteractive
WORKDIR ./hello_world
RUN apt update
RUN apt-get -y install cmake ninja-build
RUN apt-get -y update && apt-get -y install build-essential
COPY . ./hello_world

RUN cd ./hello_world &&\
    cmake -S ./ -B ./build &&\
    cmake --build ./build &&\
    echo "reach"
    # cd build &&\
    # make install &&\
    # touch test.txt

# CMD [""]
ENTRYPOINT ["sh"]