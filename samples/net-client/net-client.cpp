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
  string jsonFile = path.join(path.dirname(process.execPath), "../../samples/node-test/net-cfg.json");
  json cfg = fs.readJsonSync(jsonFile);

  if (cfg["isPipe"].get<bool>()) {
    client_ = net.connect(pipeName, [this]() {
      cout << "pipe client connected to " << pipeName << endl;
      client_->write(Buffer("hello server, i am pipe client."));
    });
  }
  else {
    client_ = net.connect(3000, "127.0.0.1", [this]() {
      cout << "tcp client connected to " << pipeName << endl;
      client_->write(Buffer("hello server, i am tcp client."));
    });
  }

  client_->on<const Error&>("error", [](const Error& err) {
    cout << "client error: " << err.str() << endl;
  });

  client_->on<const Buffer&>("data", [](const Buffer& buf) {
    cout << "recv from svr: " << buf.toString() << endl;
  });

  nodecpp::run();
}

int _tmain(int argc, _TCHAR* argv[])
{
  app.run();
	return 0;
}

