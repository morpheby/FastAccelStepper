ifndef SILENCE
	SILENCE=0
endif

PRJ_ROOT=$(shell git rev-parse --show-toplevel)
TESTS=$(wildcard test_*)
TESTS_SD=$(wildcard test_*sd_*)

TEST_FILES=$(addsuffix /.tested,$(TESTS))
SD_SRC_DIRS=$(addsuffix /src/.dir,$(TESTS_SD))

SRC=$(wildcard ../../src/*)

test: $(TEST_FILES) pmf externalCall 

pmf:
	make -C test_pmf

externalCall:
	make -C test_externalCall

%/.tested:	$(SRC) run_avr %/expect.txt %/platformio.ini .makefiles .links
	make SILENCE=$(SILENCE)  -C $(dir $@)

.makefiles: makefiles

makefiles: $(addsuffix /Makefile,$(TESTS))
	touch .makefiles

%/Makefile:
	cd $(dir $@); ln -s ../Makefile.test Makefile

.links: links

links: $(SD_SRC_DIRS)
	touch .links

%/src/.dir:
	mkdir -p $(dir $@)
	cd $(dir $@); ln -sf $(PRJ_ROOT)/examples/StepperDemo/* .

run_avr: simavr/simavr/run_avr
	ln -s simavr/simavr/run_avr .

simavr/simavr/run_avr: simavr/simavr/sim/run_avr.c
	make -C simavr build-simavr

simavr/simavr/sim/run_avr.c:
	# git clone https://github.com/gin66/simavr.git
	git clone https://github.com/buserror/simavr.git
	(cd simavr;git checkout	132cc67)

proper: clean
	rm -f run_avr
	rm -fR simavr

clean:
	rm -fR */.pio */.tested */x.vcd */result.txt
	find . -type l -delete
	find . -type d -empty -delete
	rm -f .links .makefiles

