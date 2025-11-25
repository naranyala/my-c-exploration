// shapes2d.h - Single-header 2D shapes library
#ifndef SHAPES2D_H
#define SHAPES2D_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Point structure
typedef struct {
    double x, y;
} Point2D;

// Shape types
typedef enum {
    SHAPE_POINT,
    SHAPE_CIRCLE,
    SHAPE_RECTANGLE,
    SHAPE_TRIANGLE,
    SHAPE_LINE
} ShapeType;

// Generic shape structure
typedef struct {
    ShapeType type;
    void* data;
} Shape2D;

// Shape-specific data structures
typedef struct {
    double radius;
    Point2D center;
} CircleData;

typedef struct {
    double width, height;
    Point2D bottom_left;
} RectangleData;

typedef struct {
    Point2D a, b, c;
} TriangleData;

typedef struct {
    Point2D start, end;
} LineData;

// Function prototypes
Point2D make_point(double x, double y);
Shape2D* create_point(Point2D p);
Shape2D* create_circle(Point2D center, double radius);
Shape2D* create_rectangle(Point2D bottom_left, double width, double height);
Shape2D* create_triangle(Point2D a, Point2D b, Point2D c);
Shape2D* create_line(Point2D start, Point2D end);
double shape_area(const Shape2D* shape);
double shape_perimeter(const Shape2D* shape);
void print_shape(const Shape2D* shape);
void destroy_shape(Shape2D* shape);
int is_point_in_shape(const Shape2D* shape, Point2D p);

// Implementation
#ifdef SHAPES2D_IMPLEMENTATION

Point2D make_point(double x, double y) {
    Point2D p = {x, y};
    return p;
}

Shape2D* create_point(Point2D p) {
    Shape2D* shape = malloc(sizeof(Shape2D));
    if (!shape) return NULL;
    
    Point2D* data = malloc(sizeof(Point2D));
    if (!data) {
        free(shape);
        return NULL;
    }
    
    *data = p;
    shape->type = SHAPE_POINT;
    shape->data = data;
    return shape;
}

Shape2D* create_circle(Point2D center, double radius) {
    if (radius < 0) return NULL;
    
    Shape2D* shape = malloc(sizeof(Shape2D));
    if (!shape) return NULL;
    
    CircleData* data = malloc(sizeof(CircleData));
    if (!data) {
        free(shape);
        return NULL;
    }
    
    data->center = center;
    data->radius = radius;
    shape->type = SHAPE_CIRCLE;
    shape->data = data;
    return shape;
}

Shape2D* create_rectangle(Point2D bottom_left, double width, double height) {
    if (width < 0 || height < 0) return NULL;
    
    Shape2D* shape = malloc(sizeof(Shape2D));
    if (!shape) return NULL;
    
    RectangleData* data = malloc(sizeof(RectangleData));
    if (!data) {
        free(shape);
        return NULL;
    }
    
    data->bottom_left = bottom_left;
    data->width = width;
    data->height = height;
    shape->type = SHAPE_RECTANGLE;
    shape->data = data;
    return shape;
}

Shape2D* create_triangle(Point2D a, Point2D b, Point2D c) {
    Shape2D* shape = malloc(sizeof(Shape2D));
    if (!shape) return NULL;
    
    TriangleData* data = malloc(sizeof(TriangleData));
    if (!data) {
        free(shape);
        return NULL;
    }
    
    data->a = a; data->b = b; data->c = c;
    shape->type = SHAPE_TRIANGLE;
    shape->data = data;
    return shape;
}

Shape2D* create_line(Point2D start, Point2D end) {
    Shape2D* shape = malloc(sizeof(Shape2D));
    if (!shape) return NULL;
    
    LineData* data = malloc(sizeof(LineData));
    if (!data) {
        free(shape);
        return NULL;
    }
    
    data->start = start;
    data->end = end;
    shape->type = SHAPE_LINE;
    shape->data = data;
    return shape;
}

double shape_area(const Shape2D* shape) {
    if (!shape || !shape->data) return -1.0;
    
    switch (shape->type) {
        case SHAPE_CIRCLE: {
            CircleData* c = (CircleData*)shape->data;
            return M_PI * c->radius * c->radius;
        }
        case SHAPE_RECTANGLE: {
            RectangleData* r = (RectangleData*)shape->data;
            return r->width * r->height;
        }
        case SHAPE_TRIANGLE: {
            TriangleData* t = (TriangleData*)shape->data;
            double area = 0.5 * fabs(
                (t->b.x - t->a.x) * (t->c.y - t->a.y) - 
                (t->c.x - t->a.x) * (t->b.y - t->a.y)
            );
            return area;
        }
        case SHAPE_POINT:
        case SHAPE_LINE:
            return 0.0;
        default:
            return -1.0;
    }
}

double shape_perimeter(const Shape2D* shape) {
    if (!shape || !shape->data) return -1.0;
    
    switch (shape->type) {
        case SHAPE_CIRCLE: {
            CircleData* c = (CircleData*)shape->data;
            return 2 * M_PI * c->radius;
        }
        case SHAPE_RECTANGLE: {
            RectangleData* r = (RectangleData*)shape->data;
            return 2 * (r->width + r->height);
        }
        case SHAPE_TRIANGLE: {
            TriangleData* t = (TriangleData*)shape->data;
            double a = sqrt(pow(t->b.x - t->a.x, 2) + pow(t->b.y - t->a.y, 2));
            double b = sqrt(pow(t->c.x - t->b.x, 2) + pow(t->c.y - t->b.y, 2));
            double c = sqrt(pow(t->a.x - t->c.x, 2) + pow(t->a.y - t->c.y, 2));
            return a + b + c;
        }
        case SHAPE_LINE: {
            LineData* l = (LineData*)shape->data;
            return sqrt(pow(l->end.x - l->start.x, 2) + pow(l->end.y - l->start.y, 2));
        }
        case SHAPE_POINT:
            return 0.0;
        default:
            return -1.0;
    }
}

void print_shape(const Shape2D* shape) {
    if (!shape || !shape->data) {
        printf("Invalid shape\n");
        return;
    }
    
    switch (shape->type) {
        case SHAPE_POINT: {
            Point2D* p = (Point2D*)shape->data;
            printf("Point: (%.2f, %.2f)\n", p->x, p->y);
            break;
        }
        case SHAPE_CIRCLE: {
            CircleData* c = (CircleData*)shape->data;
            printf("Circle: Center(%.2f, %.2f) Radius: %.2f\n", 
                   c->center.x, c->center.y, c->radius);
            break;
        }
        case SHAPE_RECTANGLE: {
            RectangleData* r = (RectangleData*)shape->data;
            printf("Rectangle: BottomLeft(%.2f, %.2f) W:%.2f H:%.2f\n",
                   r->bottom_left.x, r->bottom_left.y, r->width, r->height);
            break;
        }
        case SHAPE_TRIANGLE: {
            TriangleData* t = (TriangleData*)shape->data;
            printf("Triangle: A(%.2f,%.2f) B(%.2f,%.2f) C(%.2f,%.2f)\n",
                   t->a.x, t->a.y, t->b.x, t->b.y, t->c.x, t->c.y);
            break;
        }
        case SHAPE_LINE: {
            LineData* l = (LineData*)shape->data;
            printf("Line: Start(%.2f,%.2f) End(%.2f,%.2f)\n",
                   l->start.x, l->start.y, l->end.x, l->end.y);
            break;
        }
    }
}

void destroy_shape(Shape2D* shape) {
    if (shape) {
        free(shape->data);
        free(shape);
    }
}

int is_point_in_shape(const Shape2D* shape, Point2D p) {
    if (!shape || !shape->data) return 0;
    
    switch (shape->type) {
        case SHAPE_CIRCLE: {
            CircleData* c = (CircleData*)shape->data;
            double dx = p.x - c->center.x;
            double dy = p.y - c->center.y;
            return (dx*dx + dy*dy) <= (c->radius * c->radius);
        }
        case SHAPE_RECTANGLE: {
            RectangleData* r = (RectangleData*)shape->data;
            return (p.x >= r->bottom_left.x && 
                    p.x <= r->bottom_left.x + r->width &&
                    p.y >= r->bottom_left.y && 
                    p.y <= r->bottom_left.y + r->height);
        }
        case SHAPE_TRIANGLE: {
            TriangleData* t = (TriangleData*)shape->data;
            // Barycentric coordinate method
            double denom = (t->b.y - t->c.y)*(t->a.x - t->c.x) + 
                          (t->c.x - t->b.x)*(t->a.y - t->c.y);
            if (fabs(denom) < 1e-10) return 0; // Degenerate triangle
            
            double a = ((t->b.y - t->c.y)*(p.x - t->c.x) + 
                       (t->c.x - t->b.x)*(p.y - t->c.y)) / denom;
            double b = ((t->c.y - t->a.y)*(p.x - t->c.x) + 
                       (t->a.x - t->c.x)*(p.y - t->c.y)) / denom;
            double c = 1 - a - b;
            
            return (a >= 0 && a <= 1) && (b >= 0 && b <= 1) && (c >= 0 && c <= 1);
        }
        case SHAPE_POINT: {
            Point2D* point = (Point2D*)shape->data;
            return (fabs(p.x - point->x) < 1e-10 && fabs(p.y - point->y) < 1e-10);
        }
        case SHAPE_LINE:
            // Lines have no area - point can't be "inside"
            return 0;
        default:
            return 0;
    }
}

#endif // SHAPES2D_IMPLEMENTATION
#endif // SHAPES2D_H
