#!/bin/sh


function streamcapture() {
    URL="$1"
    NAME="$2"
    EXT="$3"
    WATERMARK="$4"

    while [ true ]; do
        ./streamcap -url ${URL} -name ${NAME} -extention ${EXT} -highwatermark ${WATERMARK}
    done
}

#streamcapture "http://194.171.96.102:8000/room2" "room1" "ogv" "100" &
streamcapture "http://194.171.96.102:8000/room2" "room2" "ogv" "100"


