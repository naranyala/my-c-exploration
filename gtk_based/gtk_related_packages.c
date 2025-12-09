
#include <gtk/gtk.h>
#include <stdarg.h>
#include <string.h>

// Structure to hold package information
typedef struct {
  const char *name;
  const char *description;
  const char **features;
  int feature_count;
  GtkWidget *expander;
  GtkWidget *feature_box;
} PackageItem;

// ========== Graphics & GUI ==========
static const char *gtk_features[] = {
    "Cross-platform GUI toolkit", "Widget-based architecture",
    "CSS theming support", "Accessibility built-in",
    "Hardware acceleration support"};

static const char *glib_features[] = {
    "Core utilities and data structures", "GObject type system",
    "Event loop and main context", "File and I/O operations",
    "Threading support"};

static const char *cairo_features[] = {
    "2D vector graphics library", "Multiple output targets",
    "Anti-aliased rendering", "Complex path operations", "PDF and SVG export"};

static const char *pango_features[] = {
    "Text layout and rendering", "Unicode support", "Font handling",
    "Bidirectional text support", "Complex text shaping"};

// ========== Game Development ==========
static const char *raylib_features[] = {
    "Simple and easy-to-use game library",
    "Hardware-accelerated 2D/3D rendering",
    "Audio support (WAV, OGG, MP3)",
    "Physics support (with raylib-physics)",
    "Single-header option available",
    "Cross-platform (Windows, Linux, macOS, Web)"};

static const char *sdl2_features[] = {
    "Low-level access to audio, keyboard, mouse, joystick, and graphics",
    "Hardware-accelerated 2D rendering", "OpenGL and Vulkan support",
    "Game controller and haptic feedback support",
    "Vast ecosystem of extensions"};

static const char *glfw_features[] = {
    "Window and input management", "OpenGL and Vulkan context creation",
    "Monitor and video mode handling", "Clipboard support",
    "Minimal dependencies"};

static const char *openal_features[] = {
    "Cross-platform 3D audio API", "Environmental audio effects",
    "Streaming audio support", "Multi-channel output"};

// ========== 2D Graphics ==========
static const char *nanovg_features[] = {
    "Anti-aliased 2D vector drawing", "OpenGL backend", "Single-header library",
    "Similar API to HTML5 Canvas", "Font and image support"};

static const char *stb_image_features[] = {
    "Single-header image loader", "Supports JPEG, PNG, TGA, BMP, PSD",
    "No external dependencies", "Very lightweight",
    "Per-file public domain license"};

// ========== 3D Graphics ==========
static const char *assimp_features[] = {
    "3D model import library", "Supports 40+ file formats",
    "Scene post-processing", "Mesh optimization", "Animation support"};

// ========== Mathematics ==========
static const char *cglm_features[] = {
    "Optimized OpenGL Mathematics (GLM) library for C",
    "Highly optimized for performance", "SIMD support where available",
    "Single-header option", "No dependencies"};

// ========== Physics ==========
static const char *chipmunk_features[] = {
    "Fast and lightweight 2D rigid body physics library", "Simple C API",
    "Collision detection", "Constraints and joints", "Sleep management"};

static const char *box2d_features[] = {
    "2D physics engine for games", "Continuous collision detection",
    "Various body types (dynamic, static, kinematic)",
    "Joints, motors, and sensors", "C++ with C API available"};

// ========== Audio Processing ==========
static const char *miniaudio_features[] = {
    "Single-header audio playback and capture library",
    "Supports WAV, FLAC, MP3, Vorbis", "Filtering and mixing", "Spatial audio",
    "No external dependencies"};

static const char *dr_libs_features[] = {
    "Collection of single-file audio libraries", "dr_wav: WAV audio loader",
    "dr_flac: FLAC audio loader", "dr_mp3: MP3 audio loader",
    "dr_libs: Miscellaneous utilities"};

// ========== Networking ==========
static const char *enet_features[] = {
    "Reliable UDP networking library", "Designed for games",
    "Connection management", "Packet fragmentation and reassembly",
    "Minimal overhead"};

static const char *libuv_features[] = {
    "Asynchronous I/O library", "Event-driven architecture",
    "Filesystem operations", "Threading and synchronization",
    "Used by Node.js"};

// ========== Data Formats ==========
static const char *cjson_features[] = {"Ultralightweight JSON parser in ANSI C",
                                       "Single header file", "No dependencies",
                                       "Simple API", "MIT licensed"};

static const char *yaml_features[] = {
    "LibYAML: YAML parser and emitter", "Streaming API", "Canonical output",
    "Anchor and alias support", "Cross-platform"};

// ========== Compression ==========
static const char *zlib_features[] = {
    "Massively spiffy yet delicately unobtrusive compression library",
    "DEFLATE compression algorithm", "gzip and PKZIP support", "Cross-platform",
    "Extremely widely used"};

static const char *miniz_features[] = {
    "Single-file lossless compression library", "ZIP archive reading/writing",
    "DEFLATE compression", "No external dependencies", "Public domain"};

// ========== Utility Libraries ==========
static const char *stb_libs_features[] = {
    "Collection of single-file public domain libraries",
    "stb_image: Image loading", "stb_truetype: TrueType font rendering",
    "stb_vorbis: Ogg Vorbis audio", "stb_ds: Data structures"};

static const char *klib_features[] = {
    "Lightweight and efficient algorithms and data structures",
    "Hash tables, B-trees, sort algorithms", "Single-header implementations",
    "No dependencies", "Highly portable"};

// ========== Testing ==========
static const char *unity_features[] = {"Simple unit testing framework for C",
                                       "Single test runner", "Assertion macros",
                                       "Test fixtures", "Cross-platform"};

// ========== Package Array ==========
static PackageItem packages[] = {
    // Core GTK Stack
    {"GTK+ (GIMP Toolkit)",
     "Modern cross-platform toolkit for creating graphical user interfaces",
     gtk_features, 5, NULL, NULL},
    {"GLib", "Low-level core library that forms the basis of GTK",
     glib_features, 5, NULL, NULL},
    {"Cairo", "2D graphics library with support for multiple output devices",
     cairo_features, 5, NULL, NULL},
    {"Pango",
     "Library for laying out and rendering text with emphasis on "
     "internationalization",
     pango_features, 5, NULL, NULL},

    // Game Development
    {"Raylib", "Simple and easy-to-use library for game development",
     raylib_features, 6, NULL, NULL},
    {"SDL2", "Simple DirectMedia Layer - cross-platform multimedia library",
     sdl2_features, 5, NULL, NULL},
    {"GLFW", "Multi-platform library for OpenGL, OpenGL ES and Vulkan",
     glfw_features, 5, NULL, NULL},
    {"OpenAL", "Cross-platform 3D audio API", openal_features, 4, NULL, NULL},

    // 2D Graphics
    {"NanoVG", "Small antialiased vector graphics rendering library",
     nanovg_features, 5, NULL, NULL},
    {"stb_image", "Single-header image loading library", stb_image_features, 5,
     NULL, NULL},

    // 3D Graphics
    {"Assimp", "Open Asset Import Library for 3D models", assimp_features, 5,
     NULL, NULL},

    // Mathematics
    {"cglm", "Highly optimized graphics math library for C", cglm_features, 5,
     NULL, NULL},

    // Physics
    {"Chipmunk", "Fast and lightweight 2D rigid body physics library",
     chipmunk_features, 5, NULL, NULL},
    {"Box2D", "2D physics engine for games", box2d_features, 5, NULL, NULL},

    // Audio
    {"miniaudio", "Single-header audio playback and capture library",
     miniaudio_features, 5, NULL, NULL},
    {"dr_libs", "Collection of single-file audio libraries", dr_libs_features,
     5, NULL, NULL},

    // Networking
    {"ENet", "Reliable UDP networking library designed for games",
     enet_features, 5, NULL, NULL},
    {"libuv", "Cross-platform asynchronous I/O library", libuv_features, 5,
     NULL, NULL},

    // Data Formats
    {"cJSON", "Ultralightweight JSON parser in ANSI C", cjson_features, 5, NULL,
     NULL},
    {"LibYAML", "YAML parser and emitter library", yaml_features, 5, NULL,
     NULL},

    // Compression
    {"zlib", "General purpose data compression library", zlib_features, 5, NULL,
     NULL},
    {"miniz", "Single-file lossless compression library", miniz_features, 5,
     NULL, NULL},

    // Utilities
    {"stb libraries", "Collection of single-file public domain libraries",
     stb_libs_features, 5, NULL, NULL},
    {"klib", "Lightweight and efficient algorithms library", klib_features, 5,
     NULL, NULL},

    // Testing
    {"Unity", "Simple unit testing framework for C", unity_features, 5, NULL,
     NULL},

    // Original packages (kept for compatibility)
    {"GdkPixbuf", "Library for image loading and pixel buffer manipulation",
     NULL, 0, NULL, NULL},
    {"JSON-GLib",
     "Library providing serialization and deserialization for JSON", NULL, 0,
     NULL, NULL},
    {"SQLite", "Self-contained, serverless SQL database engine", NULL, 0, NULL,
     NULL},
    {"libsoup", "HTTP client/server library for GNOME", NULL, 0, NULL, NULL}};
