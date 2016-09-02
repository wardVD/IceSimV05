cmake_minimum_required(VERSION 2.6.4 FATAL_ERROR)

#
#  A few toplevel customization points, set these as you like
#

#
#   I3_EXTRA_TOOLS: a list of tools to search for.  
#
set(I3_EXTRA_TOOLS photonics)
#
#
#   I3_EXTRA_TOOLS_DIR: For each 'F' in I3_EXTRA_TOOLS, this directory
#   contains 'F.cmake'  These will be found *before* the ones in cmake/tools,
#   so you can override e.g. the 'root' detection by putting your own version here
#
set(I3_EXTRA_TOOLS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/icesim-tools)

#
# Now include the rest of the build.
#
include(cmake/toplevel.cmake NO_POLICY_SCOPE)
