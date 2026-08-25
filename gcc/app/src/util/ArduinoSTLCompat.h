/*
 * ArduinoSTLCompat.h - Repeatable header to shield STL includes from Arduino min/max macros.
 * Intentionally no include guard so it can be used multiple times per translation unit.
 */

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

