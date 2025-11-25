// shapes3d.h - Single-header 3D shapes library (C-compatible)
#ifndef SHAPES3D_H
#define SHAPES3D_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Ensure M_PI is defined
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// 3D Point structure
typedef struct {
    double x, y, z;
} Point3D;

// Shape types
typedef enum {
    SHAPE3D_POINT,
    SHAPE3D_SPHERE,
    SHAPE3D_CUBE,
    SHAPE3D_CYLINDER,
    SHAPE3D_CONE,
    SHAPE3D_PYRAMID,
    SHAPE3D_TETRAHEDRON
} Shape3DType;

// Generic shape structure
typedef struct {
    Shape3DType type;
    void* data;
} Shape3D;

// Shape-specific data structures
typedef struct {
    double radius;
    Point3D center;
} SphereData;

typedef struct {
    double width, height, depth;
    Point3D center;
} CubeData;

typedef struct {
    double radius;
    double height;
    Point3D base_center;
} CylinderData;

typedef struct {
    double radius;
    double height;
    Point3D base_center;
} ConeData;

typedef struct {
    double base_width;
    double base_depth;
    double height;
    Point3D base_center;
} PyramidData;

typedef struct {
    Point3D a, b, c, d;
} TetrahedronData;

// Function prototypes
Point3D make_point3d(double x, double y, double z);
Shape3D* create_point3d(Point3D p);
Shape3D* create_sphere(Point3D center, double radius);
Shape3D* create_cube(Point3D center, double width, double height, double depth);
Shape3D* create_cylinder(Point3D base_center, double radius, double height);
Shape3D* create_cone(Point3D base_center, double radius, double height);
Shape3D* create_pyramid(Point3D base_center, double base_width, double base_depth, double height);
Shape3D* create_tetrahedron(Point3D a, Point3D b, Point3D c, Point3D d);
double shape3d_volume(const Shape3D* shape);
double shape3d_surface_area(const Shape3D* shape);
void print_shape3d(const Shape3D* shape);
void destroy_shape3d(Shape3D* shape);
int is_point_in_shape3d(const Shape3D* shape, Point3D p);

// Implementation
#ifdef SHAPES3D_IMPLEMENTATION

// Helper for tetrahedron surface area
static double tetra_triangle_area(Point3D p1, Point3D p2, Point3D p3) {
    double abx = p2.x - p1.x, aby = p2.y - p1.y, abz = p2.z - p1.z;
    double acx = p3.x - p1.x, acy = p3.y - p1.y, acz = p3.z - p1.z;
    
    // Cross product AB × AC
    double cpx = aby * acz - abz * acy;
    double cpy = abz * acx - abx * acz;
    double cpz = abx * acy - aby * acx;
    
    return 0.5 * sqrt(cpx * cpx + cpy * cpy + cpz * cpz);
}

Point3D make_point3d(double x, double y, double z) {
    Point3D p = {x, y, z};
    return p;
}

Shape3D* create_point3d(Point3D p) {
    Shape3D* shape = malloc(sizeof(Shape3D));
    if (!shape) return NULL;
    
    Point3D* data = malloc(sizeof(Point3D));
    if (!data) {
        free(shape);
        return NULL;
    }
    
    *data = p;
    shape->type = SHAPE3D_POINT;
    shape->data = data;
    return shape;
}

Shape3D* create_sphere(Point3D center, double radius) {
    if (radius < 0) return NULL;
    
    Shape3D* shape = malloc(sizeof(Shape3D));
    if (!shape) return NULL;
    
    SphereData* data = malloc(sizeof(SphereData));
    if (!data) {
        free(shape);
        return NULL;
    }
    
    data->center = center;
    data->radius = radius;
    shape->type = SHAPE3D_SPHERE;
    shape->data = data;
    return shape;
}

Shape3D* create_cube(Point3D center, double width, double height, double depth) {
    if (width < 0 || height < 0 || depth < 0) return NULL;
    
    Shape3D* shape = malloc(sizeof(Shape3D));
    if (!shape) return NULL;
    
    CubeData* data = malloc(sizeof(CubeData));
    if (!data) {
        free(shape);
        return NULL;
    }
    
    data->center = center;
    data->width = width;
    data->height = height;
    data->depth = depth;
    shape->type = SHAPE3D_CUBE;
    shape->data = data;
    return shape;
}

Shape3D* create_cylinder(Point3D base_center, double radius, double height) {
    if (radius < 0 || height < 0) return NULL;
    
    Shape3D* shape = malloc(sizeof(Shape3D));
    if (!shape) return NULL;
    
    CylinderData* data = malloc(sizeof(CylinderData));
    if (!data) {
        free(shape);
        return NULL;
    }
    
    data->base_center = base_center;
    data->radius = radius;
    data->height = height;
    shape->type = SHAPE3D_CYLINDER;
    shape->data = data;
    return shape;
}

Shape3D* create_cone(Point3D base_center, double radius, double height) {
    if (radius < 0 || height < 0) return NULL;
    
    Shape3D* shape = malloc(sizeof(Shape3D));
    if (!shape) return NULL;
    
    ConeData* data = malloc(sizeof(ConeData));
    if (!data) {
        free(shape);
        return NULL;
    }
    
    data->base_center = base_center;
    data->radius = radius;
    data->height = height;
    shape->type = SHAPE3D_CONE;
    shape->data = data;
    return shape;
}

Shape3D* create_pyramid(Point3D base_center, double base_width, double base_depth, double height) {
    if (base_width < 0 || base_depth < 0 || height < 0) return NULL;
    
    Shape3D* shape = malloc(sizeof(Shape3D));
    if (!shape) return NULL;
    
    PyramidData* data = malloc(sizeof(PyramidData));
    if (!data) {
        free(shape);
        return NULL;
    }
    
    data->base_center = base_center;
    data->base_width = base_width;
    data->base_depth = base_depth;
    data->height = height;
    shape->type = SHAPE3D_PYRAMID;
    shape->data = data;
    return shape;
}

Shape3D* create_tetrahedron(Point3D a, Point3D b, Point3D c, Point3D d) {
    Shape3D* shape = malloc(sizeof(Shape3D));
    if (!shape) return NULL;
    
    TetrahedronData* data = malloc(sizeof(TetrahedronData));
    if (!data) {
        free(shape);
        return NULL;
    }
    
    data->a = a; data->b = b; data->c = c; data->d = d;
    shape->type = SHAPE3D_TETRAHEDRON;
    shape->data = data;
    return shape;
}

double shape3d_volume(const Shape3D* shape) {
    if (!shape || !shape->data) return -1.0;
    
    switch (shape->type) {
        case SHAPE3D_SPHERE: {
            SphereData* s = (SphereData*)shape->data;
            return (4.0/3.0) * M_PI * s->radius * s->radius * s->radius;
        }
        case SHAPE3D_CUBE: {
            CubeData* c = (CubeData*)shape->data;
            return c->width * c->height * c->depth;
        }
        case SHAPE3D_CYLINDER: {
            CylinderData* cy = (CylinderData*)shape->data;
            return M_PI * cy->radius * cy->radius * cy->height;
        }
        case SHAPE3D_CONE: {
            ConeData* co = (ConeData*)shape->data;
            return (1.0/3.0) * M_PI * co->radius * co->radius * co->height;
        }
        case SHAPE3D_PYRAMID: {
            PyramidData* p = (PyramidData*)shape->data;
            return (1.0/3.0) * p->base_width * p->base_depth * p->height;
        }
        case SHAPE3D_TETRAHEDRON: {
            TetrahedronData* t = (TetrahedronData*)shape->data;
            double abx = t->b.x - t->a.x, aby = t->b.y - t->a.y, abz = t->b.z - t->a.z;
            double acx = t->c.x - t->a.x, acy = t->c.y - t->a.y, acz = t->c.z - t->a.z;
            double adx = t->d.x - t->a.x, ady = t->d.y - t->a.y, adz = t->d.z - t->a.z;
            
            double cpx = acy*adz - acz*ady;
            double cpy = acz*adx - acx*adz;
            double cpz = acx*ady - acy*adx;
            
            double scalar_triple = abx*cpx + aby*cpy + abz*cpz;
            return fabs(scalar_triple) / 6.0;
        }
        case SHAPE3D_POINT:
            return 0.0;
        default:
            return -1.0;
    }
}

double shape3d_surface_area(const Shape3D* shape) {
    if (!shape || !shape->data) return -1.0;
    
    switch (shape->type) {
        case SHAPE3D_SPHERE: {
            SphereData* s = (SphereData*)shape->data;
            return 4.0 * M_PI * s->radius * s->radius;
        }
        case SHAPE3D_CUBE: {
            CubeData* c = (CubeData*)shape->data;
            return 2.0 * (c->width*c->height + c->width*c->depth + c->height*c->depth);
        }
        case SHAPE3D_CYLINDER: {
            CylinderData* cy = (CylinderData*)shape->data;
            double base_area = M_PI * cy->radius * cy->radius;
            double side_area = 2.0 * M_PI * cy->radius * cy->height;
            return 2.0 * base_area + side_area;
        }
        case SHAPE3D_CONE: {
            ConeData* co = (ConeData*)shape->data;
            double base_area = M_PI * co->radius * co->radius;
            double slant_height = sqrt(co->radius * co->radius + co->height * co->height);
            double side_area = M_PI * co->radius * slant_height;
            return base_area + side_area;
        }
        case SHAPE3D_PYRAMID: {
            PyramidData* p = (PyramidData*)shape->data;
            double base_area = p->base_width * p->base_depth;
            double half_w = p->base_width / 2.0;
            double half_d = p->base_depth / 2.0;
            
            // Calculate all 4 triangular face areas
            double slant_front = sqrt(half_d * half_d + p->height * p->height);
            double slant_back = slant_front;
            double slant_left = sqrt(half_w * half_w + p->height * p->height);
            double slant_right = slant_left;
            
            double area_front = 0.5 * p->base_width * slant_front;
            double area_back = 0.5 * p->base_width * slant_back;
            double area_left = 0.5 * p->base_depth * slant_left;
            double area_right = 0.5 * p->base_depth * slant_right;
            
            return base_area + area_front + area_back + area_left + area_right;
        }
        case SHAPE3D_TETRAHEDRON: {
            TetrahedronData* t = (TetrahedronData*)shape->data;
            double area1 = tetra_triangle_area(t->a, t->b, t->c);
            double area2 = tetra_triangle_area(t->a, t->b, t->d);
            double area3 = tetra_triangle_area(t->a, t->c, t->d);
            double area4 = tetra_triangle_area(t->b, t->c, t->d);
            return area1 + area2 + area3 + area4;
        }
        case SHAPE3D_POINT:
            return 0.0;
        default:
            return -1.0;
    }
}

void print_shape3d(const Shape3D* shape) {
    if (!shape || !shape->data) {
        printf("Invalid shape\n");
        return;
    }
    
    switch (shape->type) {
        case SHAPE3D_POINT: {
            Point3D* p = (Point3D*)shape->data;
            printf("Point3D: (%.2f, %.2f, %.2f)\n", p->x, p->y, p->z);
            break;
        }
        case SHAPE3D_SPHERE: {
            SphereData* s = (SphereData*)shape->data;
            printf("Sphere: Center(%.2f, %.2f, %.2f) Radius: %.2f\n", 
                   s->center.x, s->center.y, s->center.z, s->radius);
            break;
        }
        case SHAPE3D_CUBE: {
            CubeData* c = (CubeData*)shape->data;
            printf("Cube: Center(%.2f,%.2f,%.2f) W:%.2f H:%.2f D:%.2f\n",
                   c->center.x, c->center.y, c->center.z, c->width, c->height, c->depth);
            break;
        }
        case SHAPE3D_CYLINDER: {
            CylinderData* cy = (CylinderData*)shape->data;
            printf("Cylinder: Base(%.2f,%.2f,%.2f) R:%.2f H:%.2f\n",
                   cy->base_center.x, cy->base_center.y, cy->base_center.z, 
                   cy->radius, cy->height);
            break;
        }
        case SHAPE3D_CONE: {
            ConeData* co = (ConeData*)shape->data;
            printf("Cone: Base(%.2f,%.2f,%.2f) R:%.2f H:%.2f\n",
                   co->base_center.x, co->base_center.y, co->base_center.z, 
                   co->radius, co->height);
            break;
        }
        case SHAPE3D_PYRAMID: {
            PyramidData* p = (PyramidData*)shape->data;
            printf("Pyramid: Base(%.2f,%.2f,%.2f) W:%.2f D:%.2f H:%.2f\n",
                   p->base_center.x, p->base_center.y, p->base_center.z,
                   p->base_width, p->base_depth, p->height);
            break;
        }
        case SHAPE3D_TETRAHEDRON: {
            TetrahedronData* t = (TetrahedronData*)shape->data;
            printf("Tetrahedron:\n");
            printf("  A(%.2f,%.2f,%.2f)\n", t->a.x, t->a.y, t->a.z);
            printf("  B(%.2f,%.2f,%.2f)\n", t->b.x, t->b.y, t->b.z);
            printf("  C(%.2f,%.2f,%.2f)\n", t->c.x, t->c.y, t->c.z);
            printf("  D(%.2f,%.2f,%.2f)\n", t->d.x, t->d.y, t->d.z);
            break;
        }
    }
}

void destroy_shape3d(Shape3D* shape) {
    if (shape) {
        free(shape->data);
        free(shape);
    }
}

int is_point_in_shape3d(const Shape3D* shape, Point3D p) {
    if (!shape || !shape->data) return 0;
    
    switch (shape->type) {
        case SHAPE3D_SPHERE: {
            SphereData* s = (SphereData*)shape->data;
            double dx = p.x - s->center.x;
            double dy = p.y - s->center.y;
            double dz = p.z - s->center.z;
            double dist_sq = dx*dx + dy*dy + dz*dz;
            return dist_sq <= (s->radius * s->radius);
        }
        case SHAPE3D_CUBE: {
            CubeData* c = (CubeData*)shape->data;
            double half_w = c->width / 2.0;
            double half_h = c->height / 2.0;
            double half_d = c->depth / 2.0;
            
            return (p.x >= c->center.x - half_w && p.x <= c->center.x + half_w &&
                    p.y >= c->center.y - half_h && p.y <= c->center.y + half_h &&
                    p.z >= c->center.z - half_d && p.z <= c->center.z + half_d);
        }
        case SHAPE3D_CYLINDER: {
            CylinderData* cy = (CylinderData*)shape->data;
            if (p.y < cy->base_center.y || p.y > cy->base_center.y + cy->height) 
                return 0;
            double dx = p.x - cy->base_center.x;
            double dz = p.z - cy->base_center.z;
            double radial_dist_sq = dx*dx + dz*dz;
            return radial_dist_sq <= (cy->radius * cy->radius);
        }
        case SHAPE3D_CONE: {
            ConeData* co = (ConeData*)shape->data;
            if (p.y < co->base_center.y || p.y > co->base_center.y + co->height)
                return 0;
            double height_ratio = (p.y - co->base_center.y) / co->height;
            double current_radius = co->radius * (1.0 - height_ratio);
            double dx = p.x - co->base_center.x;
            double dz = p.z - co->base_center.z;
            double radial_dist_sq = dx*dx + dz*dz;
            return radial_dist_sq <= (current_radius * current_radius);
        }
        case SHAPE3D_PYRAMID: {
            PyramidData* py = (PyramidData*)shape->data;
            if (p.y < py->base_center.y || p.y > py->base_center.y + py->height)
                return 0;
            double height_ratio = (p.y - py->base_center.y) / py->height;
            double current_width = py->base_width * (1.0 - height_ratio);
            double current_depth = py->base_depth * (1.0 - height_ratio);
            double half_w = current_width / 2.0;
            double half_d = current_depth / 2.0;
            return (p.x >= py->base_center.x - half_w && p.x <= py->base_center.x + half_w &&
                    p.z >= py->base_center.z - half_d && p.z <= py->base_center.z + half_d);
        }
        case SHAPE3D_POINT: {
            Point3D* point = (Point3D*)shape->data;
            return (fabs(p.x - point->x) < 1e-10 && 
                    fabs(p.y - point->y) < 1e-10 && 
                    fabs(p.z - point->z) < 1e-10);
        }
        case SHAPE3D_TETRAHEDRON: {
            TetrahedronData* t = (TetrahedronData*)shape->data;
            double abx = t->b.x - t->a.x, aby = t->b.y - t->a.y, abz = t->b.z - t->a.z;
            double acx = t->c.x - t->a.x, acy = t->c.y - t->a.y, acz = t->c.z - t->a.z;
            double adx = t->d.x - t->a.x, ady = t->d.y - t->a.y, adz = t->d.z - t->a.z;
            double apx = p.x - t->a.x, apy = p.y - t->a.y, apz = p.z - t->a.z;
            
            double v = abx*(acy*adz - acz*ady) - aby*(acx*adz - acz*adx) + abz*(acx*ady - acy*adx);
            if (fabs(v) < 1e-10) return 0;
            
            double v1 = apx*(acy*adz - acz*ady) - apy*(acx*adz - acz*adx) + apz*(acx*ady - acy*adx);
            double v2 = abx*(apy*adz - apz*ady) - aby*(apx*adz - apz*adx) + abz*(apx*ady - apy*adx);
            double v3 = abx*(acy*apz - acz*apy) - aby*(acx*apz - acz*apx) + abz*(acx*apy - acy*apx);
            
            double u = v1 / v;
            double v_coord = v2 / v;
            double w = v3 / v;
            
            // Fixed: Check that all coordinates are non-negative and sum <= 1
            return (u >= 0 && v_coord >= 0 && w >= 0 && (u + v_coord + w) <= 1.0);
        }
        default:
            return 0;
    }
}

#endif // SHAPES3D_IMPLEMENTATION
#endif // SHAPES3D_H
