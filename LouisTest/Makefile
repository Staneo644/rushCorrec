NAME		= ft_president

SRCS		= \
			  srcs/init.c \
			  srcs/parse.c \
			  srcs/read_data.c \

CPPSRCS		= \
			  srcs/solve.cpp \
			  main.cpp \

ASMSRCS		= \

HEADERS		= \
			  includes

CCDEFS		= \

LIB_NAMES	= \
			  
DYN_LIBS	= \

DYN_LIBS	:= $(addprefix -l, $(DYN_LIBS))

LIBS		= $(subst lib,-l,$(notdir $(LIB_NAMES)))
LIB_LD		= $(foreach lib,$(LIB_NAMES),-L$(lib))
LIB_PATHS	= $(foreach lib,$(LIB_NAMES),$(lib)/$(notdir $(lib)).a)
LIB_HEADERS	= $(foreach lib,$(LIB_NAMES),-I$(lib)/includes/)

CCDEFSFLGS	= $(foreach def,$(CCDEFS),-D $(def))

BUILDDIR	= .build
OBJS		= $(SRCS:%.c=$(BUILDDIR)/%.o) \
			  $(CPPSRCS:%.cpp=$(BUILDDIR)/%.o) \
			  $(ASMSRCS:%.s=$(BUILDDIR)/%.o)
DEPS		= $(SRCS:%.c=$(BUILDDIR)/%.d) \
			  $(CPPSRCS:%.cpp=$(BUILDDIR)/%.d)
CC			= cc
CCWFLGS		= -O2 -march=native -ffast-math
CCDBGFLGS	= #-fsanitize=address -g
CCO1FLGS	= -O1 -march=native -ffast-math
CCO2FLGS	= -O2 -march=native -ffast-math
CCO3FLGS	= -O3 -march=native -ffast-math
CXX			= c++ -std=c++17
CXXWFLGS	= -O2 -march=native -ffast-math
CXXDBGFLGS	= #-fsanitize=address -g
CXXO1FLGS	= -O1 -march=native -ffast-math
CXXO2FLGS	= -O2 -march=native -ffast-math
CXXO3FLGS	= -O3 -march=native -ffast-math
DEPSFLAGS	= -MMD -MP
RM			= rm -Rf
MAKE		= make -C
MKDIR		= mkdir
AR			= ar
ARFLAGS		= rcs
NASM		= nasm
NASMFLAGS	= -felf64

all : libs $(NAME)

$(NAME) : $(LIB_PATHS) $(OBJS)
		$(CXX) $(CXXWFLGS) -o $(NAME) $(OBJS) $(LIB_LD) $(LIBS) $(DYN_LIBS)

bonus : $(NAME)

libs :
		$(foreach lib, $(LIB_NAMES), \
			$(MAKE) $(lib); \
		)

clean :
		-$(RM) $(BUILDDIR)

fclean : clean
		$(foreach lib, $(LIB_NAMES), \
			$(MAKE) $(lib) fclean; \
		)
		-$(RM) $(NAME)

re : fclean all

-include $(DEPS)

$(BUILDDIR)/%.o : %.c Makefile $(LIB_PATHS)
		@mkdir -p $(@D)
		$(CC) $(CCWFLGS) $(DEPSFLAGS) $(CCDEFSFLGS) -I$(HEADERS) $(LIB_HEADERS) -c $< -o $@

$(BUILDDIR)/%.o : %.cpp Makefile $(LIB_PATHS)
		@mkdir -p $(@D)
		$(CXX) $(CXXWFLGS) $(DEPSFLAGS) $(CCDEFSFLGS) -I$(HEADERS) $(LIB_HEADERS) -c $< -o $@

$(BUILDDIR)/%.o : %.s Makefile $(LIB_PATHS)
		@mkdir -p $(@D)
		$(NASM) $(NASMFLAGS) -o $@ $<

.PHONY: all clean fclean re bonus libs
