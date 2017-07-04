
## use libsusanow

```
include $(SSN_SDK)/mk/ssn.vars.mk
CXXFLAGS += $(SSN_CXXFLAGS)
LDFLAGS  += $(SSN_LDFLAGS)

def:
	$(CXX) $(CXXFLAGS) main.cc $(LDFLAGS)
```
