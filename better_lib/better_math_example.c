#include <stdio.h>
#include "better_math.h"

int main(void) {
    /* ── Clamping ─────────────────────────────────────────────── */
    double health = 150.0;
    health = clamp(health, 0.0, 100.0);
    printf("Health clamped: %.1f\n", health);  // 100.0

    int volume = -5;
    volume = clampi(volume, 0, 100);
    printf("Volume clamped: %d\n", volume);    // 0

    /* ── Linear Interpolation ─────────────────────────────────── */
    double start_pos = 10.0, end_pos = 50.0;
    
    for (double t = 0.0; t <= 1.0; t += 0.25) {
        double pos = lerp(start_pos, end_pos, t);
        printf("t=%.2f -> position=%.1f\n", t, pos);
    }
    // t=0.00 -> position=10.0
    // t=0.25 -> position=20.0
    // ...

    /* ── Inverse Lerp & Remap ─────────────────────────────────── */
    double temp_celsius = 25.0;
    double temp_fahrenheit = remap(temp_celsius, 0.0, 100.0, 32.0, 212.0);
    printf("%.1f°C = %.1f°F\n", temp_celsius, temp_fahrenheit);  // 77.0°F

    // Where does 75 fall in range [0, 100]?
    double progress = inv_lerp(0.0, 100.0, 75.0);
    printf("75 is %.0f%% through [0,100]\n", progress * 100);  // 75%

    /* ── Angle Conversions ────────────────────────────────────── */
    double angle_deg = 45.0;
    double angle_rad = deg2rad(angle_deg);
    printf("%.1f° = %.4f rad\n", angle_deg, angle_rad);

    // Normalize crazy angles
    double weird_angle = -450.0;
    printf("%.1f° normalized = %.1f°\n", weird_angle, normalize_deg(weird_angle));
    // -450.0° normalized = 270.0°

    /* ── Smooth Animations ────────────────────────────────────── */
    printf("\nLinear vs Smooth interpolation:\n");
    for (double t = 0.0; t <= 1.0; t += 0.2) {
        double linear = lerp(0.0, 100.0, t);
        double smooth = lerp(0.0, 100.0, smoothstep(0.0, 1.0, t));
        printf("  t=%.1f: linear=%5.1f, smooth=%5.1f\n", t, linear, smooth);
    }

    /* ── Float Comparison ─────────────────────────────────────── */
    double a = 0.1 + 0.2;
    double b = 0.3;
    printf("\n0.1 + 0.2 == 0.3? %s\n", (a == b) ? "true" : "false");  // false!
    printf("approx_eq?        %s\n", approx_eq(a, b, 1e-9) ? "true" : "false");  // true

    /* ── NaN Safety ───────────────────────────────────────────── */
    double bad_value = 0.0 / 0.0;  // NaN
    double safe = clamp(bad_value, 0.0, 100.0);
    printf("\nNaN clamped to: %.1f\n", safe);  // 0.0 (returns min)

    return 0;
}
