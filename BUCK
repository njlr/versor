cxx_library(
  name = 'versor',
  header_namespace = '',
  exported_headers = subdir_glob([
    ('include', '**/*.h'),
  ]),
  headers = subdir_glob([
    ('src', '**/*.h'),
    ('include/vsr', '**/*.h'),
  ]),
  srcs = glob([
    'src/**/*.cpp',
  ], excludes = glob([
    'src/draw/**/*.cpp',
    'src/z_deprecated/**/*.cpp',
  ])),
  visibility = [
    'PUBLIC',
  ],
)
