include_directories( ${KDE4_INCLUDES} )

set( kcometen4_SRCS bezier.cpp cometen3.cpp cometenmath.cpp cometenscene.cpp
     curvecomet.cpp decal.cpp explosion.cpp glow.cpp kcometen4.cpp
     kglcometen4.cpp lightning.cpp particlesystem.cpp pcomet.cpp
     rotatecomet.cpp settings.cpp vec.cpp )

kde4_add_ui_files( kcometen4_SRCS kcometen4ui.ui )

kde4_add_executable( kcometen4.kss ${kcometen4_SRCS} )

target_link_libraries( kcometen4.kss ${KDE4_KIO_LIBS} ${KSCREENSAVER_LIBRARY} ${QT_QTOPENGL_LIBRARY} ${OPENGL_LIBRARIES} )

install( TARGETS kcometen4.kss ${INSTALL_TARGETS_DEFAULT_ARGS} )
install( FILES lightmap.png particle.png DESTINATION ${DATA_INSTALL_DIR}/kcometen4/pixmaps )
install( FILES kcometen4.desktop DESTINATION ${SERVICES_INSTALL_DIR}/ScreenSavers )
install( FILES kcometen4.kss.1 DESTINATION ${MAN_INSTALL_DIR}/man1 )
