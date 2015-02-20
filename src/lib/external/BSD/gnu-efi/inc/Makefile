SRCDIR = .

VPATH = $(SRCDIR)

include $(SRCDIR)/../Make.defaults

TOPDIR = $(SRCDIR)/..

CDIR=$(TOPDIR)/..

INCDIRINSTALL = $(INSTALLROOT)$(PREFIX)/include/efi

SRCHEADERS = $(wildcard $(SRCDIR)/*.h) \
             $(wildcard $(SRCDIR)/protocol/*.h) \
             $(wildcard $(SRCDIR)/$(ARCH)/*.h)
ifeq ($(ARCH),ia64)
 SRCHEADERS += $(wildcard $(SRCDIR)/protocol/$(ARCH)/*.h)
endif

HEADERS = $(patsubst $(SRCDIR)/%,%,$(SRCHEADERS))

all:

clean:

	rm -f *~

$(INCDIRINSTALL) \
$(INCDIRINSTALL)/protocol \
$(INCDIRINSTALL)/$(ARCH) \
$(INCDIRINSTALL)/protocol/$(ARCH):
	mkdir -p $@

.SECONDEXPANSION:

$(INCDIRINSTALL)/%.h: %.h | $$(dir $$@)
	$(INSTALL) -m 644 $< $(dir $@)

install: $(addprefix $(INCDIRINSTALL)/,$(HEADERS))

include $(SRCDIR)/../Make.rules
