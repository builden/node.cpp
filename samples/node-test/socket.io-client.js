const io = require('socket.io-client');
const client = io.connect('http://localhost:3000');

client.on('news', (data) => {
  console.log('news:', data);
});

client.on('connect', () => {
  console.log('connect succ');
  client.emit('client-msg', { hello: 'i am client'});
});

client.on('close', () => {
  console.log('close event');
});