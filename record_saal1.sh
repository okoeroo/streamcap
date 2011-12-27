#!/bin/sh

HTML_ROOT="/media/ccc/www/html"
REL_VIDEO_DIR="video"
ABS_VIDEO_DIR="${HTML_ROOT}/${REL_VIDEO_DIR}"

STREAMCAP="/home/okoeroo/dvl/clib/streamcap/streamcap"

function streamcapture() {
    URL="$1"
    NAME="$2"
    EXT="$3"
    WATERMARK="$4"

    while [ true ]; do
        ${STREAMCAP} -url ${URL} -name ${NAME} -extention ${EXT} -highwatermark ${WATERMARK}
    done
}


# saal1.theora.28c3.fem-net.de
# saal2.theora.28c3.fem-net.de
# saal3.theora.28c3.fem-net.de


cd "${ABS_VIDEO_DIR}"

#streamcapture "http://194.171.96.102:8000/room2" "room1" "ogv" "100" &

# saal 1
streamcapture "http://http.28c3.fem-net.de/live/saal1_hq-2011.12.27-11_54_26.ts" "saal1" "ogv" "400M"


