// net-client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

string pipeName = R"(\\.\pipe\sample)";

class App : public Singleton<App> {
public:
  void run();

private:
  SocketPtr_t client_;
};

App& app = App::instance();

void App::run() {
  client_ = net.connect(pipeName, [this]() {
    cout << "client connected to " << pipeName << endl;
  });

  client_->on<const Error&>("error", [](const Error& err) {
    cout << "client error: " << err.str() << endl;
  });

  client_->on<const Buffer&>("data", [](const Buffer& buf) {
    cout << "recv from svr: " << buf.toString() << endl;
  });

  setTimeout([this]() {
    client_->write(Buffer("hello server, i am client."));
  }, 1000);

  nodecpp::run();
}

int _tmain(int argc, _TCHAR* argv[])
{
  app.run();
	return 0;
}

