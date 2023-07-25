
FROM ubuntu
ARG DEBIAN_FRONTEND=noninteractive
WORKDIR ./01.ESP32
RUN apt update
RUN apt-get install -y git wget flex bison gperf python3 python3-venv cmake ninja-build ccache libffi-dev libssl-dev dfu-util libusb-1.0-0
RUN cd .. &&\
    mkdir esp && \
    cd esp &&\
    git clone -b v5.0.3 --recursive https://github.com/espressif/esp-idf.git

#COPY . .
#ADD https://github.com/espressif/esp-idf.git ./
RUN cd /esp/esp-idf &&\
    ./install.sh esp32
RUN cd /esp/esp-idf &&\
    . ./export.sh

CMD ["sh"]