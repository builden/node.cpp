const http = require('http');
const fs = require('fs-extra');
const _ = require('lodash');

const cfg = fs.readJsonSync('http-cfg.json');
for (const key of _.keys(cfg)) {
  cfg[key] = fs.readFileSync(cfg[key], 'utf8');
}

const app = http.createServer((req, res) => {
  res.setHeader('Content-Type', 'text/html');
  res.setHeader('X-Foo', 'bar');
  res.writeHead(200, { 'Content-Type': 'text/plain' });
  console.log(req.url);

  if (cfg[req.url]) {
    res.end(cfg[req.url]);
  } else {
    res.end('ok');
  }
});

app.listen(3000);

app.on('error', (err) => {
  console.log('error:', err);
});