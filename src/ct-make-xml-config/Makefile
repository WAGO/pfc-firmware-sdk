# ===========================================================================
# Makefile for the various kconfig executables:
#
# Adapted by andrej.gantvorg@wago.com from ptxdist's kconf Makefile
#
# conf:	  Used for defconfig, oldconfig and related targets
# [--------------8<--------------]
#            [skipped]
# object files used by all kconfig flavours

#CPPFLAGS_PRINT_UNUSED = -ffunction-sections -fdata-sections -Wunused
#LDFLAGS_PRINT_UNUSED = -Wl,--gc-sections,--print-gc-sections

CFLAGS += $(EXTRA_CFLAGS)

LDFLAGS += $(LDFLAGS_PRINT_UNUSED) $(EXTRA_LDFLAGS)

CPPFLAGS += \
	-DCURSES_LOC="$(CURSES_LOC)" \
	-DKBUILD_NO_NLS \
	-DPACKAGE='"ptxdist"' \
	-DCONFIG_='"PTXCONF_"' $(EXTRA_CPPFLAGS)
conf-libs  := $(CONF_LIBS)

lkc-deps := lkc.h lkc_defs.h expr.h

make-xml-config-objs := \
  make-xml-config.o \
	zconf.tab.o conf.o

make-xml-config: $(make-xml-config-objs) xsd_defs.py
	$(CC) $(make-xml-config-objs) $(LDFLAGS) -o $@ $(conf-libs)


%.moc: %.h
	$(MOC) -i $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

%.o: %.cc
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

%.moc: %.h
	$(KC_QT_MOC) -i $< -o $@

lkc_defs.h: lkc_proto.h
	sed < $< > $@ 's/P(\([^,]*\),.*/#define \1 (\*\1_p)/'

zconf.tab.o: lex.zconf.c zconf.hash.c zconf.tab.c confdata.c expr.c symbol.c menu.c $(lkc-deps)

kconfig_load.o: $(lkc-deps)

# python xsd representation

xsd_defs.py: xsd/menu.xsd
  # parameters: -f: force overwrite; -q: quiet; --no-process-includes: ignore included xsd files
	$(PYTHON) generateDS/generateDS.py  -f -q --no-process-includes -o $@ $<

# parser

zconf.tab.c: zconf.y
lex.zconf.c: zconf.l
zconf.hash.c: zconf.gperf

# These targets are only needed if you extend the lexer/parser.
# Otherwise, the shipped files are used.
#%.tab.c: %.y
#	bison -l -b $* -p $(notdir $*) $<
#
#lex.%.c: %.l
#	flex -L -P$(notdir $*) -o$@ $<
#
#%.hash.c: %.gperf
#	gperf < $< > $@
#

%.tab.c: %.y
	cp $@_shipped $@

lex.%.c: %.l
	cp $@_shipped $@

%.hash.c: %.gperf
	cp $@_shipped $@

INSTALLDIR:=$(DESTDIR)/bin/make-xml-config
install:
	mkdir -p $(INSTALLDIR)
	install -t $(INSTALLDIR) make-xml-config 
	install -t $(INSTALLDIR) python_c_glue.py
	install -t $(INSTALLDIR) xsd_defs.py
	install -t $(INSTALLDIR) libptxdist+xml.sh
	install -t $(INSTALLDIR) ptxd_lib_kgen.sh
	mkdir -p $(INSTALLDIR)/generateDS	
	install -t $(INSTALLDIR)/generateDS/ -D generateDS/generateDS.py
	install -t $(INSTALLDIR) set_user_level.py

.PHONY: clean bak

clean:
	rm -f $(make-xml-config-objs) make-xml-config
	rm -f lkc_defs.h zconf.tab.o zconf.tab.c zconf.hash.c lex.zconf.c
	rm -f xsd_defs.pyc

bak:
	make clean
	timestamp=$$(date | sed 's/ /_/g') && mkdir bak/$${timestamp} && rsync -av --exclude bak . bak/$${timestamp}/
