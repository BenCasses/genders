##
# Makefile Include for RPM Construction
#   by Chris Dunlap <cdunlap@llnl.gov>
##
# $Id: Make-rpm.mk,v 1.1.2.1 2004-10-26 23:52:34 achu Exp $
##
# REQUIREMENTS:
# - requires project to be under CVS version control
# - requires "PACKAGE" macro definition set to the CVS project name
# - requires "META" file to reside in the top-level directory of the project
# - requires RPM spec file named "$(PACKAGE).spec.in" or "$(PACKAGE).spec"
#     to reside in the top-level directory of the project
##
# META FILE FORMAT:
# - whitespace is ignored
# - comments are ignored (ie, lines beginning with the pound character)
# - lines are of the form "TAG:VALUE"
# - TAGs and VALUEs cannot contain whitespace
# - supported tags: NAME, VERSION, RELEASE
# - NAME tag must be set the same as the PACKAGE macro definition
##
# CVS TAG FORMAT:
# - if RELEASE is not defined, tags are of the form "NAME-VERSION";
#     o/w, they are of the form "NAME-VERSION-RELEASE"
# - periods are converted to hyphens
# - examples: foo v1.2.3 (foo-1-2-3), foo v1.2.3 r4 (foo-1-2-3-4)
##
# NOTES:
# - RPM will be built directly from the CVS repository based on the CVS tag
# - CVS tag will be based on the contents of the META file by default;
#     this allows the version information to be stored and tagged within CVS
# - CVS tag can be specified on the make cmdline to override the default
#     (eg, make rpm tag=foo-1-2-3)
# - CVS "HEAD" tag can be used to build the most recent version in CVS
#     w/o requiring it to be tagged within CVS (eg, make rpm tag=HEAD);
#     this is intended for pre-release testing purposes only
# - CVS "HEAD" releases will have a release number of the form "YYMMDDHHMM"
# - RPM will be signed with a PGP/GPG key if one is specified in ~/.rpmmacros
##
# USAGE:
# - update and cvs commit the META file (cf, META FILE FORMAT)
# - cvs tag/rtag the project (cf, CVS TAG FORMAT)
# - make rpm
##

tar rpm:
	@proj=$(PACKAGE); if test -z "$$proj"; then \
	  echo "ERROR: PACKAGE macro def is not defined." 1>&2; exit 1; fi; \
	if test -z "$$tag"; then \
	  if ! test -f META; then \
	    echo "ERROR: Cannot find $$proj metadata in \"`pwd`/META\"." 1>&2; \
	      exit 1; fi; \
	  name=`perl -ne 'print,exit if s/^\s*NAME:\s*(\S*).*/\1/i' META`; \
	  ver=`perl -ne 'print,exit if s/^\s*VERSION:\s*(\S*).*/\1/i' META`; \
	  rel=`perl -ne 'print,exit if s/^\s*RELEASE:\s*(\S*).*/\1/i' META`; \
	  test -z "$$rel" && pkg=$$name-$$ver || pkg=$$name-$$ver-$$rel; \
	  tag=`echo $$pkg | sed 's/\./-/g'`; fi; \
	tmp=$${TMPDIR-/tmp}/tmp-$$proj-$$$$; \
	rm -rf $$tmp || exit 1; \
	test -n "$(mkinstalldirs)" \
	  && mkdir="$(mkinstalldirs)" || mkdir="mkdir -p"; \
	if ! $$mkdir $$tmp >/dev/null; then \
	  echo "ERROR: Cannot create \"$$tmp\" dir." 1>&2; exit 1; fi; \
	test -f CVS/Root && cvs="cvs -d `cat CVS/Root`" || cvs="cvs"; \
	echo "Fetching $$proj from CVS (tag=$$tag) ..."; \
	(cd $$tmp; $$cvs -Q export -r $$tag $$proj) || exit 1; \
	meta=$$tmp/$$proj/META; \
	if test ! -f "$$meta"; then \
	  echo "ERROR: Cannot find $$proj metadata in CVS." 1>&2; exit 1; fi; \
	name=`perl -ne 'print,exit if s/^\s*NAME:\s*(\S*).*/\1/i' $$meta`; \
	if test "$$proj" != "$$name"; then \
	  echo "ERROR: PACKAGE does not match metadata." 1>&2; exit 1; fi; \
	ver=`perl -ne 'print,exit if s/^\s*VERSION:\s*(\S*).*/\1/i' $$meta`; \
	rver="$$ver"; \
	rel=`perl -ne 'print,exit if s/^\s*RELEASE:\s*(\S*).*/\1/i' $$meta`; \
	if test "$$tag" = "HEAD"; then \
	  if test "$$rel" = "UNSTABLE"; then rel="0.pre0"; fi; \
	  if echo $$rel | grep "pre"; then \
	     rel="$$rel.`date +%Y%m%d%H%M`"; else rel="`date +%Y%m%d%H%M`"; fi; \
	  perl -pi -e "s/^(\s*Release:\s*).*/\$${1}$$rel/i" $$meta; fi; \
	if test -z "$$rel"; then \
	  pkg=$$name-$$ver; rel=1; else pkg=$$name-$$ver-$$rel; fi; \
	if test -x "$$tmp/$$proj/autogen.sh"; then \
          (cd $$tmp/$$proj && ln -sf autogen.sh bootstrap); fi; \
        if test -x "$$tmp/$$proj/bootstrap"; then \
	  mv "$$tmp/$$proj" "$$tmp/$$proj.bak" || exit 1; \
	  (cd "$$tmp/$$proj.bak"; ./bootstrap; ./configure; \
	   $(MAKE) -s distdir) || exit 1; \
	  mv "$$tmp/$$proj.bak/$$name-$$rver" "$$tmp/$$pkg" || exit 1; \
	  rm -rf "$$tmp/$$proj.bak"; \
	  else mv "$$tmp/$$proj" "$$tmp/$$pkg" || exit 1; fi; \
	$(MAKE) -s $@-internal mkdir="$$mkdir" tmp="$$tmp" \
	  proj="$$proj" pkg="$$pkg" ver="$$ver" rel="$$rel" \
	    && rm -rf $$tmp

tar-internal:
	@echo "Creating $$pkg.tgz ..."; \
	rm -f $$pkg.tgz || exit 1; \
	(cd $$tmp; tar -cf - $$pkg) | gzip -c9 >$$tmp/$$pkg.tgz; \
	cp -p $$tmp/$$pkg.tgz $$pkg.tgz || exit 1

rpm-internal: tar-internal
	@echo "Creating $$pkg*rpm ..."; \
	rpm=`type -p rpmbuild || type -p rpm`; \
	for d in BUILD RPMS SOURCES SPECS SRPMS TMP; do \
	  if ! $$mkdir $$tmp/$$d >/dev/null; then \
	    echo "ERROR: Cannot create \"$$tmp/$$d\" dir." 1>&2; exit 1; fi; \
	      done; \
	mv $$tmp/$$pkg.tgz $$tmp/SOURCES/ || exit 1; \
	test -f $$tmp/$$pkg/$$proj.spec.in \
	  && spec=$$tmp/$$pkg/$$proj.spec.in \
	  || spec=$$tmp/$$pkg/$$proj.spec; \
	if ! test -f $$spec; then \
	  echo "ERROR: Cannot find $$proj spec file in CVS." 1>&2; exit 1; fi; \
	sed -e "s/^\([ 	]*Name:\).*/\1 $$proj/i" \
	    -e "s/^\([ 	]*Version:\).*/\1 $$ver/i" \
	    -e "s/^\([ 	]*Release:\).*/\1 $$rel/i" \
	    -e "s/^\([ 	]*Source0?:\).*/\1 $$pkg.tgz/i" \
	    <$$spec >$$tmp/SPECS/$$proj.spec; \
	if ! test -s $$tmp/SPECS/$$proj.spec; then \
	  echo "ERROR: Cannot create $$proj.spec." 1>&2; exit 1; fi; \
	$$rpm --showrc | egrep "_(gpg|pgp)_nam" >/dev/null && sign="--sign"; \
	args="-ba --define '_tmppath $$tmp/TMP' --define '_topdir $$tmp'"; \
	for pkg in $$WITH_OPTIONS; do \
	   args="$$args --define '_with_$$pkg --with-$$pkg'"; \
    done; \
	for pkg in $$WITHOUT_OPTIONS; do \
	   args="$$args --define '_without_$$pkg --without-$$pkg'"; \
    done; \
	args="$$args $$sign --quiet $$tmp/SPECS/$$proj.spec"; \
	if ! sh -c "$$rpm $$args" >$$tmp/rpm.log 2>&1; then \
	        cat $$tmp/rpm.log; exit 1; fi; \
	cp -p $$tmp/RPMS/*/$$proj-*.rpm $$tmp/SRPMS/$$proj-*.src.rpm . || exit 1
