project('GTKTankClient', 'c',
  version : '0.1',
  default_options : ['warning_level=3'])


gtkdep = dependency('gtk+-3.0')
glib = dependency('glib-2.0')
threads = dependency('threads')
gthread = dependency('gthread-2.0')
curl = dependency('libcurl')
gst = dependency('gstreamer-1.0')
dependencies= [gtkdep, glib, threads, gthread, curl, gst]

exe = executable('gtktankclient', [
		'gtktankclient.c',
		'network.c',
		'vehicle.c'
	], dependencies: dependencies,
  install : true)

test('basic', exe)


