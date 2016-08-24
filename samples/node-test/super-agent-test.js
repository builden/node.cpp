const sa = require('superagent');

// sa.get('http://localhost:3000/xxx')
//   .end((err, res) => {
//     console.log(res);
//   });

// sa.post('http://localhost:3000/xxx')
//   .type('application/x-www-form-urlencoded')
//   .send({a: '中"文a'})
//   .end((err, res) => {
//     console.log(res);
//   });

const qs = require('querystring');
qs.stringify({a: '中文a'});