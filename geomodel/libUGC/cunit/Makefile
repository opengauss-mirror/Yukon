SRC =\
	cu_example.cpp \
	cu_tester.cpp \

CUNIT_LDFLAGS= -lcunit


cu_tester: 
	g++ $(SRC) -lcunit -I../Include -L../Builds/MakeProject/Bin64_Linux -lYkBase3D -lYkBase   -lYkGeometry3D -lYkStream -lYkToolkit -D_UGUNICODE -D_UGQSTRING -g -fshort-wchar -o $@


check: cu_tester
	@./cu_tester

clean:
	rm *.o ./cu_tester -f