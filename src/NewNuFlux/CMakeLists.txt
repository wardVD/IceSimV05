
i3_project(NewNuFlux)

add_definitions("-DICETRAY")

i3_add_library(NewNuFlux
    private/NewNuFlux/FluxFunction.cpp
    private/NewNuFlux/Fluxes/LegacyConventionalFlux.cpp
    private/NewNuFlux/Fluxes/LegacyPromptFlux.cpp
    private/NewNuFlux/Fluxes/ANFlux.cpp
    private/NewNuFlux/Fluxes/LEFlux.cpp
    private/NewNuFlux/Fluxes/IPLEFlux.cpp
    private/NewNuFlux/Fluxes/SplineFlux.cpp
    USE_PROJECTS serialization icetray dataclasses photospline
    USE_TOOLS boost python
)

SET(NewNuFlux_pybindings_tools boost python)
IF(NUMPY_FOUND)
    add_definitions("-DUSE_NUMPY")
    LIST(APPEND NewNuFlux_pybindings_tools numpy)
ENDIF(NUMPY_FOUND)

i3_add_pybindings(NewNuFlux
    private/pybindings/module.cxx
    USE_PROJECTS NewNuFlux
    USE_TOOLS ${NewNuFlux_pybindings_tools}
)
