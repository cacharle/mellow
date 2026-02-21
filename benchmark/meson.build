benchmark_sources = files('benchmark.cpp')
# benchmark_inc = include_directories('../include')
benchmark_dep = dependency('benchmark', required : true)

executable(
  'mellow_bench',
  benchmark_sources,
  include_directories : incdir,
  link_with : lib,
  dependencies : benchmark_dep,
  install : false
)
