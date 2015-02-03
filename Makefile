# Makefile

# compiler options
CXX			+= # -pg
CXXFLAGS	 = -pipe -Wall -O2 -mtune=native -fstack-protector-strong --param=ssp-buffer-size=4 # -Wfatal-errors # -g -ggdb
LDFLAGS		 = -s -lmicrohttpd -lpthread -Wl,-O1,--sort-common,--as-needed,-z,relro
SRC_NAME	 = main
LIBS		 = /home/asdf/Desktop/libmicrohttpd-0.9.35/install
EXECUTABLE	 = my_httpd
SRC_EXT		 = cpp


# targets
all: $(EXECUTABLE)

$(EXECUTABLE): $(patsubst %,%.o,$(SRC_NAME))
	@echo
	$(CXX) -L$(LIBS)/lib $(patsubst %,%.o,$(SRC_NAME)) $(LDFLAGS) -o $(EXECUTABLE)
	@echo

$(patsubst %,%.o,$(SRC_NAME)): %.o : %.$(SRC_EXT)
	@echo
	$(CXX) $(CXXFLAGS) -I$(LIBS)/include -c $< -o $@

clean:
	@echo
	rm -f $(patsubst %,./%.o,$(SRC_NAME)) ./$(EXECUTABLE)
	@echo
