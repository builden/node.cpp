const net = require('net');
const fs = require('fs-extra');
const pipeName = '\\\\.\\pipe\\sample';
const cfg = fs.readJsonSync('net-cfg.json');

const server = net.createServer((c) => {
  // 'connection' listener
  console.log('client connected');
  c.on('data', (data) => {
    console.log('recv:', data.toString());
  });

  c.on('end', () => {
    console.log('client disconnected');
  });

  c.write('hello\r\n');

  // send back
  c.pipe(c);

  c.on('error', (err) => {
    console.log('c error', err);
  });
});

server.on('error', (err) => {
  console.log('server error:', err);
  throw err;
});

if (cfg.isPipe) {
  server.listen(pipeName, () => {
    console.log('pipe server bound');
  });
} else {
  server.listen(3000, 'localhost', () => {
    console.log('tcp server bound');
  });
}
