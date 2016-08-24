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
  if (req.url !== '/favicon.ico') {
    console.log('url:', req.url);
    // console.log('headers:', req.headers);
    console.log('method:', req.method);
    console.log('rawHeaders:', req.rawHeaders);
    // console.log(_.keys(req));
  }

  let data;
  req.on('data', chunk => {
    data = chunk;
  });

  req.on('end', () => {
    console.log(req.headers);
    // if (req.headers['content-type'] === 'application/json')
      console.log(data.toString());
  });

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
