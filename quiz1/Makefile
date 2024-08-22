CC = gcc
CFLAGS = -O0 -g -w

all: main data


OBJS := main.o list.o linux_list.o test.o proc_data.o random.o 
deps := $(OBJS:%.o=.%.o.d)

DATAS := gendata.o proc_data.o random.o 
data_deps := $(DATAS:%.o=.%.o.d)

main: $(OBJS)
	$(CC) -o $@ $(LDFLAGS) $^

data: $(DATAS)
	$(CC) -o $@ $(LDFLAGS) $^

$.o: %.c
	$(CC) -o $@ $(CFLAGS) -c -MMD -MF .$@.d $<

gendata: data
	@./data
	
test: main
	@./main
format:
	clang-format -i *.[ch]

perf: main
	sudo perf record -g ./main
	sudo perf report --symbol-filter=__quick_sort

clean_result:
	rm -f ./result/*.txt
clean:
	rm -f $(OBJS) $(deps) $(DATAS) $(data_deps)