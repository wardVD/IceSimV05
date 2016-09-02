#
# Try to find GEANT4 visualization drivers
# and set corresponding compiler flags
#
    
if(EXISTS ${GEANT4_INC_DIR}/G4VisManager.hh)
   message(STATUS "  + visualization")
   set(GEANT4_CXX_FLAGS "-DG4VIS_USE")
       			    
   if(EXISTS ${GEANT4_INC_DIR}/G4FukuiRenderer.hh)
      # message(STATUS "  + DAWN")
      #set(GEANT4_CXX_FLAGS "${GEANT4_CXX_FLAGS} -DG4VIS_USE_DAWN")
   endif(EXISTS ${GEANT4_INC_DIR}/G4FukuiRenderer.hh)
    
   if(EXISTS ${GEANT4_INC_DIR}/G4OpenGLImmediateX.hh)
      # message(STATUS "  + OpenGL ImmediateX")
      #set(GEANT4_CXX_FLAGS "${GEANT4_CXX_FLAGS} -DG4VIS_USE_OPENGLX")
   endif(EXISTS ${GEANT4_INC_DIR}/G4OpenGLImmediateX.hh)
      
   if(EXISTS ${GEANT4_INC_DIR}/G4OpenGLImmediateXm.hh)
      # message(STATUS "  + OpenGL ImmediateXm")
      #set(GEANT4_CXX_FLAGS "${GEANT4_CXX_FLAGS} -DG4VIS_USE_OPENGLXM")
   endif(EXISTS ${GEANT4_INC_DIR}/G4OpenGLImmediateXm.hh)
       
   if(EXISTS ${GEANT4_INC_DIR}/G4OpenGLImmediateQt.hh)
      # message(STATUS "  + OpenGL ImmediateQt")
      # set(GEANT4_CXX_FLAGS "${GEANT4_CXX_FLAGS} -DG4VIS_USE_OPENGLQT")
   endif(EXISTS ${GEANT4_INC_DIR}/G4OpenGLImmediateQt.hh)
       
   if(EXISTS ${GEANT4_INC_DIR}/G4OpenInventorX.hh)
      # message(STATUS "  + OpenGInventorX")
      # set(GEANT4_CXX_FLAGS "${GEANT4_CXX_FLAGS} -DG4VIS_USE_OIX")
   endif(EXISTS ${GEANT4_INC_DIR}/G4OpenInventorX.hh)
              
   if(EXISTS ${GEANT4_INC_DIR}/G4RayTracerX.hh)
      # message(STATUS "  + RayTracerX")
      #set(GEANT4_CXX_FLAGS "${GEANT4_CXX_FLAGS} -DG4VIS_USE_RAYTRACERX")
   endif(EXISTS ${GEANT4_INC_DIR}/G4RayTracerX.hh)
       
   if(EXISTS ${GEANT4_INC_DIR}/G4VRML1.hh)
      # message(STATUS "  + VRML")
      #set(GEANT4_CXX_FLAGS "${GEANT4_CXX_FLAGS} -DG4VIS_USE_VRML")
   endif(EXISTS ${GEANT4_INC_DIR}/G4VRML1.hh)
   
   message(STATUS "    ${GEANT4_CXX_FLAGS}")
   add_definitions(${GEANT4_CXX_FLAGS})

else(EXISTS ${GEANT4_INC_DIR}/G4VisManager.hh)
   
   message(STATUS "  + no visualization")

endif(EXISTS ${GEANT4_INC_DIR}/G4VisManager.hh)
