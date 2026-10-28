/*
  Force-included into the JUCE module translation units only (not ISSE's own
  sources). JUCE 3.0.0 renames "Point" and "Component" while importing Apple
  headers, because old Carbon headers declare global types with those names.
  Modern macOS SDKs pull CarbonCore / QuickDraw in indirectly (e.g. through
  Accelerate -> CoreVideo -> ApplicationServices) at points outside JUCE's
  rename blocks, which then fail with "unknown type name 'Point'".

  Importing those umbrella headers once, up front and under the rename, makes
  every later inclusion a no-op thanks to their include guards.
*/
#pragma once

#define Point CarbonDummyPointName
#define Component CarbonDummyCompName
#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>
#include <ApplicationServices/ApplicationServices.h>
#undef Point
#undef Component
