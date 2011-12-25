#!/bin/bash


FFMPEG="ffmpeg"
RESOL="100x75"
INPUT="$1"

if [ -z "$INPUT" ]; then
    echo "Nothing to thumbnail"
    exit 1
fi


function thumbnailer() {
    if [ -z "$1" ]; then
        echo "Nothing to thumbnail"
        return 1
    fi
    if [ -z "$2" ]; then
        echo "No offset (in seconds) given"
        return 1
    fi
    INPUT="$1"
    OFFSET="$2"

    ${FFMPEG} \
        -i "${INPUT}" \
        -s ${RESOL} \
        -ss ${OFFSET} \
        -f image2 \
        thumb_${OFFSET}_${INPUT}.png
}


thumbnailer "${INPUT}" 5
thumbnailer "${INPUT}" 10
thumbnailer "${INPUT}" 15


