CC=gcc
CFLAG=-fPIC -c
LDFLAG=-shared
all: fivepos test
fivepos:
	mkdir pos1 pos2 pos3
	$(CC) $(CFLAG) -o pos.o pos1.c
	$(CC) $(LDFLAG) pos.o -o libpos.so    	
	mv libpos.so pos1
	$(CC) $(CFLAG) -o pos.o pos2.c
	$(CC) $(LDFLAG) pos.o -o libpos.so
	mv libpos.so pos2
	$(CC) $(CFLAG) -o pos.o pos3.c
	$(CC) $(LDFLAG) pos.o -o libpos.so
	mv libpos.so pos3
	$(CC) $(CFLAG) -o pos.o pos4.c
	$(CC) $(LDFLAG) pos.o -o libpos.so
	# sudo mv libpos.so /lib
	$(CC) $(CFLAG) -o pos.o pos5.c
	$(CC) $(LDFLAG) pos.o -o libpos.so
	# sudo mv libpos.so /usr/lib
test: main.c
	gcc -o test main.c -L pos1 -lpos -Wl,-rpath,pos1
# -L pos1 是说连接时到 pos1 目录去找
# -Wl,-rpath,pos1 是说运行时到pos1目录去找 