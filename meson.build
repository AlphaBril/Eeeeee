project('eeeeee', 'c',
  version : '1.0.0',
  default_options : ['warning_level=3',
                     'c_std=c99'])

# Dependencies
gtk_dep = dependency('gtk4')
libevdev_dep = dependency('libevdev')
gio_dep = dependency('gio-2.0')

# Include directories
inc_dir = include_directories('headers')

# Source files
sources = [
  'src/main.c',
  'src/config.c',
  'src/keyboard.c',
  'src/events.c',
  'src/ui.c'
]

# Executable
executable('eeeeee',
  sources,
  dependencies : [gtk_dep, libevdev_dep, gio_dep],
  include_directories : inc_dir,
  install : true)

# Optional: install configuration
install_data('README.md', install_dir : get_option('datadir') / 'doc' / 'eeeeee')