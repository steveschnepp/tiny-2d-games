.PHONY: all gfx feos clean install

all: feos

nds: gfx
	@$(MAKE) --no-print-directory -f Makefile.libnds || exit 1

gfx:
	@$(MAKE) --no-print-directory -C gfx || exit 1
	@for i in gfx/*.s; do if ! grep -q hidden $$i; then sed 's/\(\([ \t]*\)\.global\([ \t]*\)\([^ \t]*\).*\)/\1\n\2.hidden\3\4/g' -i $$i; fi; done

feos: gfx
	@THIS_MAKEFILE=Makefile.FeOS $(MAKE) -f Makefile.FeOS || exit 1

clean:
	@$(MAKE) --no-print-directory -C gfx clean || exit 1
	@THIS_MAKEFILE=Makefile.FeOS $(MAKE) -f Makefile.FeOS clean || exit 1
	@$(MAKE) --no-print-directory -f Makefile.libnds clean || exit 1

install: all
	@THIS_MAKEFILE=Makefile.FeOS $(MAKE) -f Makefile.FeOS install || exit 1
