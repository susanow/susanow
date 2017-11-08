
# Hacking Susanow

Template Makefile

```
SSN_SDK = /path/to/susanow
include $(SSN_SDK)/mk/ssn.vars.mk

# If you want to use REST-API to control susanow,
# you must link libboost_system and libboost_thread
# for REST-API-Framework 'crow'
CXXFLAGS = -std=c++11 $(SSN_CXXFLAGS)
LDFLAGS  = $(SSN_LDFLAGS) -lboost_system -lboost_thread

all:
	g++ $(CXXFLAGS) main.cc $(LDFLAGS)
```

