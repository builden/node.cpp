const msbuild = require('./msbuild');
const spawn = require('child_process').spawn;
const iconv = require('iconv-lite');
const path = require('path');
const fs = require('fs');
const s = require('underscore.string');
const log = require('debug')('build-one-conf');
require('colors');

function replaceProjectFile(conf) {
  let buf = fs.readFileSync(conf.targetProj, 'utf-8');
  switch(conf.runtimeLibrary) {
    case 'MultiThreadedDebugDLL':
      buf = s.replaceAll(buf, /(<RuntimeLibrary>)(MultiThreadedDebug)(<\/RuntimeLibrary>)/, `$1${conf.runtimeLibrary}$3`);
    break;
    case 'MultiThreadedDLL':
      buf = s.replaceAll(buf, /(<RuntimeLibrary>)(MultiThreaded)(<\/RuntimeLibrary>)/, `$1${conf.runtimeLibrary}$3`);
    break;
    case 'MultiThreadedDebug':
      buf = s.replaceAll(buf, /(<RuntimeLibrary>)(MultiThreadedDebugDLL)(<\/RuntimeLibrary>)/, `$1${conf.runtimeLibrary}$3`);
    break;
    case 'MultiThreaded':
      buf = s.replaceAll(buf, /(<RuntimeLibrary>)(MultiThreadedDLL)(<\/RuntimeLibrary>)/, `$1${conf.runtimeLibrary}$3`);
    break;
    default: break;
  }
  
  fs.writeFileSync(conf.targetProj, buf);
}

function getSuffixTargetName(conf) {
  switch(conf.runtimeLibrary) {
    case 'MultiThreadedDebugDLL': return '-d';
    case 'MultiThreaded': return '-s';
    case 'MultiThreadedDebug': return '-sd';
    default: return '';
  }
}

function getArgs(conf) {
  const platformDirName = (conf.platform == 'x64') ? 'x64' : 'Win32';
  const platformCfg = `/p:Configuration=${conf.configuration};Platform=${conf.platform};PlatformToolset=v140_xp`;
  const outDirCfg = '/p:OutDIr=' + path.join(__dirname, '..', 'bin', platformDirName, 'libs') + '\\';
  const intDirCfg = '/p:IntDir=' + path.join(__dirname, '..', 'tmp', platformDirName, conf.configuration, conf.runtimeLibrary, conf.targetName) + '\\';
  const targetNameCfg = '/p:TargetName=' + conf.targetName + getSuffixTargetName(conf);
  return [
    conf.targetProj,
    '/m',
    platformCfg,
    outDirCfg,
    intDirCfg,
    targetNameCfg,
  ];
}

function buildOneConf(conf, cb) {
  replaceProjectFile(conf);
  const args = getArgs(conf);
  const child = spawn(msbuild, args);
  const outChunks = [];
  child.stdout.on('data', chunk => {
    outChunks.push(chunk);
  });
  
  child.stderr.on('data', chunk => {
    console.error(iconv.decode(chunk, 'gbk').red);
  });

  child.on('close', (code) => {
    console.log(iconv.decode(Buffer.concat(outChunks), 'gbk'));

    if (code == 0) return cb();
    cb('code ' + code);
  });
}

module.exports = buildOneConf;