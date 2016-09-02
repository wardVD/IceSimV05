/**
 * copyright  (C) 2004
 * the icecube collaboration
 * $Id: I3ModuleTestConfig.h 1433 2012-11-04 16:34:42Z nwhitehorn $
 *
 * @file I3ModuleTestConfig.h
 * @version $Revision: 1433 $
 * @date $Date: 2012-11-04 10:34:42 -0600 (Sun, 04 Nov 2012) $
 * @author pretz
 */

#ifndef I3MODULETESTCONFIG_H
#define I3MODULETESTCONFIG_H

/**
 * @brief A simple struct for holding compile-time configurations.
 */
struct I3ModuleTestConfig
{
  /**
   * @brief The maximum number of frames that the system will put out.
   */
  static const int max_frames=20;
};

#endif
