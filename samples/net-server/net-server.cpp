// net-server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

string pipeName = R"(\\.\pipe\sample)";

class App : public Singleton<App> {
public:
  void run();

private:
  ServerPtr_t server_;
  SocketPtr_t client_;
};

App& app = App::instance();

void App::run() {
  server_ = net.createServer([this](SocketPtr_t sock) {
    cout << "have one client connected" << endl;
    client_ = sock;
    sock->on<const Buffer&>("data", [this](const Buffer& buf) {
      cout << buf.format() << endl;

      client_->write(Buffer("welcome to server"));
    });
  });

  server_->listen(pipeName);
  cout << "server listen at " << pipeName << endl;

  server_->on<const Error&>("error", [](const Error& err) {
    cout << "server err: " << err.str() << endl;
  });

  nodecpp::run();
}

int _tmain(int argc, _TCHAR* argv[])
{
  app.run();
	return 0;
}

