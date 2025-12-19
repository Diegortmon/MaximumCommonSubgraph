project(
  'mcs_aco',
  'cpp',
  version: '1.0.0',
  default_options: [
    'cpp_std=c++17',
    'warning_level=3',
    'optimization=3',
    'buildtype=release'
  ]
)

omp = dependency('openmp', required: true)

subdir('src')

executable(
  'mcs_aco',
  sources,
  dependencies: [omp],
)
