head = dask.h
srcs = dask.cc
objs = dask.o
opts = -w -g -c
all:	 dask
dask: 	$(objs)
	g++ $(objs) -o dask
dask.o: $(srcs) $(head)
	g++ $(opts) $(srcs)
clean:
	rm dask *.o
