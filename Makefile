include config.mk

build:
	$(CC) powermenu.c -o powermenu

install: build
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f powermenu $(DESTDIR)$(PREFIX)/bin
	chmod 755 $(DESTDIR)$(PREFIX)/bin/powermenu

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/powermenu
