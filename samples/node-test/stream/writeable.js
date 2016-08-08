const Writable = require('stream').Writable;
const writable = new Writable();
writable._write = (chunk, encoding, next) => {
  process.stdout.write('_write ' + chunk.toString() + '\n');
  next();
};

writable.write('a');
writable.write('b');
writable.end();

writable.on('finish', () => console.log('ONFINISH'));
writable.on('end', () => console.log('ONEND'));