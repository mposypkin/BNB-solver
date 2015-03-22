# DO NOT CHANGE BELOW THIS LINE

JSONLIB = libjson
UTIL = util
KERNEL = kernel
PROBLEMS = problems

all dep clean indent tests::
	cd $(JSONLIB) && $(MAKE) $@ && cd .. &&\
	cd $(UTIL) && $(MAKE) $@ && cd .. &&\
	cd $(KERNEL) && $(MAKE) $@ && cd .. &&\
	cd $(PROBLEMS) && $(MAKE) $@ && cd ..

doc: indent doxy

doxy:
	mkdir -p doc/html &&\
	doxygen doxy.conf

clean::
	rm -rf *~ PI* core bin/* obj/* tmp *.log

tar: clean
	echo Tarring... &&\
	cd .. &&\
	tar czvf bnb-solver.`date +%Y.%m.%d.%H.%M.tgz` bnb-solver














