all:
	gcc server.c -o server -lpthread
	gcc sender.c -o sender
	gcc receiver.c -o receiver
	gcc launcher.c -o launcher

clean:
	rm launcher
	rm sender
	rm receiver
	rm server
