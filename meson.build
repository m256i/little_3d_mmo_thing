project(
    'proj',
    ['cpp'],
    version: '0.1',
    default_options: [
        'warning_level=3',
        'cpp_std=c++20',
        'backend=ninja'
    ]
)
sources = run_command('py', 'globber.py', check : true).stdout().strip().split('\n')

message('source files: ')
foreach f : sources
  message('   ' + f)
endforeach

cmake             = import('cmake')

root_path = meson.global_source_root()
lib_path = root_path / 'libs'
include_path = root_path / 'include'
headers_path = root_path / 'headers'

useless_warnings = [
'-Wno-unused-variable',
'-Wno-unused-private-field',
'-Wno-gnu-zero-variadic-macro-arguments',
'-Wno-unused-parameter',
'-Wno-extra-tokens',
'-Wno-nested-anon-types',
'-Wno-gnu-anonymous-struct',
'-Wno-language-extension-token'
]

executable(
  'WoWClone',
  [sources],
  include_directories : [include_path, headers_path],
  dependencies : [],
  cpp_args : ['-O3','-march=native', '-fno-math-errno', '-fno-rtti', '-fno-exceptions', useless_warnings],
  link_args : ['-lassimp-vc143-mt', '-lopengl32', '-lglfw3', '-L' + lib_path],
  install : true
)