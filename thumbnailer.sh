#!/bin/bash

ffmpeg -y -i room.ogv -acodec libfaac -ab 128k -vcodec libx264 -vpre medium -b 800k -threads 8 output.mp4

ffmpeg -i output.mp4 2>&1 | grep Duration |cut -d' ' -f4 | cut -d: -f1

FFMPEG="ffmpeg"
RESOL="100x75"
INPUT="$1"

if [ -z "$INPUT" ]; then
    echo "Nothing to thumbnail"
    exit 1
fi


function makemp4() {
    if [ -z "$1" ]; then
        echo "No input to makemp4"
        return 1
    fi
    INPUT="$1"
    if [ -z "$2" ]; then
        echo "No output to makemp4"
        return 1
    fi
    OUTPUT="$2"

    ${FFMPEG} \
        -y \
        -i "${INPUT}" \
        -acodec libfaac \
        -ab 128k \
        -vcodec libx264 \
        -vpre medium \
        -b 800k -threads 4 \
        "${OUTPUT}"
}

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

    TMP_FILE="tmp_${INPUT}.mp4"
    makemp4 "${INPUT}" "${TMP_FILE}"

    ${FFMPEG} \
        -y \
        -i "${TMP_FILE}" \
        -s ${RESOL} \
        -ss ${OFFSET} \
        -f image2 \
        thumb_${OFFSET}_${INPUT}.png
}


thumbnailer "${INPUT}" 15
thumbnailer "${INPUT}" 30
thumbnailer "${INPUT}" 45
thumbnailer "${INPUT}" 60 
thumbnailer "${INPUT}" 120 


