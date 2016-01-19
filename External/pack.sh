#!/bin/sh

##local path
TP="/usr/local/bin/TexturePacker"
PDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"/..

##skin list
declare -a sheet=("main" "dungeon1" "camp1" "gui_mission")


##engine--------------------------------------------------
##--------------------------------------------------------

if [ "$1" == "clean" ] || [ "${ACTION}" == "clean" ]; then
	echo "cleaning..."
	rm -fv ${PDIR}/Resources-iOS/sd/ss-*.*
	rm -fv ${PDIR}/Resources-iOS/hd/ss-*.*
	rm -fv ${PDIR}/Resources-android/sd/ss-*.*
	rm -fv ${PDIR}/Resources-android/hd/ss-*.*
else
	echo "building spritesheets..."
    for d in "${sheet[@]}"
    do
        ${TP} --smart-update \
              --max-size 1024 \
		      --trim-mode Trim \
		      --extrude 1 \
		      --format cocos2d \
		      --texture-format png \
		      --variant 1:hd \
		      --variant 0.5:sd \
		      --algorithm MaxRects \
		      --data ${PDIR}/Resources-iOS/{v}/ss-${d}.plist \
		      --sheet ${PDIR}/Resources-iOS/{v}/ss-${d}.png \
		      --opt RGBA8888 \
        ${PDIR}/External/assets/${d}/*

        ${TP} --smart-update \
              --max-size 1024 \
		      --trim-mode Trim \
		      --extrude 1 \
		      --format cocos2d \
		      --texture-format png \
		      --variant 1:hd \
		      --variant 0.5:sd \
		      --algorithm MaxRects \
		      --data ${PDIR}/Resources-android/{v}/ss-${d}.plist \
		      --sheet ${PDIR}/Resources-android/{v}/ss-${d}.png \
		      --opt RGBA8888 \
        ${PDIR}/External/assets/${d}/*
    done
    echo "transfering extra ressources..."
    cp -R ${PDIR}/External/extra/* ${PDIR}/Resources-iOS
    cp -R ${PDIR}/External/extra/* ${PDIR}/Resources-android
fi
exit 0