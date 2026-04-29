CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2
LDFLAGS  :=

TARGET   := rnp_miner_main

SRCS     := pdata.cpp          \
            rnp_miner.cpp      \
            rnp_b.cpp          \
            rnp_d.cpp          \
            rnp_pup.cpp        \
            dfom_rnp.cpp       \
            nosep_rnp.cpp      \
            pro_rnp.cpp        \
            prefixspan.cpp     \
            main.cpp

OBJS     := $(SRCS:.cpp=.o)
DEPS     := $(SRCS:.cpp=.d)

.PHONY: all clean run help

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -MMD -MP -c -o $@ $<

-include $(DEPS)

clean:
	rm -f $(TARGET) $(OBJS) $(DEPS)

run: $(TARGET)
	./$(TARGET) $(ARGS)

help:
	@echo "Usage:"
	@echo "  make              Build $(TARGET)"
	@echo "  make clean        Remove build artifacts"
	@echo "  make run ARGS=\"rnp-miner ../datasets/SDB1.txt 10\""
	@echo ""
	@echo "Algorithms: rnp-miner | rnp-b | rnp-d | rnp-pup | dfom-rnp | nosep-rnp | pro-rnp | prefixspan | all"
