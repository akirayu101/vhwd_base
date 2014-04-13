
all: release

vhwd: vhwd_realse

test: test_realse

vhwd_debug:
	cd build/linux && make debug -f Makefile.vhwd

vhwd_realse:
	cd build/linux && make release -f Makefile.vhwd

test_debug:
	cd build/linux && make debug -f Makefile.test

test_realse:
	cd build/linux && make release -f Makefile.test

clean: clean_debug clean_release


debug:
	cd build/linux && make debug -f Makefile.vhwd && make debug -f Makefile.test

release:
	cd build/linux && make release -f Makefile.vhwd && make release -f Makefile.test

clean_debug:
	cd build/linux && make clean_debug -f Makefile.vhwd && make clean_debug -f Makefile.test

clean_release:
	cd build/linux && make clean_release -f Makefile.vhwd && make clean_release -f Makefile.test
