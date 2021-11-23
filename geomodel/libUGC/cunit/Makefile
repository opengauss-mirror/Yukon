SRC =\
	cu_example.cpp \
	cu_tester.cpp \

CUNIT_LDFLAGS= -lcunit


cu_tester: 
	g++ $(SRC) -lcunit -o $@


check: cu_tester
	@./cu_tester

clean:
	rm *.o ./cu_tester -f