abstopsrcdir := $(realpath $(topsrcdir))
abssrcdir := $(realpath .)

TESTS := $(patsubst $(topsrcdir)/%,$(abstopsrcdir)/%,$(TESTS))
TESTS := $(patsubst $(abssrcdir)/%,./%,$(TESTS))

check-regress:

	@echo "RUNTESTFLAGS: $(RUNTESTFLAGS)"
	@echo "RUNTESTFLAGS_INTERNAL: $(RUNTESTFLAGS_INTERNAL)"

	@$(PERL) $(topsrcdir)/regress/run_test.pl $(RUNTESTFLAGS) $(RUNTESTFLAGS_INTERNAL) $(TESTS)


check-long:
	$(PERL) $(topsrcdir)/regress/run_test.pl $(RUNTESTFLAGS) $(TESTS) $(TESTS_SLOW)

