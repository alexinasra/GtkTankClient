project('GTKTankClient', 'cpp',
  version : '0.1',
  default_options : ['warning_level=3'])


gtkdep = dependency('gtk+-3.0')
glib = dependency('glib-2.0')
threads = dependency('threads')
gthread = dependency('gthread-2.0')
curl = dependency('libcurl')
dependencies= [gtkdep, glib, threads, gthread, curl]
exe = executable('gtktankclient', 'gtktankclient.cpp', dependencies: dependencies,
  install : true)

test('basic', exe)


