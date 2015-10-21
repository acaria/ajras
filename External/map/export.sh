#!/bin/sh

##local path
TILED="/Applications/Tiled.app/Contents/MacOS/Tiled"
PDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"/../..

usage="$(basename "$0") [-h] [clean|crush] -- export tiled maps tmx => json

where:
    -h     show this help text
    clean  clean generated json files
    crush  remove sRGB chung in png assets"

while true; do
  case "$1" in
    -h | --help ) echo "$usage"
                  exit
                  ;;
    -- ) shift; break ;;
    * ) break ;;
  esac
done

if [ "$1" == "clean" ] || [ "${ACTION}" == "clean" ]; then
	echo "cleaning..."
    rm -fv ${PDIR}/Resources/maps/*.json
elif [ "$1" == "crush" ] || [ "${ACTION}" == "crush" ]; then
	echo "crushing..."
    find -f ${PDIR}/External/assets png | while read line; do pngcrush -ow -brute $line; done
else
    echo "exporting maps"
    for file in ${PDIR}/External/map/*.tmx
    do
        ${TILED} --export-map "Json files (*.json)" "$file" "${PDIR}/Resources/maps/$(basename $file .tmx)."json""
    done
fi
exit 0