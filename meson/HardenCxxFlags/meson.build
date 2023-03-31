cxx = meson.get_compiler('cpp')
common_flags = [
    '-pedantic-errors',
    '-Wall',
    '-Werror=cast-align',
    '-Werror=conversion',
    '-Werror=format',
    '-Werror=format-nonliteral',
    '-Werror=implicit-fallthrough',
    '-Werror=infinite-recursion',
    '-Werror=parentheses',
    '-Werror=pedantic',
    '-Werror=return-type',
    '-Werror=sign-compare',
    '-Werror=sign-conversion',
    '-Werror=switch',
    '-Werror=tautological-compare',
    '-Werror=uninitialized',
    '-Wextra',
    '-Wnull-dereference',
    '-Wshadow',
    '-Wundef',
]

gcc_flags = [
    '-Wduplicated-branches',
    '-Wduplicated-cond',
    '-Werror=dangling-pointer=2',
    '-Werror=maybe-uninitialized',
    '-Werror=nonnull-compare',
    '-Werror=return-local-addr',
    '-Wformat-signedness',
    '-Wlogical-op',
    '-Wstrict-overflow=4',
]

clang_flags = [
    '-Werror=dangling',
    '-Werror=delete-non-abstract-non-virtual-dtor',
    '-Werror=return-stack-address',
    '-Wfor-loop-analysis',
]

definitions = ['-D_GLIBCXX_ASSERTIONS']

HARDEN_CXX_OPTIONS = cxx.get_supported_arguments(
  common_flags + gcc_flags + clang_flags + definitions
)
