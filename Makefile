server:	main.cpp ./Connection/Connection.cpp ./ThreadPool/ThreadPool.cpp ./TimerWheel/TimerWheel.cpp ./WebServer/WebServer.cpp
		g++ -std=c++14 -O2 -g -pthread \
		main.cpp                       \
		./Connection/Connection.cpp    \
		./ThreadPool/ThreadPool.cpp    \
		./TimerWheel/TimerWheel.cpp    \
		./WebServer/WebServer.cpp      \
		-o server

clean:
	rm -f *.o server
