/*
 * TinyPhys3D - A simple single-header 3D physics library
 * 
 * Usage:
 *   #define TINYPHYS_IMPLEMENTATION
 *   #include "tinyphys_3d.h"
 * 
 * Features:
 *   - Basic rigid body dynamics
 *   - Sphere and box collision detection
 *   - Impulse-based collision response
 *   - Simple gravity and friction
 */

#ifndef TINYPHYS_3D_H
#define TINYPHYS_3D_H

#include <math.h>
#include <string.h>

/* Vector3 */
typedef struct {
    float x, y, z;
} tp3d_vec3;

/* Quaternion for rotation */
typedef struct {
    float x, y, z, w;
} tp3d_quat;

/* Shape types */
typedef enum {
    TP3D_SHAPE_SPHERE,
    TP3D_SHAPE_BOX
} tp3d_shape_type;

/* Collision shape */
typedef struct {
    tp3d_shape_type type;
    union {
        float radius;              /* For sphere */
        tp3d_vec3 half_extents;    /* For box */
    };
} tp3d_shape;

/* Rigid body */
typedef struct {
    tp3d_vec3 position;
    tp3d_vec3 velocity;
    tp3d_vec3 acceleration;
    tp3d_quat rotation;
    tp3d_vec3 angular_velocity;
    
    float mass;
    float inv_mass;
    float restitution;    /* Bounciness (0-1) */
    float friction;
    
    tp3d_shape shape;
    int is_static;        /* Static bodies don't move */
} tp3d_body;

/* Collision info */
typedef struct {
    tp3d_body* body_a;
    tp3d_body* body_b;
    tp3d_vec3 normal;
    float penetration;
    int is_colliding;
} tp3d_collision;

/* World */
typedef struct {
    tp3d_body* bodies;
    int body_count;
    int body_capacity;
    tp3d_vec3 gravity;
} tp3d_world;

/* API */
tp3d_world* tp3d_create_world(int max_bodies);
void tp3d_destroy_world(tp3d_world* world);
tp3d_body* tp3d_add_body(tp3d_world* world);
void tp3d_step(tp3d_world* world, float dt);

/* Body functions */
void tp3d_body_set_sphere(tp3d_body* body, float radius);
void tp3d_body_set_box(tp3d_body* body, float width, float height, float depth);
void tp3d_body_apply_force(tp3d_body* body, tp3d_vec3 force);
void tp3d_body_apply_impulse(tp3d_body* body, tp3d_vec3 impulse);

/* Vector operations */
tp3d_vec3 tp3d_vec3_add(tp3d_vec3 a, tp3d_vec3 b);
tp3d_vec3 tp3d_vec3_sub(tp3d_vec3 a, tp3d_vec3 b);
tp3d_vec3 tp3d_vec3_scale(tp3d_vec3 v, float s);
float tp3d_vec3_dot(tp3d_vec3 a, tp3d_vec3 b);
tp3d_vec3 tp3d_vec3_cross(tp3d_vec3 a, tp3d_vec3 b);
float tp3d_vec3_length(tp3d_vec3 v);
tp3d_vec3 tp3d_vec3_normalize(tp3d_vec3 v);

#ifdef TINYPHYS_IMPLEMENTATION

#include <stdlib.h>

/* Vector operations */
tp3d_vec3 tp3d_vec3_add(tp3d_vec3 a, tp3d_vec3 b) {
    return (tp3d_vec3){a.x + b.x, a.y + b.y, a.z + b.z};
}

tp3d_vec3 tp3d_vec3_sub(tp3d_vec3 a, tp3d_vec3 b) {
    return (tp3d_vec3){a.x - b.x, a.y - b.y, a.z - b.z};
}

tp3d_vec3 tp3d_vec3_scale(tp3d_vec3 v, float s) {
    return (tp3d_vec3){v.x * s, v.y * s, v.z * s};
}

float tp3d_vec3_dot(tp3d_vec3 a, tp3d_vec3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

tp3d_vec3 tp3d_vec3_cross(tp3d_vec3 a, tp3d_vec3 b) {
    return (tp3d_vec3){
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

float tp3d_vec3_length(tp3d_vec3 v) {
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

tp3d_vec3 tp3d_vec3_normalize(tp3d_vec3 v) {
    float len = tp3d_vec3_length(v);
    if (len > 0.00001f) {
        return tp3d_vec3_scale(v, 1.0f / len);
    }
    return (tp3d_vec3){0, 0, 0};
}

/* World functions */
tp3d_world* tp3d_create_world(int max_bodies) {
    tp3d_world* world = (tp3d_world*)malloc(sizeof(tp3d_world));
    world->bodies = (tp3d_body*)malloc(sizeof(tp3d_body) * max_bodies);
    world->body_count = 0;
    world->body_capacity = max_bodies;
    world->gravity = (tp3d_vec3){0, -9.81f, 0};
    return world;
}

void tp3d_destroy_world(tp3d_world* world) {
    free(world->bodies);
    free(world);
}

tp3d_body* tp3d_add_body(tp3d_world* world) {
    if (world->body_count >= world->body_capacity) return NULL;
    
    tp3d_body* body = &world->bodies[world->body_count++];
    memset(body, 0, sizeof(tp3d_body));
    
    body->mass = 1.0f;
    body->inv_mass = 1.0f;
    body->restitution = 0.5f;
    body->friction = 0.3f;
    body->rotation = (tp3d_quat){0, 0, 0, 1};
    
    return body;
}

/* Body functions */
void tp3d_body_set_sphere(tp3d_body* body, float radius) {
    body->shape.type = TP3D_SHAPE_SPHERE;
    body->shape.radius = radius;
}

void tp3d_body_set_box(tp3d_body* body, float width, float height, float depth) {
    body->shape.type = TP3D_SHAPE_BOX;
    body->shape.half_extents = (tp3d_vec3){width * 0.5f, height * 0.5f, depth * 0.5f};
}

void tp3d_body_apply_force(tp3d_body* body, tp3d_vec3 force) {
    if (body->is_static) return;
    tp3d_vec3 accel = tp3d_vec3_scale(force, body->inv_mass);
    body->acceleration = tp3d_vec3_add(body->acceleration, accel);
}

void tp3d_body_apply_impulse(tp3d_body* body, tp3d_vec3 impulse) {
    if (body->is_static) return;
    tp3d_vec3 vel_change = tp3d_vec3_scale(impulse, body->inv_mass);
    body->velocity = tp3d_vec3_add(body->velocity, vel_change);
}

/* Collision detection */
static int tp3d_check_sphere_sphere(tp3d_body* a, tp3d_body* b, tp3d_collision* col) {
    tp3d_vec3 diff = tp3d_vec3_sub(b->position, a->position);
    float dist = tp3d_vec3_length(diff);
    float sum_radius = a->shape.radius + b->shape.radius;
    
    if (dist < sum_radius) {
        col->body_a = a;
        col->body_b = b;
        col->penetration = sum_radius - dist;
        col->normal = tp3d_vec3_normalize(diff);
        col->is_colliding = 1;
        return 1;
    }
    return 0;
}

static void tp3d_resolve_collision(tp3d_collision* col) {
    tp3d_body* a = col->body_a;
    tp3d_body* b = col->body_b;
    
    /* Separate bodies */
    float total_inv_mass = a->inv_mass + b->inv_mass;
    if (total_inv_mass > 0.0f) {
        tp3d_vec3 correction = tp3d_vec3_scale(col->normal, col->penetration / total_inv_mass);
        if (!a->is_static) {
            a->position = tp3d_vec3_sub(a->position, tp3d_vec3_scale(correction, a->inv_mass));
        }
        if (!b->is_static) {
            b->position = tp3d_vec3_add(b->position, tp3d_vec3_scale(correction, b->inv_mass));
        }
    }
    
    /* Calculate relative velocity */
    tp3d_vec3 rel_vel = tp3d_vec3_sub(b->velocity, a->velocity);
    float vel_along_normal = tp3d_vec3_dot(rel_vel, col->normal);
    
    if (vel_along_normal > 0) return; /* Objects moving apart */
    
    /* Calculate restitution */
    float e = fminf(a->restitution, b->restitution);
    
    /* Calculate impulse scalar */
    float j = -(1.0f + e) * vel_along_normal;
    j /= total_inv_mass;
    
    /* Apply impulse */
    tp3d_vec3 impulse = tp3d_vec3_scale(col->normal, j);
    if (!a->is_static) {
        a->velocity = tp3d_vec3_sub(a->velocity, tp3d_vec3_scale(impulse, a->inv_mass));
    }
    if (!b->is_static) {
        b->velocity = tp3d_vec3_add(b->velocity, tp3d_vec3_scale(impulse, b->inv_mass));
    }
    
    /* Apply friction */
    rel_vel = tp3d_vec3_sub(b->velocity, a->velocity);
    tp3d_vec3 tangent = tp3d_vec3_sub(rel_vel, tp3d_vec3_scale(col->normal, tp3d_vec3_dot(rel_vel, col->normal)));
    float tangent_len = tp3d_vec3_length(tangent);
    if (tangent_len > 0.0001f) {
        tangent = tp3d_vec3_scale(tangent, 1.0f / tangent_len);
        float friction = (a->friction + b->friction) * 0.5f;
        float jt = -tp3d_vec3_dot(rel_vel, tangent) * friction;
        jt /= total_inv_mass;
        
        tp3d_vec3 friction_impulse = tp3d_vec3_scale(tangent, jt);
        if (!a->is_static) {
            a->velocity = tp3d_vec3_sub(a->velocity, tp3d_vec3_scale(friction_impulse, a->inv_mass));
        }
        if (!b->is_static) {
            b->velocity = tp3d_vec3_add(b->velocity, tp3d_vec3_scale(friction_impulse, b->inv_mass));
        }
    }
}

/* Physics step */
void tp3d_step(tp3d_world* world, float dt) {
    /* Integrate forces */
    for (int i = 0; i < world->body_count; i++) {
        tp3d_body* body = &world->bodies[i];
        if (body->is_static) continue;
        
        /* Apply gravity */
        tp3d_vec3 gravity_force = tp3d_vec3_scale(world->gravity, body->mass);
        tp3d_body_apply_force(body, gravity_force);
        
        /* Update velocity */
        body->velocity = tp3d_vec3_add(body->velocity, tp3d_vec3_scale(body->acceleration, dt));
        body->acceleration = (tp3d_vec3){0, 0, 0};
        
        /* Update position */
        body->position = tp3d_vec3_add(body->position, tp3d_vec3_scale(body->velocity, dt));
    }
    
    /* Detect and resolve collisions */
    for (int i = 0; i < world->body_count; i++) {
        for (int j = i + 1; j < world->body_count; j++) {
            tp3d_body* a = &world->bodies[i];
            tp3d_body* b = &world->bodies[j];
            
            if (a->is_static && b->is_static) continue;
            
            tp3d_collision col = {0};
            if (a->shape.type == TP3D_SHAPE_SPHERE && b->shape.type == TP3D_SHAPE_SPHERE) {
                if (tp3d_check_sphere_sphere(a, b, &col)) {
                    tp3d_resolve_collision(&col);
                }
            }
        }
    }
}

#endif /* TINYPHYS_IMPLEMENTATION */
#endif /* TINYPHYS_3D_H */
