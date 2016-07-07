const spawn = require('child_process').spawn;
const path = require('path');

const args = [1, 'abc', true];
const cwd = path.join(__dirname, '../../Debug');

console.log('cwd:', cwd);
const opts = {
  cwd,
};
const child = spawn('sub-process.exe', args, opts);
child.stdout.on('data', (data) => {
  // data是gbk Buffer
  console.log('stdout:', data.toString());
});

child.stderr.on('data', (data) => {
  console.log('stderr:', data.toString());
});

child.on('close', (code) => {
  console.log(`child process exited with code ${code}`);
});