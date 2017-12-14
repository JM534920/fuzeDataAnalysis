CC = gcc
INCL = -Iinclude -I$(MDSPLUS_DIR)/include -I/usr/include/libxml2
FLAGS = -g -Wall
LIBRY = -lgsl -lgslcblas -lxml2 -lm -L$(MDSPLUS_DIR)/lib -lMdsLib

all: example

example: src/dataAnalysis.c src/main.c src/getLFData.c src/getSVFData.c
	$(CC) $(FLAGS) $(INCL) \
	src/dataAnalysis.c src/main.c src/getLFData.c src/getSVFData.c \
	$(LIBRY) -o example

clean:
	rm -f *~ *.o example
