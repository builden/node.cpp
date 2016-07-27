const net = require('net');
const pipeName = '\\\\.\\pipe\\sample';
const fs = require('fs-extra');
const cfg = fs.readJsonSync('net-cfg.json');

let client;
if (cfg.isPipe) {
  client = net.connect(pipeName, () => {
    // 'connect' listener
    console.log('connected to pipe server!');
    client.write('pipe world!\r\n');
  });
} else {
  client = net.connect(3000, 'localhost', () => {
    console.log('connected to tcp server!');
    client.write('tcp world!\r\n');
  });
}

client.on('data', (data) => {
  console.log('recv:', data.toString());

  // close connect
  client.end();
});

client.on('end', () => {
  console.log('disconnected from server');
});

client.on('error', (err) => {
  console.log('client error:', err);
});
