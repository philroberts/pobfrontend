project('POB Frontend', 'cpp', default_options : ['cpp_std=c++11'])

qt5_dep = dependency('qt5', modules : ['Gui'])
lua_dep = dependency('luajit')
if build_machine.system() == 'darwin'
   gl_dep = dependency('appleframeworks', modules: ['OpenGL'])
else
   gl_dep = dependency('gl')
endif

# Import the extension module that knows how
# to invoke Qt tools.
qt5 = import('qt5')
prep = qt5.preprocess(moc_headers : ['subscript.hpp', 'pobwindow.hpp'])

executable('pobfrontend',
  sources : ['main.cpp', prep],
  dependencies : [qt5_dep, lua_dep, gl_dep])
