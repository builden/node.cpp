const http = require('http');
const app = http.createServer((req, res) => {
  res.setHeader('Content-Type', 'text/html');
  res.setHeader('X-Foo', 'bar');
  res.writeHead(200, { 'Content-Type': 'text/plain' });
  res.end('ok');
});

app.listen(3000);

app.on('error', (err) => {
  console.log('error:', err);
});