BINDIR = bin
OBJDIR = bin/obj
SRCDIR = src
INCDIR = include

TARGETS = hog
CXX = g++

CXXFLAGS += -O3 -Wall -I ${INCDIR} -I /usr/local/boost_1_52_0
LDFLAGS += -lX11 -lpthread -L /usr/X11/lib


########################################################################

.PHONY: all clean

all: $(addprefix $(BINDIR)/,$(TARGETS))

clean:
	rm -rf $(OBJDIR)/*.o
	
$(OBJDIR)/hog_wrappers.o:	$(SRCDIR)/hog_wrappers.cpp $(INCDIR)/hog_wrappers.h
	@test -e $(dir $@) || mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OBJDIR)/integral_histogram.o:	$(SRCDIR)/integral_histogram.cpp $(INCDIR)/integral_histogram.h
	@test -e $(dir $@) || mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OBJDIR)/hog_extractor.o:	$(SRCDIR)/hog_extractor.cpp $(INCDIR)/hog_extractor.h  $(INCDIR)/integral_histogram.h
	@test -e $(dir $@) || mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c -o $@ $<
	
$(OBJDIR)/hog.o:	$(SRCDIR)/hog.cpp
	@test -e $(dir $@) || mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BINDIR)/hog: $(OBJDIR)/hog.o $(OBJDIR)/hog_extractor.o $(OBJDIR)/integral_histogram.o $(OBJDIR)/hog_wrappers.o Makefile
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $(OBJDIR)/hog.o $(OBJDIR)/integral_histogram.o $(OBJDIR)/hog_extractor.o $(OBJDIR)/hog_wrappers.o 

