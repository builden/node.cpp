const http = require('http');

const url = 'http://localhost:3000';
// const url = 'http://ip.qq.com';
const client = http.get(url, (res) => {
  console.log(res.statusCode);
  console.log(res.headers);
}).setHeader('aaa', 'bbb');

// client;

client.on('error', (err) => {
  console.log('error:', err);
});
