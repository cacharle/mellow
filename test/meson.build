sources = files(
  'test_malloc.c',
  'test_calloc.c',
  'test_free.c',
  'utils.c',
)
cc = meson.get_compiler('c')
criterion_dep = cc.find_library('criterion', required : true)
test_exec = executable(
  'test_criterion',
  sources,
  # default_options : ['debug'],
  # c_args : ['--coverage'],
  link_with : lib,
  include_directories : incdir,
  dependencies : criterion_dep
)
test('test', test_exec)

