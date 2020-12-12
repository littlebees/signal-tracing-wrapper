CC:=g++
prog:=wrapper
obj:=wrapper.o handler.o utils.o

$(prog): $(obj)
	$(CC) -o $@ $(obj)  

*.o: $@.cpp
	$(CC) -c $^ -o $@

.PHONY:clean
clean:
	rm -rf *.o $(prog)