const http = require('http');
const app = http.createServer((req, res) => {
  res.writeHead(200);
  res.end('hello world');
});
const io = require('socket.io')(app);

app.listen(3000);

io.on('connection', (socket) => {
  console.log('have one client connected');
  socket.emit('news', { hello: 'i am server' });
  socket.on('client-msg', (data) => {
    console.log('client-msg:', data);
  });
});
