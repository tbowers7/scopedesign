SUBDIRS = libargtable libxpa data src

upload: $(DIST_ARCHIVES)
	scp $(DIST_ARCHIVES) ellswotp@arlsrv2.colorado.edu:unpack/.

$(DIST_ARCHIVES): dist
