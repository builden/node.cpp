const sa = require('superagent');

sa.get('http://127.0.0.1:8080/2.txt')
  .end((err, res) => {
    console.log(res);
  });

// sa.post('http://localhost:3000/xxx')
//   .type('application/x-www-form-urlencoded')
//   .send({a: '中"文a'})
//   .end((err, res) => {
//     console.log(res);
//   });