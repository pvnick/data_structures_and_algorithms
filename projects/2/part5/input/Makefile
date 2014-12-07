# Add .d to Make's recognized suffixes.
SUFFIXES += .d
CXX=g++
LDFLAGS=
SOURCES=$(wildcard src/*.cpp)
INC=-Iinclude
CXXFLAGS=--std=c++11 $(INC) -MMD -MP
OUTDIR=obj
DEPDIR=.depends
NODEPS=clean test
DEPFILENAMES:=$(patsubst %.cpp,%.d,$(SOURCES))
DEPFILEPATHS:=$(patsubst src/%.d,$(DEPDIR)/%.d,$(DEPFILENAMES))
EXECUTABLE=part5

#Don't create dependencies when we're cleaning, for instance
ifeq (0, $(words $(findstring $(MAKECMDGOALS), $(NODEPS))))
	#Chances are, these files don't exist.  GMake will create them and
	#clean up automatically afterwards
	-include $(DEPFILEPATHS)
endif

default:
	echo foobar $(DEPFILEPATHS)
	-include $(DEPFILEPATHS)

#%.o:%.d
#	$(Gpp) -c $*.cpp

#%.d:%.cpp
#	echo hello world
#	mkdir -p $(DEPDIR)/$(shell dirname $*.d)
#	$(CXX) $(CXXFLAGS) $*.cpp > $(DEPDIR)/$*.d

#This is the rule for creating the dependency files
$(DEPDIR)/%.d: %.cpp
	mkdir -p $(DEPDIR)/$(shell dirname $*.d)
	echo $(patsubst src/%.cpp,obj/%.o,$<)
	#$(CXX) $(CXXFLAGS) -MM -MT '$(patsubst src/%.cpp,obj/%.o,$<)' $< -MF $@

#This rule does the compilation
obj/%.o: src/%.cpp src/%.d src/%.h
	echo hello world

-include $(DEPFILEPATHS)
#
#-include $(DEPFILEPATHS)

#Gpp = g++
#srcs = $(wildcard *.cpp)
#objs = $(srcs:.cpp=.o)
#deps = $(srcs:.cpp=.d)

#test: $(objs)
#    $(Gpp) $^ -o $@

#%.o: %.cpp
#    $(Gpp) -MMD -MP -c $< -o $@

#.PHONY: clean

# $(RM) is rm -f by default
#clean:
#    $(RM) $(objs) $(deps) test
