project(
  'mellow',
  'c',
  default_options : ['c_std=c99'],
  # c_args : ['Wall', 'Wextra']
)
# MAP_ANONYMOUS not defined when using meson
add_global_arguments('-g', language : 'c')
incdir = include_directories('include')
subdir('src')
lib = library('mellow', sources, include_directories : incdir)
subdir('examples')
