SVN_VERSION = 1.6.15
SVN_URL = http://subversion.tigris.org/downloads/subversion-$(SVN_VERSION).tar.bz2
SVN_DEPS_URL = http://subversion.tigris.org/downloads/subversion-deps-$(SVN_VERSION).tar.bz2

$(DST):
	@mkdir -p '$@'

$(DST)/subversion-$(SVN_VERSION).tar.bz2: | $(DST)
	@echo 'Downloading ‘$(@F)’…'
	@curl -sLo '$@' $(SVN_URL)
	@echo 'Extracting ‘$(@F)’…'
	@tar -jxf '$@' -C '$(@D)'

$(DST)/subversion-deps-$(SVN_VERSION).tar.bz2: | $(DST)
	@echo 'Downloading ‘$(@F)’…'
	@curl -sLo '$@' $(SVN_DEPS_URL)
	@echo 'Extracting ‘$(@F)’…'
	@tar -jxf '$@' -C '$(@D)'

$(DST)/subversion-$(SVN_VERSION)/Makefile: $(DST)/subversion-$(SVN_VERSION).tar.bz2 $(DST)/subversion-deps-$(SVN_VERSION).tar.bz2
	@echo 'Configuring ‘subversion-$(SVN_VERSION)’……'
	@cd '$(DST)/subversion-$(SVN_VERSION)' && ./configure >/dev/null CFLAGS=-m32 --enable-static --disable-shared --without-sasl --without-ssl --without-swig --without-neon --without-serf

$(DST)/svnlibs: $(DST)/subversion-$(SVN_VERSION)/Makefile
	@$(MAKE) EXTRA_CFLAGS=-m32 -sC '$(DST)/subversion-$(SVN_VERSION)' external-all
	@$(MAKE) EXTRA_CFLAGS=-m32 -sC '$(DST)/subversion-$(SVN_VERSION)' local-all
	@mkdir -p '$@'
	@cp "$(DST)/subversion-$(SVN_VERSION)/apr/.libs/libapr-1.a" '$(DST)/svnlibs'
	@cp "$(DST)/subversion-$(SVN_VERSION)/apr-util/.libs/libaprutil-1.a" '$(DST)/svnlibs'
	@for LIB in client delta diff fs fs_fs fs_util ra ra_local ra_svn repos subr wc; do cp "$(DST)/subversion-$(SVN_VERSION)/subversion/libsvn_$${LIB}/.libs/libsvn_$${LIB}-1.a" '$(DST)/svnlibs'; done
	@touch '$@'
