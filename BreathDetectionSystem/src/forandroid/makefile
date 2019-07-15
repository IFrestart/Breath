include makefile.def

DIRS =	\
	socks5	\
	jrtplib-2.7	\
	tinyxmllib	\
	CommBase	\
	NetCtrlDll	\
	FCNetACLib	\
	testClient2

all:
	for i in $(DIRS) ; do \
		if [ -d $$i ]; then	\
			(cd $$i; cd jni; make) ;	\
		else	\
			echo "----------------------------------------" ;	\
			echo "$$i is not a directory..." ;	\
			echo "----------------------------------------" ;	\
		fi	\
	done

clean:
	for i in $(DIRS) ; do \
		if [ -d $$i ]; then	\
			(cd $$i; cd jni; make clean) ;	\
		else	\
			echo "----------------------------------------" ;	\
			echo "$$i is not a directory..." ;	\
			echo "----------------------------------------" ;	\
		fi	\
	done

