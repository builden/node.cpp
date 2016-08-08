// 双工，继承Readable和Writeable
// 实现_read和_write方法
const Duplex = require('stream').Duplex;
const duplex = new Duplex();
duplex._read = () => {
  this._readNum = this._readNum || 0;
  if (this._readNum > 1) {
    // 读取完毕
    duplex.push(null);
  } else {
    duplex.push('' + (this._readNum++));
  }
};

duplex._write = (buf, enc, next) => {
  // a, b
  process.stdout.write('_write ' + buf.toString() + '\n');
  next();
};

// 0, 1
duplex.on('data', data => console.log('ondata', data.toString()));
duplex.on('finish', () => console.log("DONE"));

duplex.write('a');
duplex.write('b');

// 写入完成，抛finish事件
duplex.end();

// whill throw Error: write after end
// duplex.write('c');
