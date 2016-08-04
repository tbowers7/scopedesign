SUBDIRS = data src

upload: $(DIST_ARCHIVES)
	scp $(DIST_ARCHIVES) ellswotp@origins2.colorado.edu:public_html/scopedesign/.

$(DIST_ARCHIVES): dist
