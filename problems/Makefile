# DO NOT CHANGE BELOW THIS LINE

# temporarily removed "cd onedknap && $(MAKE) $@ && cd ..&&\" due to linker issue

all dep clean indent tests::	
	cd optlib && $(MAKE) $@ && cd .. &&\
	cd cover && $(MAKE) $@ && cd .. &&\
	cd pareto && $(MAKE) $@ && cd .. &&\
	cd ssum && $(MAKE) $@ && cd .. &&\
	cd boxcon && $(MAKE) $@ && cd .. &&\
	cd nlp && $(MAKE) $@ && cd ..

