#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>

struct Vec3 {
    double x, y, z;
    Vec3(double x_=0, double y_=0, double z_=0) : x(x_), y(y_), z(z_) {}
    Vec3 operator+(const Vec3& v) const { return Vec3(x+v.x, y+v.y, z+v.z); }
    Vec3 operator-(const Vec3& v) const { return Vec3(x-v.x, y-v.y, z-v.z); }
    Vec3 operator*(double s) const { return Vec3(x*s, y*s, z*s); }
    Vec3 operator/(double s) const { return Vec3(x/s, y/s, z/s); }
    double dot(const Vec3& v) const { return x*v.x + y*v.y + z*v.z; }
    Vec3 normalize() const { double mag = std::sqrt(x*x + y*y + z*z); return *this / mag; }
};

struct Ray {
    Vec3 origin, direction;
    Ray(const Vec3& o, const Vec3& d) : origin(o), direction(d) {}
};

struct Sphere {
    Vec3 center;
    double radius;
    Vec3 color;
    Sphere(const Vec3& c, double r, const Vec3& col) : center(c), radius(r), color(col) {}

    bool intersect(const Ray& ray, double& t) const {
        Vec3 oc = ray.origin - center;
        double b = 2.0 * oc.dot(ray.direction);
        double c = oc.dot(oc) - radius*radius;
        double discriminant = b*b - 4*c;
        if (discriminant < 0) return false;
        else {
            t = (-b - std::sqrt(discriminant)) / 2.0;
            return t > 0;
        }
    }
};

Vec3 trace(const Ray& ray, const std::vector<Sphere>& spheres) {
    double t_min = 1e9;
    const Sphere* hit_sphere = nullptr;

    for (const auto& sphere : spheres) {
        double t;
        if (sphere.intersect(ray, t) && t < t_min) {
            t_min = t;
            hit_sphere = &sphere;
        }
    }

    if (hit_sphere) {
        Vec3 hit_point = ray.origin + ray.direction * t_min;
        Vec3 normal = (hit_point - hit_sphere->center).normalize();
        double light = std::max(0.0, normal.dot(Vec3(1, 1, -1).normalize()));
        return hit_sphere->color * light;
    }

    return Vec3(0.2, 0.7, 0.8); // Background color
}

int main() {
    const int width = 800, height = 600;
    std::ofstream out("output.ppm");
    out << "P3\n" << width << " " << height << "\n255\n";

    Vec3 camera(0, 0, 0);
    std::vector<Sphere> spheres = {
        Sphere(Vec3(0, 0, -5), 1, Vec3(1, 0, 0)),
        Sphere(Vec3(2, 0, -6), 1, Vec3(0, 1, 0)),
        Sphere(Vec3(-2, 0, -6), 1, Vec3(0, 0, 1))
    };

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            double u = (x - width / 2.0) / width;
            double v = (height / 2.0 - y) / height;
            Vec3 dir = Vec3(u, v, -1).normalize();
            Ray ray(camera, dir);
            Vec3 color = trace(ray, spheres);
            int r = std::min(255, int(color.x * 255));
            int g = std::min(255, int(color.y * 255));
            int b = std::min(255, int(color.z * 255));
            out << r << " " << g << " " << b << "\n";
        }
    }

    out.close();
    std::cout << "Rendered to output.ppm\n";
    return 0;
}


