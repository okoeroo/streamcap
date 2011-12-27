#!/bin/sh

HTML_ROOT="/media/ccc/www/html"
REL_VIDEO_DIR="video"
ABS_VIDEO_DIR="${HTML_ROOT}/${REL_VIDEO_DIR}"

STREAMCAP="/home/okoeroo/dvl/clib/streamcap/streamcap"

NAME="saal3"
MAINURL="http://http.28c3.fem-net.de/live/${NAME}_hq.m3u8"
EXT="ogv"
WATERMARK="400M"


function streamcapture() {
    URL="$1"
    NAME="$1"
    EXT="$2"
    WATERMARK="$3"

    while [ true ]; do
        rand_num=$RANDOM
        count=0
        let "rand_num %= 8"

        echo $rand_num

        URL=`curl ${MAINURL} | grep -E "^http" | awk "NR==${rand_num}{print \$1}"`

        echo "Downloading from: $URL"

        ${STREAMCAP} -url ${URL} -name ${NAME} -extention ${EXT} -highwatermark ${WATERMARK}
    done
}

# main
streamcapture


