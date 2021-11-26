SRC =\
	cu_Yk.cpp \
	cu_tester.cpp \

CUNIT_LDFLAGS= -lcunit

uname_m = $(shell uname -m)

LD_FLAGS.x86_64 = ../Builds/MakeProject/Bin64_Linux
LD_FLAGS.aarch64 = ../Builds/MakeProject/Binaarch64_Linux

LD_FLAGS += $(LD_FLAGS.$(uname_m))

cu_tester: 
	g++ $(SRC) -lcunit -I../Include -L$(LD_FLAGS) -lYkBase3D -lYkBase -lYkStream -lYkToolkit -lYkGeometry3D -D_UGUNICODE -D_UGQSTRING -O2 -fshort-wchar -o $@


check: cu_tester
	@./cu_tester

clean:
	rm *.o ./cu_tester -f