all: base stream toolkit base3d  geometry3d

base:
	$(MAKE) -C Src/Base

stream: base
	$(MAKE) -C Src/Stream

toolkit: stream
	$(MAKE) -C Src/Toolkit

base3d:	toolkit
	$(MAKE) -C Src/Base3D

geometry3d: base3d
	$(MAKE) -C Src/Geometry3D

check: geometry3d
	$(MAKE) -C cunit $@

clean:
	$(MAKE) -C Src/Base $@
	$(MAKE) -C Src/Stream $@
	$(MAKE) -C Src/Toolkit $@
	$(MAKE) -C Src/Base3D $@
	$(MAKE) -C Src/Geometry3D $@
	$(MAKE) -C cunit $@
distclean:clean
	rm -f  Makefile
