project(
    'proj',
    ['cpp', 'c'],
    version: '0.1',
    default_options: [
        'warning_level=3',
        'cpp_std=c++20',
        'backend=ninja'
    ]
)

sources = run_command('python', 'globber.py', check : true).stdout().strip().split('\n')

message('source files: ')
foreach f : sources
  message('   ' + f)
endforeach

cmake           = import('cmake')

meshopt_lib = cmake.subproject('meshoptimizer')
meshopt_target = meshopt_lib.dependency('meshoptimizer')


root_path       = meson.global_source_root()
lib_path        = root_path / 'libs'
include_path    = 'include'
headers_path    = 'headers'

build_platform = build_machine.system()

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

windows_link_args = [
  '-lassimp-vc143-mt',
  '-lassimp-vc143-mt',
  '-lopengl32',
  '-lglfw3',
  '-L' + lib_path,
]

linux_link_args = [

]

applied_link_args = []

if build_platform == 'windows'
  applied_link_args += windows_link_args
  message('------ building for host system: windows ------')
elif build_platform == 'linux'
  applied_link_args += linux_link_args
  message('------ building for host system: linux ------')
endif

executable(
  'WoWClone',
  [sources],
  include_directories : [include_path, headers_path],
  dependencies : [meshopt_target],
  cpp_args : ['-O3', '-g','-march=native', '-fno-rtti', useless_warnings],
  link_args : [windows_link_args],
  install : true
)