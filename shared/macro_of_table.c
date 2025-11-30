// Define your table once
#define COLORS_TABLE                                                           \
  X(red, "#FF0000")                                                            \
  X(green, "#00FF00")                                                          \
  X(blue, "#0000FF")

// Enum
#define X(name, hex) COLOR_##name,
typedef enum { COLORS_TABLE } Color;
#undef X

// String array
#define X(name, hex) hex,
const char *color_hex[] = {COLORS_TABLE};
#undef X

// Name array
#define X(name, hex) #name,
const char *color_name[] = {COLORS_TABLE};
#undef X
