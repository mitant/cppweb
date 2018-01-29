FROM gcc:7.2

# These directories were picked because I sometimes develop on a Mac.
WORKDIR /usr/local/Cellar/openssl
RUN wget https://www.openssl.org/source/openssl-1.1.0g.tar.gz
RUN tar -xvf openssl-1.1.0g.tar.gz
WORKDIR /usr/local/Cellar/openssl/openssl-1.1.0g
RUN ./config --prefix=/usr/local --openssldir=/usr/local/Cellar/openssl/1.1.0g
RUN make
RUN make install

WORKDIR /tmp/boost
RUN wget https://dl.bintray.com/boostorg/release/1.66.0/source/boost_1_66_0.tar.bz2
RUN tar --bzip2 -xf /tmp/boost/boost_1_66_0.tar.bz2

WORKDIR /tmp/boost/boost_1_66_0
RUN ./bootstrap.sh
RUN ./b2 install --prefix=/usr/local/Cellar/boost/1.66.0 threading=multi link=static; exit 0

COPY src/web_service/Makefile /usr/src/web_service/
COPY src/web_service/*.cpp /usr/src/web_service/
COPY src/web_service/*.h /usr/src/web_service/
COPY src/web_service/*.hpp /usr/src/web_service/

WORKDIR /usr/src/web_service
RUN make
CMD PORT=80 THREADS=1 ./web_service