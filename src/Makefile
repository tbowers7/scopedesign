all: scopedesign

scopedesign: main.c illumination.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ $+

clean:
	-rm scopedesign &>/dev/null

check: all
	./scopedesign | grep "Hello from .*scopedesign!"
	@echo "*** All TESTS PASSED"

install:
	install -d $(DESTDIR)$(bindir)
	install -m 0755 scopedesign $(DESTDIR)$(bindir)

uninstall:
	-rm $(DESTDIR)$(bindir)/scopedesign &>/dev/null

.PHONY: all clean check install uninstall
