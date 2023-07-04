# Project & library structure.
LIBNAME    = zigil
PROJDIR    = $(realpath $(CURDIR)/..)
SRCDIR     = $(PROJDIR)/src
BUILDDIR   = $(PROJDIR)/build/linux

CC = gcc
AR = ar -rc
RM = rm -rf
CP = cp -v
MKDIR = mkdir -vp

# Headers
HDRS = zigil.h \
zigil_event.h \
zigil_input.h \
zigil_keycodes.h \
zigil_dir.h \
zigil_time.h \
zigil_mem.h \
zigil_mip.h \

DIBLIB_LOCAL_HDRS = \
diblib_local/bfio.h \
diblib_local/dibassert.h \
diblib_local/dibhash.h \
diblib_local/dibstr.h \
diblib_local/fixpt.h \
diblib_local/term_colors.h \
diblib_local/verbosity.h 

# Headers which should be copied to diblib directory (ie. ignore any "internal"
# headers). NOTE: As of 2023-07-04 there are no internal headers.
PUBLIC_HEADERS = $(addprefix $(SRCDIR)/, $(HDRS))
DIBLIB_LOCAL_PUBLIC_HEADERS = $(addprefix $(SRCDIR)/, $(DIBLIB_LOCAL_HDRS))

GPROF_CFLAGS  = #-pg
GPROF_LDFLAGS = #-pg

SANITIZE_CFLAGS  = #-fsanitize=address,undefined
SANITIZE_LDFLAGS = #-fsanitize=address,undefined

INCLUDES = -I$(PROJDIR)

CFLAGS = \
-Wall \
-Wextra \
-Wconversion \
-Wdouble-promotion \
-Wno-unused-parameter \
-Wno-unused-function \
-Wno-sign-conversion \
-std=c99 -pedantic \
-MMD \
-O3 \
$(SANITIZE_CFLAGS) \
$(GPROF_CFLAGS)

LDFLAGS = $(SANITIZE_LDFLAGS) $(GPROF_LDFLAGS)
LDLIBS =  #-lbfio #-ldibhash #-lX11 -lX11ext

DIRS = 2D X11 diblib_local
TARGETDIRS = $(foreach dir, $(DIRS), $(addprefix $(BUILDDIR)/, $(dir)))

TARGET_LIB = $(BUILDDIR)/lib$(LIBNAME).a
TARGET_OBJ = $(BUILDDIR)/lib$(LIBNAME).o

# These are C source files which are included directly into other C source
# files, and therefore are not compiled separately. TODO: Avoid this madness.
INCLUDE_SRCS = \
zigil_time.c \
X11/zglx_printstruct.c

# Platform-agnostic source files.
COMMON_SRCS = \
zigil.c \
zigil_event.c \
zigil_keycodes.c \
zigil_bmp.c \
zigil_qoi.c \
zigil_mem.c \
zigil_mip.c \
2D/draw_segs_and_circles.c \
2D/clipping.c \
diblib_local/bfio.c \
diblib_local/dibhash.c \
diblib_local/dibstr.c \
diblib_local/fixpt.c


# All source files, including platform-dependent ones.
SRCS = \
$(COMMON_SRCS) \
X11/zglx.c \
X11/zglx_dir.c \
X11/zglx_memory.c \
X11/zglx_event.c \
X11/zglx_ZK_from_XK.c \
X11/zglx_time.c
SOURCES = $(addprefix $(SRCDIR)/,$(SRCS))

OBJS = $(addprefix $(BUILDDIR)/,$(SRCS:.c=.o))
DEPS = $(OBJS:.o=.d)

VERBOSE = #TRUE
ifeq ($(VERBOSE), TRUE)
	HIDE =
else
	HIDE = @
endif

define generateRules
$(1) : $$(subst /build/linux,/src,$(1:.o=.c))
	$$(HIDE)printf '$$(BOLD)$$(ORANGE)Building$$(SGR0) $$@\n'
	$$(HIDE)$$(CC) $$(CFLAGS) $$(INCLUDES) -c $$< -o $$@
endef

.PHONY : all clean install directories

all : directories $(TARGET_LIB)

BOLD = $(shell tput bold)
ORANGE = $(shell tput setaf 11)
SGR0 = $(shell tput sgr0)

$(TARGET_LIB) : $(TARGET_OBJ)
	$(HIDE)printf '$(BOLD)$(ORANGE)Creating archive$(SGR0) $@\n'
	$(HIDE)$(AR) $(TARGET_LIB) $(TARGET_OBJ)

$(TARGET_OBJ) : $(OBJS)
	$(HIDE)printf '$(BOLD)$(ORANGE)Partially linking all object files into $(SGR0)$(TARGET_OBJ)\n'
	$(HIDE)$(CC) $(LDFLAGS) -r $(OBJS) $(LDLIBS) -o $(TARGET_OBJ)

-include $(DEPS)

$(foreach obj, $(OBJS), $(eval $(call generateRules, $(obj))))

directories :
	$(HIDE)$(MKDIR) $(TARGETDIRS)

# Used to copy the library to the diblib directory.
A_DEST     = $(realpath $(PROJDIR)/../epm/zigil)
H_DEST     = $(realpath $(PROJDIR)/../epm/zigil)
#A_DEST     = $(DIBLIB_DIR)/a
#H_DEST     = $(DIBLIB_DIR)/h

install : 
	$(HIDE)printf '$(BOLD)$(ORANGE)Copying library$(SGR0)\n'
	$(HIDE)$(CP) $(TARGET_LIB) $(A_DEST)
	$(HIDE)$(MKDIR) $(H_DEST)
	$(HIDE)$(MKDIR) $(H_DEST)/diblib_local
	$(HIDE)printf '$(BOLD)$(ORANGE)Copying headers$(SGR0)\n'
	$(HIDE)$(CP) $(PUBLIC_HEADERS) $(H_DEST)
	$(HIDE)$(CP) $(DIBLIB_LOCAL_PUBLIC_HEADERS) $(H_DEST)/diblib_local

clean :
	$(RM) $(BUILDDIR)
	@echo Cleaning done!
