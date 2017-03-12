


.SUFFIXES: .out .c .cc .o .hS
.cc.o:
	@echo "CXX $@"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

.c.o:
	@echo "CC $@"
	@$(CC) $(CFLAGS) -c $< -o $@

# $(TARGET): $(CXXOBJS) $(COBJS)
# 	@echo "LD $@"
# 	@$(CXX) $(CXXFLAGS) -o $(TARGET) $(CXXOBJS) $(COBJS) $(LDFLAGS)


