#!/bin/sh
DEV="-d /dev/ttyUSB0 -b 115200"

CMD="M1 p7,0,0 t M1 07 R "
PASS="Test passed"
MAX_RUN_S=10

LOG="$0.log"

grabserial $DEV -c 'reset ' -q "M1:" -e 10
sleep 2

grabserial $DEV -c "$CMD" -q "$PASS" -e $MAX_RUN_S -o $LOG
echo

if [ `gawk -f seq_02.awk $LOG | grep -c PASS` -ne 1 ]
then
	grabserial $DEV -c 'r ' -q StepperDemo -e 1
	echo
	echo FAIL $0 pulse counter mismatch
	exit 1
fi

if [ `grep -c "$PASS" $LOG` -eq 1 ]
then
	echo PASS
else
	grabserial $DEV -c 'r ' -q StepperDemo -e 1
	echo
	echo "FAIL $0 result pattern: $PASS"
	exit 1
fi

