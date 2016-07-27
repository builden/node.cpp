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
  string jsonFile = path.join(path.dirname(process.execPath), "../samples/node-test/net-cfg.json");
  json cfg = fs.readJsonSync(jsonFile);

  server_ = net.createServer([this](SocketPtr_t sock) {
    cout << "have one client connected" << endl;
    client_ = sock;
    sock->on<const Buffer&>("data", [this](const Buffer& buf) {
      cout << "recv: " << buf.toString() << endl;

      client_->write(Buffer("welcome to server"));
    });
  });

  if (cfg["isPipe"].get<bool>()) {
    server_->listen(pipeName);
    cout << "pipe server listen at " << pipeName << endl;
  }
  else {
    server_->listen(3000);
    cout << "tcp server listen at port " << 3000 << endl;
  }


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

