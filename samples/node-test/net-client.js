const net = require('net');
const pipeName = '\\\\.\\pipe\\sample';

const client = net.connect(pipeName, () => {
  // 'connect' listener
  console.log('connected to server!');
  client.write('world!\r\n');
});

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