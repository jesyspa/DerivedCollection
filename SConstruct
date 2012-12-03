env = Environment(CCFLAGS='-O0 -g -Wall -Wextra -pedantic -Werror -std=c++11')
env.Append(CPPDEFINES='_GLIBCXX_DEBUG')
env.Append(CXX='-4.8')
env.Program(target="test", source=Glob('*.cpp'), LIBS=['boost_unit_test_framework'])
