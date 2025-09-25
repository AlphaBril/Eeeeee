project('eeeeee', 'c',
  version : '1.0.0',
  default_options : ['warning_level=2',
                     'c_std=c99',
					 'optimization=2'])

add_project_arguments([
  '-Wno-pedantic',
], language : 'c')

gtk_dep = dependency('gtk4')
libevdev_dep = dependency('libevdev')
gio_dep = dependency('gio-2.0')

inc_dir = include_directories('headers')

sources = [
  'src/main.c',
  'src/config.c',
  'src/keyboard.c',
  'src/events.c',
  'src/ui.c'
]

executable('eeeeee',
  sources,
  dependencies : [gtk_dep, libevdev_dep, gio_dep],
  include_directories : inc_dir,
  install : true)

install_data('README.md', install_dir : get_option('datadir') / 'doc' / 'eeeeee')