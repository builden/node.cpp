const net = require('net');
const pipeName = '\\\\.\\pipe\\sample';

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
});

server.on('error', (err) => {
  console.log('server error:', err);
  throw err;
});

server.listen(pipeName, () => {
  console.log('server bound');
});