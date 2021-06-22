OUTPUT = ./bin
TEST = ./test

test: test.o
	gcc -g -o $(OUTPUT)/test $(OUTPUT)/test.o

test.o: $(TEST)/test.c
	gcc -c -g $(TEST)/test.c -o $(OUTPUT)/test.o

clean:
	rm -rf $(OUTPUT)/*