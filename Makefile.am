AUTOMAKE_OPTIONS = foreign

SUBDIRS = . images

bin_PROGRAMS = gms
bin_SCRIPTS = $(scripts)

scripts = $(perl_scripts)
perl_scripts = rptgen

gms_SOURCES = gts_monitor.c \
              callback.c \
              comm.c \
              gts_monitor_form.c gts_monitor_form.h \
              gms2monitor_comm.c \
              gms_comm.c \
              gts_monitor_form_cb.c \
              gms_time.c comm.h gms_comm.h gms_time.h gts_monitor.h

network_hosts_filename=$(prefix)/data/gms_network_hosts

INCLUDES = -DPROG_VERSION=\"$(PACKAGE)-$(VERSION)\" -DGMS_NETWORK_HOSTS_FILENAME=\"$(network_hosts_filename)\" -I/usr/local/include -I/usr/X11/include -I/usr/X11R6/include

EXTRA_DIST = $(network_hosts_filename) $(bin_SCRIPTS) CHANGES gts_monitor_form.fd

gts_monitor_form.c gts_monitor_form.h: gts_monitor_form.fd
	fdesign -convert $?

rptgen: Makefile
	@for p in $(scripts); do \
	   echo -n "Checking file $$p ... ";\
	   cp $$p $$p.in;\
	   sed -e 's&#\!.*perl&#\! $(perlbin)&' \
	   < $$p.in > $$p.new;\
	   if cmp -s $$p $$p.new; then \
          rm $$p.new; \
          echo "$$p remains untouched."; \
       else \
          mv $$p.new $$p; \
          echo "substitutions made in $$p."; \
       fi;\
	   rm -f $$p.in;\
    done
