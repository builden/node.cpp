// 转化，继承Duplex
// 可写端写入的数据经转化后自动添加到可读端
// 除了实现_read和_write方法，还需要实现一个_transform方法
const Transform = require('stream').Transform;

const trans = new Transform();
trans._transform = function(buf, enc, next) {
  const res = buf.map((v, i) => {
    v += 1;
    return v;
  });
  this.push(res);
  next();
};

trans.on('data', (data) => console.log('ondata', data.toString()));
trans.on('finish', () => console.log("DONE"));

trans.write('a');
trans.write('b');
trans.end();
