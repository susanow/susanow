
# Hacking Susanow

Template Makefile

```
SSN_SDK = /path/to/susanow
include $(SSN_SDK)/mk/ssn.vars.mk

CXXFLAGS = -std=c++11 $(SSN_CXXFLAGS)
LDFLAGS  = $(SSN_LDFLAGS)

all:
	g++ $(CXXFLAGS) main.cc $(LDFLAGS)
```

