#!/bin/bash

TITLE="28c3 stream capture dumper index"
HEAD_PAGE="<html><head><title>${TITLE}</title></head>"
TAIL_PAGE="</html>"

HTML_ROOT="/media/ccc/www/html"
REL_VIDEO_DIR="video"
ABS_VIDEO_DIR="${HTML_ROOT}/${REL_VIDEO_DIR}"

BODY=
PAGE=
BODY_DUMP=
BODY_DUMP_ROW=
BODY_DUMP_ROWS=

function build_dumps() {
    BODY_DUMP_TOP="<table>"
    BODY_DUMP_ROW=""
    BODY_DUMP_ROWS=""
    BODY_DUMP_BOTTOM="</table>"


    cd "${ABS_VIDEO_DIR}"
    BODY_DUMP_ROWS=$(find . -name \*\.ogv -type f | while read OGV_FILE; do
        BODY_DUMP_ROW="<tr>"
        BODY_DUMP_ROW="${BODY_DUMP_ROW}<td>"
        BODY_DUMP_ROW="${BODY_DUMP_ROW}<a href=\"${REL_VIDEO_DIR}/$OGV_FILE\">$OGV_FILE</a>"
        BODY_DUMP_ROW="${BODY_DUMP_ROW}</td>"
        BODY_DUMP_ROW="${BODY_DUMP_ROW}</tr>"

        echo "${BODY_DUMP_ROW}"
    done)

    BODY_DUMP=${BODY_DUMP_TOP}${BODY_DUMP_ROWS}${BODY_DUMP_BOTTOM}
}

function build_body() {
    BODY_CONTENT=

    BODY_TOP="<center><h1>28c3 stream capture dumper</h1></center><br>"
    BODY_TOP="${BODY_TOP}<center>This page is NOT an official mirror</center><br>"
    BODY_TOP="${BODY_TOP}<center>Do NOT expect any quality at all, it's a best effort thingy.</center><br>"

    BODY_TAIL="<br><center>Last update: `date`.<br>For comments, tweet me up: <a href=\"https://twitter.com/#!/okoeroo\">@okoeroo</a></center>"

    BODY_CONTENT=${BODY_TOP}${BODY_DUMP}${BODY_TAIL}



    BODY="<body>${BODY_CONTENT}</body>"
}


function build_page() {

    PAGE="${HEAD_PAGE}${BODY}${TAIL_PAGE}"
}


build_dumps
build_body
build_page

# Write it
echo $PAGE > ${HTML_ROOT}/index.html
