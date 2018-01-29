#!/bin/bash

docker build -t cppweb .
docker run --rm -it -p80:80 --name cppweb cppweb
