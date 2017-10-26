
PREF = $(SSN_SDK)/docs/sphinx
html:
	make -C $(PREF) html

open:
	make -C $(PREF) open

clean:
	make -C $(PREF) clean


