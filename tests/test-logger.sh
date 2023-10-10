#!/bin/bash

usage=$(cat <<EOF
$0 - SELKIELogger Test Script

Usage: $0 /path/to/Logger
EOF
)

TD=$(mktemp -d)
if [[ -z "${TD}" || ! -d ${TD} ]]; then
	echo "Unable to create temporary directory"
	exit 99
fi

cat > ${TD}/config.ini <<EOF
verbose = 3
frequency = 50
prefix = "${TD}/SLx-"
rotate = false
savestate = false

[TICK]
type=timer
frequency = 10
EOF

if [ $# -lt 1 ]; then
	echo "${usage}"
	exit 1
fi

logger=$1
config=${TD}/config.ini


echo Launching Logger instance
${logger} ${config}  > /dev/null 2>&1 &
lpid=$!

echo "Logger launched as PID ${lpid}"
echo "Waiting 20 seconds (Startup and run)"
sleep 20
echo "Starting signal testing"
echo "Each signal will be followed by a five second delay"
echo "Sending SIGHUP (Log rotate)...."
kill -HUP $lpid
sleep 5
echo "Sending RTMIN+3 (Pause logging)...."
kill -SIGRTMIN+3 $lpid
sleep 5
echo "Sending RTMIN+4 (Unpause logging)...."
sleep 5
echo "Sending SIGQUIT..."
kill -QUIT $lpid
sleep 5
if kill -n 0 $lpid; then
	echo "Logger still running after shutdown signal"
	echo "Sending SIGTERM..."
	kill -TERM $lpid
	sleep 5
fi
if kill -n 0 $lpid ; then
	echo "Logger still running after SIGTERM"
	echo "Sending SIGKILL...."
	kill -KILL $lpid
fi
wait -f ${lpid}
lstat=$?
cat ${TD}/*.log
rm -rf ${TD}
echo "Done - logger exit process recorded as ${lstat}."
