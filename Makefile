#
# tp-autofan makefile
#

CC ?= gcc
SRCDIR := src
BUILDDIR := build
TARGETDIR := bin
PROJNAME := tp-autofan
TARGET := $(TARGETDIR)/$(PROJNAME)
INSTALLPREFIX := /usr/local/bin
SERVICEFILE := extra/$(PROJNAME).service
SYSTEMDPREFIX := /etc/systemd
SRCEXT := c
SOURCES := fancontrol.c hwmon_wrapper.c main.c
OBJECTS := $(BUILDDIR)/fancontrol.o $(BUILDDIR)/hwmon_wrapper.o $(BUILDDIR)/main.o
DEBUGFLAGS := -Wall -Wextra -pedantic -Werror -Wfatal-errors -Wformat=2 \
	-Wunused-function -Wswitch-enum -Wcast-align -Wpointer-arith \
	-Wbad-function-cast -Wstrict-overflow=5 -Wfloat-conversion \
	-Wstrict-prototypes -Winline -Wundef -Wnested-externs -Wcast-qual \
	-Wshadow -Wunreachable-code -Wlogical-op -Wfloat-equal -Wredundant-decls \
	-Wold-style-definition -ggdb3 -O0 -fno-omit-frame-pointer -ffloat-store \
	-fno-common -fstrict-aliasing
RELEASEFLAGS := -O3 -Wall -Wl,--strip-all
LIB := 
INC := 

all: release-gnu89

debug-gnu89: | gnu89 debug-flag makedirs $(OBJECTS) $(TARGET)

release-gnu89: | gnu89 release-flag makedirs $(OBJECTS) $(TARGET)

# NOTE: This (naively) assumes systemd is the init
install: release-gnu89
	@cp -v $(TARGET) $(INSTALLPREFIX)/
	@cp -v $(SERVICEFILE) $(SYSTEMDPREFIX)/system/
	@echo " systemctl enable $(PROJNAME)" ; \
		systemctl enable $(PROJNAME)

$(TARGET): $(OBJECTS)
	@echo ""
	@echo " Linking..."
	@echo " $(CC) $(CFLAGS) $^ -o $(TARGET) $(LIB)" ; \
		$(CC) $(CFLAGS) $^ -o $(TARGET) $(LIB)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@echo " $(CC) $(CFLAGS) $(INC) -c -o $@ $<"; \
		$(CC) $(CFLAGS) $(INC) -c -o $@ $<

makedirs:
	@mkdir -p $(BUILDDIR)
	@mkdir -p $(TARGETDIR)

debug-flag:
	$(eval CFLAGS += $(DEBUGFLAGS))

release-flag:
	$(eval CFLAGS += $(RELEASEFLAGS))

c89:
	$(eval DEBUGFLAGS="-std=c89" $(DEBUGFLAGS))
	$(eval RELEASEFLAGS="-std=c89" $(RELEASEFLAGS))

gnu89:
	$(eval DEBUGFLAGS="-std=gnu89" "-D_GNU_SOURCE" $(DEBUGFLAGS))
	$(eval RELEASEFLAGS="-std=gnu89" "-D_GNU_SOURCE" $(RELEASEFLAGS))

clean:
	@echo " Cleaning..."; 
	@echo " $(RM) -r $(BUILDDIR) $(TARGETDIR)" ; \
		$(RM) -r $(BUILDDIR) $(TARGETDIR)

.PHONY: all makedirs debug release clean tests
