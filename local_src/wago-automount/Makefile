# $(call bats,test-file,packageprefix)
bats = \
     mkdir -p $(dir $1) \
     &&    bats $(realpath $1) \
         | tee $(basename $1).tap \
         | tap-xunit --package='$(addprefix $2,$(notdir $(basename $1)))' > $(basename $1).xml \
         || (cat $(basename $1).tap && false)

.PHONY: all check clean

all: check

check:
	@$(call bats,$(wildcard *.bats))


clean:
	@rm -f *.tap *.xml
