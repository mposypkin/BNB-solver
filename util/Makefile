# DO NOT CHANGE BELOW THIS LINE

ACLIB = aclib
COMMON=common
LINAL = linal
XML = xml
MPLIB = mplib
INEQ = ineq
BOX = box
SPACEFILL = spacefill
BOXBNB = boxbnb
TREE = tree

all dep clean indent tests::
	cd $(ACLIB) && $(MAKE) $@ && cd .. &&\
	cd $(COMMON) && $(MAKE) $@ && cd .. &&\
	cd $(LINAL) && $(MAKE) $@ && cd .. &&\
	cd $(XML) && $(MAKE) $@ && cd ..&&\
	cd $(MPLIB) && $(MAKE) $@ && cd ..&&\
	cd $(INEQ) && $(MAKE) $@ && cd .. &&\
	cd $(BOX) && $(MAKE) $@ && cd .. &&\
	cd $(SPACEFILL) && $(MAKE) $@ && cd .. &&\
	cd $(BOXBNB) && $(MAKE) $@ && cd .. &&\
	cd $(TREE) && $(MAKE) $@ && cd ..
