INCLUDES = -I$(abs_top_srcdir)/opengles/include -I$(abs_top_srcdir)/opengles/src -I$(abs_top_srcdir)/opengles/src/linux-ftk \
	-I$(abs_top_srcdir)/opengles/src/arm -I$(abs_top_srcdir)/opengles/src/codegen \
	-I$(abs_top_srcdir)/src -I$(abs_top_srcdir)/src/im  -I$(abs_top_srcdir)/src/os/linux -I$(abs_top_srcdir)/ \
	-I$(abs_top_srcdir)/apps/common	-I$(abs_top_srcdir)/src/demos -DUSE_FTK 

noinst_PROGRAMS=aargb  alpha3D  alpha  checker  colormat  cube  double  fog  hello  light  lines  material  mipmap  model  movelight  planet  polyoff  robot  scene  smooth  texbind  texsub  torus  varray  wrap

LIBS=$(abs_top_builddir)/src/libftk.la $(abs_top_builddir)/opengles/src/libopengles.la -lpthread -ldl -lm

lib_LTLIBRARIES=libftk_opengles_demos.la
libftk_opengles_demos_la_SOURCES=$(top_srcdir)/src/demos/ftk_app_demo.c alpha3D.c material.c fog.c hello.c

libftk_opengles_demos_la_LDFLAGS=$(LIBS)
libftk_opengles_demos_la_CFLAGS=-DFTK_AS_PLUGIN

aargb_SOURCES=$(srcdir)/aargb.c
aargb_LDFLAGS=$(LIBS)

alpha3D_SOURCES=$(srcdir)/alpha3D.c
alpha3D_LDFLAGS=$(LIBS)

alpha_SOURCES=$(srcdir)/alpha.c
alpha_LDFLAGS=$(LIBS)

checker_SOURCES=$(srcdir)/checker.c
checker_LDFLAGS=$(LIBS)

colormat_SOURCES=$(srcdir)/colormat.c
colormat_LDFLAGS=$(LIBS)

cube_SOURCES=$(srcdir)/cube.c
cube_LDFLAGS=$(LIBS)

double_SOURCES=$(srcdir)/double.c
double_LDFLAGS=$(LIBS)

fog_SOURCES=$(srcdir)/fog.c
fog_LDFLAGS=$(LIBS)

hello_SOURCES=$(srcdir)/hello.c
hello_LDFLAGS=$(LIBS)

light_SOURCES=$(srcdir)/light.c
light_LDFLAGS=$(LIBS)

lines_SOURCES=$(srcdir)/lines.c
lines_LDFLAGS=$(LIBS)

material_SOURCES=$(srcdir)/material.c
material_LDFLAGS=$(LIBS)

mipmap_SOURCES=$(srcdir)/mipmap.c
mipmap_LDFLAGS=$(LIBS)

model_SOURCES=$(srcdir)/model.c
model_LDFLAGS=$(LIBS)

movelight_SOURCES=$(srcdir)/movelight.c
movelight_LDFLAGS=$(LIBS)

planet_SOURCES=$(srcdir)/planet.c
planet_LDFLAGS=$(LIBS)

polyoff_SOURCES=$(srcdir)/polyoff.c
polyoff_LDFLAGS=$(LIBS)

robot_SOURCES=$(srcdir)/robot.c
robot_LDFLAGS=$(LIBS)

scene_SOURCES=$(srcdir)/scene.c
scene_LDFLAGS=$(LIBS)

smooth_SOURCES=$(srcdir)/smooth.c
smooth_LDFLAGS=$(LIBS)

texbind_SOURCES=$(srcdir)/texbind.c
texbind_LDFLAGS=$(LIBS)

texsub_SOURCES=$(srcdir)/texsub.c
texsub_LDFLAGS=$(LIBS)

torus_SOURCES=$(srcdir)/torus.c
torus_LDFLAGS=$(LIBS)

varray_SOURCES=$(srcdir)/varray.c
varray_LDFLAGS=$(LIBS)

wrap_SOURCES=$(srcdir)/wrap.c
wrap_LDFLAGS=$(LIBS)

desktopdir=$(datadir)/ftk/base/apps
desktop_DATA=opengles_demos.desktop

EXTRA_DIST=opengles_demos.desktop
