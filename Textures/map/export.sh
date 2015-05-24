#!/bin/sh

##local path
TILED="/Applications/Tiled.app/Contents/MacOS/Tiled"
PDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"/../..

if [ "$1" == "clean" ] || [ "${ACTION}" == "clean" ]; then
	echo "cleaning..."
    rm -fv ${PDIR}/Resources/maps/*.json
else
    echo "exporting maps"
    for file in ${PDIR}/Textures/map/*.tmx
    do
        ${TILED} --export-map "Json files (*.json)" "$file" "${PDIR}/Resources/maps/$(basename $file .tmx)."json""
    done
fi
exit 0