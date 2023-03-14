# Compiler flags
CC = gcc
CFLAGS = -Wall -Wextra -Werror

# Directories
SRCDIR = src
LIBDIR = lib
PARSEDIR = parser
LOGDIR = log
OBJDIR = obj
INCLUDEDIR = includes

# Files
SERVER = server
UTILS = server_utils.c
LIBNAME = libpicohttpparser.a
PARSEFILE = $(PARSEDIR)/picohttpparser.c
HEADERS = $(INCLUDEDIR)/picohttpparser.h $(INCLUDEDIR)/server.h
LOGFILE = server.log

# Targets
.PHONY: all clean fclean

all: $(OBJDIR) $(LIBDIR) $(LIBDIR)/$(LIBNAME) $(SERVER)

$(SERVER): $(OBJDIR)/server.o $(OBJDIR)/server_utils.o $(LIBDIR)/$(LIBNAME) $(HEADERS)
	$(CC) $(CFLAGS) -o $(SERVER) $(OBJDIR)/server.o $(OBJDIR)/server_utils.o -L$(LIBDIR) -lpicohttpparser

$(LIBDIR)/$(LIBNAME): $(OBJDIR)/picohttpparser.o
	ar rcs $(LIBDIR)/$(LIBNAME) $(OBJDIR)/picohttpparser.o

$(OBJDIR)/picohttpparser.o: $(PARSEFILE) $(INCLUDEDIR)/picohttpparser.h
	$(CC) $(CFLAGS) -c $(PARSEFILE) -o $(OBJDIR)/picohttpparser.o

$(OBJDIR)/server.o: $(SRCDIR)/server.c $(HEADERS)
	$(CC) $(CFLAGS) -c $(SRCDIR)/server.c -o $(OBJDIR)/server.o

$(OBJDIR)/server_utils.o: $(SRCDIR)/server_utils.c $(HEADERS)
	$(CC) $(CFLAGS) -c $(SRCDIR)/server_utils.c -o $(OBJDIR)/server_utils.o

$(OBJDIR):
	mkdir $(OBJDIR)

$(LIBDIR):
	mkdir $(LIBDIR)

fclean: clean
	rm -f $(LIBDIR)/$(LIBNAME)

clean:
	rm -rf $(OBJDIR)
	rm -f $(SERVER)
	rm -f $(LOGDIR)/$(LOGFILE)
	rm -f $(LIBDIR)/$(LIBNAME)

re: fclean all
