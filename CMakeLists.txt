project( kcometen4.kss )

find_package( KDE4 REQUIRED )
find_package( OpenGL REQUIRED )

set( CMAKE_INCLUDE_CURRENT_DIR TRUE )

add_definitions( ${QT_DEFINITIONS} ${KDE4_DEFINITIONS} )

set ( libGL_FIND_REQUIRED TRUE )
set ( libGLU_FIND_REQUIRED TRUE )
set ( QtOpenGL_FIND_REQUIRED TRUE )
set ( kscreensaver_FIND_REQUIRED TRUE )

find_path( KSCREENSAVER_INCLUDE_DIR kscreensaver.h ${KDE4_INCLUDE_DIR} )
find_library( KSCREENSAVER_LIBRARY NAMES kscreensaver PATHS ${KDE4_LIB_DIR} )

include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( kscreensaver DEFAULT_MSG KSCREENSAVER_LIBRARY KSCREENSAVER_INCLUDE_DIR )
find_package_handle_standard_args( libGL DEFAULT_MSG OPENGL_gl_LIBRARY OPENGL_INCLUDE_DIR )
find_package_handle_standard_args( libGLU DEFAULT_MSG OPENGL_glu_LIBRARY OPENGL_INCLUDE_DIR )
find_package_handle_standard_args( QtOpenGL DEFAULT_MSG QT_QTOPENGL_LIBRARY QT_QTOPENGL_INCLUDE_DIR )

add_subdirectory( src )
