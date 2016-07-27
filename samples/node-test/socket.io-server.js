const app = require('http').createServer(handler)
const io = require('socket.io')(app);
const fs = require('fs');

app.listen(3000);

function handler (req, res) {
  res.writeHead(200);
  res.end('hello world');
}

io.on('connection', (socket) => {
  console.log('have one client connected');
  socket.emit('news', { hello: 'i am server' });
  socket.on('client-msg', (data) => {
    console.log('client-msg:', data);
  });
});