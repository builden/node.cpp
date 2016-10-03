const fs = require('fs');
const os = require('os');
const path = require('path');
const _ = require('lodash');
const async = require('async');
const buildOneConf = require('./build-one-conf');
const s = require('underscore.string');

function replaceOutputFile(proj) {
  let buf = fs.readFileSync(proj.targetProj, 'utf-8');
  buf = s.replaceAll(buf, /<OutputFile>.*<\/OutputFile>/, '<OutputFile>$(OutDir)$(TargetName)$(TargetExt)<\/OutputFile>');
  fs.writeFileSync(proj.targetProj, buf);
}

function getTargetName(proj) {
  if (proj.targetName) return proj.targetName;
  const extname = path.extname(proj.targetProj);
  return path.basename(proj.targetProj, extname);
}

function createOneConf(tmpl, platform, configuration, runtimeLibrary) {
  return _.assignIn(_.clone(tmpl), {
    platform,
    configuration,
    runtimeLibrary,
  });
}

function getConfs(proj) {
  const targetName = getTargetName(proj);
  const tmpl = {
    targetProj: proj.targetProj,
    targetName: targetName,
    targetType: proj.targetType,
  };
  return [
    createOneConf(tmpl, 'x86', 'Debug', 'MultiThreadedDebugDLL'),
    createOneConf(tmpl, 'x86', 'Release', 'MultiThreadedDLL'),
    createOneConf(tmpl, 'x86', 'Debug', 'MultiThreadedDebug'),
    createOneConf(tmpl, 'x86', 'Release', 'MultiThreaded'),
    
    // createOneConf(tmpl, 'x64', 'Debug', 'MultiThreadedDebugDLL'),
    // createOneConf(tmpl, 'x64', 'Release', 'MultiThreadedDLL'),
    // createOneConf(tmpl, 'x64', 'Debug', 'MultiThreadedDebug'),
    // createOneConf(tmpl, 'x64', 'Release', 'MultiThreaded'),
  ];
}

function buildOneProj(proj, cb) {
  replaceOutputFile(proj);
  // os.cpus().length
  const confs = getConfs(proj);
  async.eachOfLimit(confs, 1, (conf, idx, cb1) => {
    buildOneConf(conf, cb1);
  }, cb);
}

module.exports = buildOneProj;