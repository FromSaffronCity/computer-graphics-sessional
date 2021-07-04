#include<iostream>
#include<fstream>
#include<cmath>
#include<vector>
#include<limits>

using namespace std;

#define PI 2*acos(0.0)
#define INF numeric_limits<double>::infinity()

/* Vector class */
class Vector {
    double x;
    double y;
    double z;

public:
    Vector() {
        x = y = z = 0.0;
    }

    Vector(double x, double y, double z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    double getX() const {
        return x;
    }

    double getY() const {
        return y;
    }

    double getZ() const {
        return z;
    }

    void normalize();
    double computeDistanceBetween(Vector);

    Vector operator+(const Vector);
    Vector operator-(const Vector);
    Vector operator*(const double);
    double operator/(const Vector);
    Vector operator%(const Vector);

    friend ifstream& operator>>(ifstream&, Vector&);
    friend ostream& operator<<(ostream&, Vector&);

    ~Vector() {
        /* destructor */
    }
};

void Vector::normalize() {
    double magnitude = sqrt(pow(x, 2.0)+pow(y, 2.0)+pow(z, 2.0));
    x /= magnitude;
    y /= magnitude;
    z /= magnitude;
}

double Vector::computeDistanceBetween(Vector _vector) {
    return sqrt(pow(x-_vector.x, 2.0)+pow(y-_vector.y, 2.0)+pow(z-_vector.z, 2.0));
}

Vector Vector::operator+(const Vector _vector) {
    return Vector(x+_vector.x, y+_vector.y, z+_vector.z);
}

Vector Vector::operator-(const Vector _vector) {
    return Vector(x-_vector.x, y-_vector.y, z-_vector.z);
}

Vector Vector::operator*(const double scalar) {
    /* vector scalar multiplication */
    return Vector(x*scalar, y*scalar, z*scalar);
}

double Vector::operator/(const Vector _vector) {
    /* vector dot multiplication */
    return x*_vector.x+y*_vector.y+z*_vector.z;
}

Vector Vector::operator%(const Vector _vector) {
    /* vector cross multiplication */
    return Vector(y*_vector.z-z*_vector.y, z*_vector.x-x*_vector.z, x*_vector.y-y*_vector.x);
}

ifstream& operator>>(ifstream &input, Vector &_vector) {
    input >> _vector.x >> _vector.y >> _vector.z;
    return input;
}

ostream& operator<<(ostream &output, Vector &_vector) {
    output << '(' << _vector.x << ", " << _vector.y << ", " << _vector.z << ')';
    return output;
}

/* Ray class */
class Ray {
    Vector rO;  // origin vector
    Vector rD;  // unit direction vector

public:
    Ray() {
        /* default constructor */
    }

    Ray(Vector rO, Vector rD) {
        this->rO = rO;
        this->rD = rD;
        this->rD.normalize();
    }

    Vector getRO() const {
        return rO;
    }

    Vector getRD() const {
        return rD;
    }

    ~Ray() {
        /* destructor */
    }
};

/* Color structure */
struct Color {
    double red;
    double green;
    double blue;

    Color() {
        red = green = blue = 0.0;
    }

    Color(double red, double green, double blue) {
        this->red = red;
        this->green = green;
        this->blue = blue;
    }

    friend ifstream& operator>>(ifstream&, Color&);
    friend ostream& operator<<(ostream&, Color&);

    ~Color() {
        /* destructor */
    }
};

ifstream& operator>>(ifstream &input, Color &color) {
    input >> color.red >> color.green >> color.blue;
    return input;
}

ostream& operator<<(ostream &output, Color &color) {
    output << '[' << color.red << ", " << color.green << ", " << color.blue << ']';
    return output;
}

/* Light class */
class Light {
    Vector position;
    Color color;

    /* necessary properties for drawing light */
    double radius;
    int segments;
    int stacks;

public:
    Light() {
        radius = 0.0;
        segments = stacks = 0;
    }

    Light(Vector position, Color color, double radius, int segments, int stacks) {
        this->position = position;
        this->color = color;

        this->radius = radius;
        this->segments = segments;
        this->stacks = stacks;
    }

    Vector getPosition() const {
        return position;
    }

    Color getColor() const {
        return color;
    }

    void draw();

    ~Light() {
        /* destructor */
    }
};

void Light::draw() {
    Vector points[stacks+1][segments+1];
    double height, _radius;

    /* generating points: segments = segments in plane; stacks = segments in hemisphere */
	for(int i=0; i<=stacks; i++) {
		height = radius*sin(((double)i/(double)stacks)*(PI/2));
		_radius = radius*cos(((double)i/(double)stacks)*(PI/2));

		for(int j=0; j<=segments; j++) {
            points[i][j] = Vector(_radius*cos(((double)j/(double)segments)*2*PI), _radius*sin(((double)j/(double)segments)*2*PI), height);
		}
	}

	/* drawing quads using generated points */
	glColor3f(color.red, color.green, color.blue);

	for(int i=0; i<stacks; i++) {
		for(int j=0; j<segments; j++) {
			glBegin(GL_QUADS);
			{
			    /* upper hemisphere */
				glVertex3f((position+points[i][j]).getX(), (position+points[i][j]).getY(), (position+points[i][j]).getZ());
				glVertex3f((position+points[i][j+1]).getX(), (position+points[i][j+1]).getY(), (position+points[i][j+1]).getZ());
				glVertex3f((position+points[i+1][j+1]).getX(), (position+points[i+1][j+1]).getY(), (position+points[i+1][j+1]).getZ());
				glVertex3f((position+points[i+1][j]).getX(), (position+points[i+1][j]).getY(), (position+points[i+1][j]).getZ());

                /* lower hemisphere */
                glVertex3f((position+points[i][j]).getX(), (position+points[i][j]).getY(), (position-points[i][j]).getZ());
				glVertex3f((position+points[i][j+1]).getX(), (position+points[i][j+1]).getY(), (position-points[i][j+1]).getZ());
				glVertex3f((position+points[i+1][j+1]).getX(), (position+points[i+1][j+1]).getY(), (position-points[i+1][j+1]).getZ());
				glVertex3f((position+points[i+1][j]).getX(), (position+points[i+1][j]).getY(), (position-points[i+1][j]).getZ());
			}
			glEnd();
		}
	}
}

/* ReflectionCoefficient structure */
struct ReflectionCoefficient {
    double ambientReflectionCoefficient;
    double diffuseReflectionCoefficient;
    double specularReflectionCoefficient;
    double recursiveReflectionCoefficient;

    ReflectionCoefficient() {
        ambientReflectionCoefficient = diffuseReflectionCoefficient = specularReflectionCoefficient = recursiveReflectionCoefficient = 0.0;
    }

    ReflectionCoefficient(double ambientReflectionCoefficient, double diffuseReflectionCoefficient, double specularReflectionCoefficient, double recursiveReflectionCoefficient) {
        this->ambientReflectionCoefficient = ambientReflectionCoefficient;
        this->diffuseReflectionCoefficient = diffuseReflectionCoefficient;
        this->specularReflectionCoefficient = specularReflectionCoefficient;
        this->recursiveReflectionCoefficient = recursiveReflectionCoefficient;
    }

    friend ifstream& operator>>(ifstream&, ReflectionCoefficient&);
    friend ostream& operator<<(ostream&, ReflectionCoefficient&);

    ~ReflectionCoefficient() {
        /* destructor */
    }
};

ifstream& operator>>(ifstream &input, ReflectionCoefficient &reflectionCoefficient) {
    input >> reflectionCoefficient.ambientReflectionCoefficient >> reflectionCoefficient.diffuseReflectionCoefficient >> reflectionCoefficient.specularReflectionCoefficient >> reflectionCoefficient.recursiveReflectionCoefficient;
    return input;
}

ostream& operator<<(ostream &output, ReflectionCoefficient &reflectionCoefficient) {
    output << '[' << reflectionCoefficient.ambientReflectionCoefficient << ", " << reflectionCoefficient.diffuseReflectionCoefficient << ", " << reflectionCoefficient.specularReflectionCoefficient << ", " << reflectionCoefficient.recursiveReflectionCoefficient << ']';
    return output;
}

/* Object abstract class */
class Object {
    Color color;
    ReflectionCoefficient reflectionCoefficient;
    int shininess;

public:
    Object() {
        shininess = 0;
    }

    Color getColor() const {
        return color;
    }

    void setColor(Color color) {
        this->color = color;
    }

    ReflectionCoefficient getReflectionCoefficient() const {
        return reflectionCoefficient;
    }

    void setReflectionCoefficient(ReflectionCoefficient reflectionCoefficient) {
        this->reflectionCoefficient = reflectionCoefficient;
    }

    int getShininess() const {
        return shininess;
    }

    void setShininess(int shininess) {
        this->shininess = shininess;
    }

    void computeAmbientLightComponent(Color&, Color);
    void computeReflectionComponents(Ray, Color&, Vector, Color, Vector, Light, Ray);
    void computeRecursiveReflectionComponent(Color&, Color);

    virtual void draw() = 0;
    virtual double intersect(Ray, Color&, int) = 0;

    ~Object() {
        /* destructor */
    }
};

void Object::computeAmbientLightComponent(Color& color, Color intersectionPointColor) {
    color.red = intersectionPointColor.red*getReflectionCoefficient().ambientReflectionCoefficient;
    color.green = intersectionPointColor.green*getReflectionCoefficient().ambientReflectionCoefficient;
    color.blue = intersectionPointColor.blue*getReflectionCoefficient().ambientReflectionCoefficient;
}

void Object::computeReflectionComponents(Ray ray, Color& color, Vector intersectionPoint, Color intersectionPointColor, Vector normal, Light light, Ray incidentRay) {
    double lambertValue = (incidentRay.getRD()*(-1.0))/normal;
    Ray reflectedRay(intersectionPoint, incidentRay.getRD()-normal*((incidentRay.getRD()/normal)*2.0));
    double phongValue = (ray.getRD()*(-1.0))/reflectedRay.getRD();

    color.red += light.getColor().red*intersectionPointColor.red*getReflectionCoefficient().diffuseReflectionCoefficient*max(lambertValue, 0.0);
    color.green += light.getColor().green*intersectionPointColor.green*getReflectionCoefficient().diffuseReflectionCoefficient*max(lambertValue, 0.0);
    color.blue += light.getColor().blue*intersectionPointColor.blue*getReflectionCoefficient().diffuseReflectionCoefficient*max(lambertValue, 0.0);

    color.red += light.getColor().red*intersectionPointColor.red*getReflectionCoefficient().specularReflectionCoefficient*pow(max(phongValue, 0.0), getShininess());
    color.green += light.getColor().green*intersectionPointColor.green*getReflectionCoefficient().specularReflectionCoefficient*pow(max(phongValue, 0.0), getShininess());
    color.blue += light.getColor().blue*intersectionPointColor.blue*getReflectionCoefficient().specularReflectionCoefficient*pow(max(phongValue, 0.0), getShininess());
}

void Object::computeRecursiveReflectionComponent(Color& color, Color reflectedColor) {
    color.red += reflectedColor.red*getReflectionCoefficient().recursiveReflectionCoefficient;
    color.green += reflectedColor.green*getReflectionCoefficient().recursiveReflectionCoefficient;
    color.blue += reflectedColor.blue*getReflectionCoefficient().recursiveReflectionCoefficient;
}

/*
    defining global parameters:
        - position = position vector of camera

        - recursionlevel = level of recursion for recursive ray tracing

        - objects = vector containing all the objects in the scene
        - lights = vector containing all the lights in the scene
*/

Vector position;

int recursionLevel = 0;

vector<Object*> objects;
vector<Light> lights;

/* Sphere class */
class Sphere: public Object {
    Vector center;
    double radius;

    /* necessary properties for drawing sphere */
    int segments;
    int stacks;

public:
    Sphere() {
        radius = 0.0;
        segments = stacks = 0;
    }

    Sphere(Vector center, double radius, int segments, int stacks) {
        this->center = center;
        this->radius = radius;

        this->segments = segments;
        this->stacks = stacks;
    }

    void draw();
    double intersect(Ray, Color&, int);

    ~Sphere() {
        /* destructor */
    }
};

void Sphere::draw() {
    Vector points[stacks+1][segments+1];
    double height, _radius;

    /* generating points: segments = segments in plane; stacks = segments in hemisphere */
	for(int i=0; i<=stacks; i++) {
		height = radius*sin(((double)i/(double)stacks)*(PI/2));
		_radius = radius*cos(((double)i/(double)stacks)*(PI/2));

		for(int j=0; j<=segments; j++) {
            points[i][j] = Vector(_radius*cos(((double)j/(double)segments)*2*PI), _radius*sin(((double)j/(double)segments)*2*PI), height);
		}
	}

	/* drawing quads using generated points */
	glColor3f(getColor().red, getColor().green, getColor().blue);

	for(int i=0; i<stacks; i++) {
		for(int j=0; j<segments; j++) {
			glBegin(GL_QUADS);
			{
			    /* upper hemisphere */
				glVertex3f((center+points[i][j]).getX(), (center+points[i][j]).getY(), (center+points[i][j]).getZ());
				glVertex3f((center+points[i][j+1]).getX(), (center+points[i][j+1]).getY(), (center+points[i][j+1]).getZ());
				glVertex3f((center+points[i+1][j+1]).getX(), (center+points[i+1][j+1]).getY(), (center+points[i+1][j+1]).getZ());
				glVertex3f((center+points[i+1][j]).getX(), (center+points[i+1][j]).getY(), (center+points[i+1][j]).getZ());

                /* lower hemisphere */
                glVertex3f((center+points[i][j]).getX(), (center+points[i][j]).getY(), (center-points[i][j]).getZ());
				glVertex3f((center+points[i][j+1]).getX(), (center+points[i][j+1]).getY(), (center-points[i][j+1]).getZ());
				glVertex3f((center+points[i+1][j+1]).getX(), (center+points[i+1][j+1]).getY(), (center-points[i+1][j+1]).getZ());
				glVertex3f((center+points[i+1][j]).getX(), (center+points[i+1][j]).getY(), (center-points[i+1][j]).getZ());
			}
			glEnd();
		}
	}
}

double Sphere::intersect(Ray ray, Color& color, int level) {
    /* finding intersecting tMin */
    double a, b, c, tMin;

    a = ray.getRD()/ray.getRD();
    b = ((ray.getRO()/ray.getRD())-(ray.getRD()/center))*2.0;
    c = (ray.getRO()/ray.getRO())+(center/center)-(ray.getRO()/center)*2.0-radius*radius;

    double discriminant = b*b-4.0*a*c;

    if(discriminant < 0.0) {
        tMin = INF;
    } else if(discriminant > 0.0) {
        double tMax = -b/(2.0*a)+sqrt(discriminant)/(2.0*a);
        tMin = -b/(2.0*a)-sqrt(discriminant)/(2.0*a);
        tMin = (tMin > 0.0)? tMin: tMax;
    } else {
        tMin = -b/(2.0*a);
    }

    if(level == 0) {
        return tMin;
    }

    /* illuminating with Phong Lighting Model */
    Vector intersectionPoint = ray.getRO()+ray.getRD()*tMin;
    Color intersectionPointColor = getColor();

    /* determining unit normal vector at intersection point on object's surface */
    Vector normal = intersectionPoint-center;
    normal.normalize();
    normal = (position.computeDistanceBetween(center) > radius)? normal: normal*(-1.0);

    /* computing ambient light component of reflected ray */
    computeAmbientLightComponent(color, intersectionPointColor);

    /* computing diffuse & specular reflection components of reflected ray */
    for(int i=0; i<lights.size(); i++) {
        Ray incidentRay(lights[i].getPosition(), intersectionPoint-lights[i].getPosition());

        /* checking if intersection point is in shadow */
        double t, tMinimum=INF;

        for(int j=0; j<objects.size(); j++) {
            Color dummyColor;  // color = black
            t = objects[j]->intersect(incidentRay, dummyColor, 0);

            if(t>0.0 && t<tMinimum) {
                tMinimum = t;
            }
        }

        Vector shadowIntersectionPoint = incidentRay.getRO()+incidentRay.getRD()*tMinimum;
        double epsilon = 0.0000001;  // for tuning light effect

        if(intersectionPoint.computeDistanceBetween(incidentRay.getRO())-epsilon > shadowIntersectionPoint.computeDistanceBetween(incidentRay.getRO())) {
            /* intersection point is, indeed, in shadow */
            continue;
        }

        /* computing diffuse & specular components of reflected ray */
        computeReflectionComponents(ray, color, intersectionPoint, intersectionPointColor, normal, lights[i], incidentRay);
    }

    /* handling recursive reflection */
    if(level >= recursionLevel) {
        return tMin;
    }

    /* incorporating concept of evil epsilon to recursive reflection computation */
    Vector reflectionDirection = ray.getRD()-normal*((ray.getRD()/normal)*2.0);
    reflectionDirection.normalize();
    Ray reflectedRay(intersectionPoint+reflectionDirection, reflectionDirection);

    /* finding nearest intersecting object (if available) */
    int nearest = INT_MAX;
    double t, tMinimum=INF;

    for(int i=0; i<objects.size(); i++) {
        Color dummyColor;  // color = black
        t = objects[i]->intersect(reflectedRay, dummyColor, 0);

        if(t>0.0 && t<tMinimum) {
            tMinimum = t;
            nearest = i;
        }
    }

    /* finding color component for reflected ray */
    Color reflectedColor;  // color = black

    if(nearest != INT_MAX) {
        tMinimum = objects[nearest]->intersect(reflectedRay, reflectedColor, level+1);
    }

    /* computing recursive reflection component of reflected ray */
    computeRecursiveReflectionComponent(color, reflectedColor);

    /* clipping the color values (if necessary) */
    color.red = (color.red > 1.0)? 1.0: ((color.red < 0.0)? 0.0: color.red);
    color.green = (color.green > 1.0)? 1.0: ((color.green < 0.0)? 0.0: color.green);
    color.blue = (color.blue > 1.0)? 1.0: ((color.blue < 0.0)? 0.0: color.blue);

    return tMin;
}

/* Triangle class */
class Triangle: public Object {
    Vector a;
    Vector b;
    Vector c;

public:
    Triangle() {
        /* default constructor */
    }

    Triangle(Vector a, Vector b, Vector c) {
        this->a = a;
        this->b = b;
        this->c = c;
    }

    void draw();
    double intersect(Ray, Color&, int);

    ~Triangle() {
        /* destructor */
    }
};

void Triangle::draw() {
    /* a, b, c - coordinates/position vectors of three corners of the triangle */
    glColor3f(getColor().red, getColor().green, getColor().blue);

    glBegin(GL_TRIANGLES);
    {
        glVertex3f(a.getX(), a.getY(), a.getZ());
        glVertex3f(b.getX(), b.getY(), b.getZ());
        glVertex3f(c.getX(), c.getY(), c.getZ());
    }
    glEnd();
}

double Triangle::intersect(Ray ray, Color& color, int level) {
    /* finding intersecting tMin */
    double determinantBase, determinantBeta, determinantGamma, determinantT, tMin;

    determinantBase = (a.getX()-b.getX())*((a.getY()-c.getY())*ray.getRD().getZ()-(a.getZ()-c.getZ())*ray.getRD().getY());
    determinantBase += (a.getX()-c.getX())*((a.getZ()-b.getZ())*ray.getRD().getY()-(a.getY()-b.getY())*ray.getRD().getZ());
    determinantBase += ray.getRD().getX()*((a.getY()-b.getY())*(a.getZ()-c.getZ())-(a.getZ()-b.getZ())*(a.getY()-c.getY()));

    determinantBeta = (a.getX()-ray.getRO().getX())*((a.getY()-c.getY())*ray.getRD().getZ()-(a.getZ()-c.getZ())*ray.getRD().getY());
    determinantBeta += (a.getX()-c.getX())*((a.getZ()-ray.getRO().getZ())*ray.getRD().getY()-(a.getY()-ray.getRO().getY())*ray.getRD().getZ());
    determinantBeta += ray.getRD().getX()*((a.getY()-ray.getRO().getY())*(a.getZ()-c.getZ())-(a.getZ()-ray.getRO().getZ())*(a.getY()-c.getY()));

    determinantGamma = (a.getX()-b.getX())*((a.getY()-ray.getRO().getY())*ray.getRD().getZ()-(a.getZ()-ray.getRO().getZ())*ray.getRD().getY());
    determinantGamma += (a.getX()-ray.getRO().getX())*((a.getZ()-b.getZ())*ray.getRD().getY()-(a.getY()-b.getY())*ray.getRD().getZ());
    determinantGamma += ray.getRD().getX()*((a.getY()-b.getY())*(a.getZ()-ray.getRO().getZ())-(a.getZ()-b.getZ())*(a.getY()-ray.getRO().getY()));

    determinantT = (a.getX()-b.getX())*((a.getY()-c.getY())*(a.getZ()-ray.getRO().getZ())-(a.getZ()-c.getZ())*(a.getY()-ray.getRO().getY()));
    determinantT += (a.getX()-c.getX())*((a.getZ()-b.getZ())*(a.getY()-ray.getRO().getY())-(a.getY()-b.getY())*(a.getZ()-ray.getRO().getZ()));
    determinantT += (a.getX()-ray.getRO().getX())*((a.getY()-b.getY())*(a.getZ()-c.getZ())-(a.getZ()-b.getZ())*(a.getY()-c.getY()));

    if(determinantBase == 0.0) {
        /* ray will not intersect the triangle plane */
        tMin = INF;
    } else {
        /* ray will intersect the triangle plane */
        if(determinantBeta/determinantBase>0.0 && determinantGamma/determinantBase>0.0 && determinantBeta/determinantBase+determinantGamma/determinantBase<1.0) {
            /* intersection point lies within the boundary of the triangle */
            tMin = determinantT/determinantBase;
        } else {
            /* intersection point does not lie within the boundary of the triangle */
            tMin = INF;
        }
    }

    if(level == 0) {
        return tMin;
    }

    /* illuminating with Phong Lighting Model */
    Vector intersectionPoint = ray.getRO()+ray.getRD()*tMin;
    Color intersectionPointColor = getColor();

    /* determining unit normal vector on appropriate side of triangle */
    Vector normal = (b-a)%(c-a);
    normal.normalize();
    normal = ((ray.getRD()*(-1.0))/normal > 0.0)? normal: normal*(-1.0);

    /* computing ambient light component of reflected ray */
    computeAmbientLightComponent(color, intersectionPointColor);

    /* computing diffuse & specular reflection components of reflected ray */
    for(int i=0; i<lights.size(); i++) {
        Ray incidentRay(lights[i].getPosition(), intersectionPoint-lights[i].getPosition());

        /* checking if intersection point is in shadow */
        double t, tMinimum=INF;

        for(int j=0; j<objects.size(); j++) {
            Color dummyColor;  // color = black
            t = objects[j]->intersect(incidentRay, dummyColor, 0);

            if(t>0.0 && t<tMinimum) {
                tMinimum = t;
            }
        }

        Vector shadowIntersectionPoint = incidentRay.getRO()+incidentRay.getRD()*tMinimum;
        double epsilon = 0.0000001;  // for tuning light effect

        if(intersectionPoint.computeDistanceBetween(incidentRay.getRO())-epsilon > shadowIntersectionPoint.computeDistanceBetween(incidentRay.getRO())) {
            /* intersection point is, indeed, in shadow */
            continue;
        }

        /* computing diffuse & specular components of reflected ray */
        computeReflectionComponents(ray, color, intersectionPoint, intersectionPointColor, normal, lights[i], incidentRay);
    }

    /* handling recursive reflection */
    if(level >= recursionLevel) {
        return tMin;
    }

    /* incorporating concept of evil epsilon to recursive reflection computation */
    Vector reflectionDirection = ray.getRD()-normal*((ray.getRD()/normal)*2.0);
    reflectionDirection.normalize();
    Ray reflectedRay(intersectionPoint+reflectionDirection, reflectionDirection);

    /* finding nearest intersecting object (if available) */
    int nearest = INT_MAX;
    double t, tMinimum=INF;

    for(int i=0; i<objects.size(); i++) {
        Color dummyColor;  // color = black
        t = objects[i]->intersect(reflectedRay, dummyColor, 0);

        if(t>0.0 && t<tMinimum) {
            tMinimum = t;
            nearest = i;
        }
    }

    /* finding color component for reflected ray */
    Color reflectedColor;  // color = black

    if(nearest != INT_MAX) {
        tMinimum = objects[nearest]->intersect(reflectedRay, reflectedColor, level+1);
    }

    /* computing recursive reflection component of reflected ray */
    computeRecursiveReflectionComponent(color, reflectedColor);

    /* clipping the color values (if necessary) */
    color.red = (color.red > 1.0)? 1.0: ((color.red < 0.0)? 0.0: color.red);
    color.green = (color.green > 1.0)? 1.0: ((color.green < 0.0)? 0.0: color.green);
    color.blue = (color.blue > 1.0)? 1.0: ((color.blue < 0.0)? 0.0: color.blue);

    return tMin;
}

/* GeneralQuadricSurfaceCoefficient structure */
struct GeneralQuadricSurfaceCoefficient {
    double a, b, c, d, e, f, g, h, i, j;

    friend ifstream& operator>>(ifstream&, GeneralQuadricSurfaceCoefficient&);
    friend ostream& operator<<(ostream&, GeneralQuadricSurfaceCoefficient&);
};

ifstream& operator>>(ifstream &input, GeneralQuadricSurfaceCoefficient &coefficient) {
    input >> coefficient.a >> coefficient.b >> coefficient.c >> coefficient.d >> coefficient.e;
    input >> coefficient.f >> coefficient.g >> coefficient.h >> coefficient.i >> coefficient.j;
    return input;
}

ostream& operator<<(ostream &output, GeneralQuadricSurfaceCoefficient &coefficient) {
    output << '[' << coefficient.a << ", " << coefficient.b << ", ";
    output << coefficient.c << ", " << coefficient.d << ", ";
    output << coefficient.e << ", " << coefficient.f << ", ";
    output << coefficient.g << ", " << coefficient.h << ", ";
    output << coefficient.i << ", " << coefficient.j << ']';
    return output;
}

/* GeneralQuadricSurface class */
class GeneralQuadricSurface: public Object {
    GeneralQuadricSurfaceCoefficient coefficient;
    Vector cubeReferencePoint;
    double length;
    double width;
    double height;

public:
    GeneralQuadricSurface() {
        coefficient.a = coefficient.b = coefficient.c = coefficient.d = coefficient.e = 0.0;
        coefficient.f = coefficient.g = coefficient.h = coefficient.i = coefficient.j = 0.0;
        length = width = height = 0.0;
    }

    GeneralQuadricSurface(GeneralQuadricSurfaceCoefficient coefficient, Vector cubeReferencePoint, double length, double width, double height) {
        this->coefficient = coefficient;
        this->cubeReferencePoint = cubeReferencePoint;
        this->length = length;
        this->width = width;
        this->height = height;
    }

    void draw() {
        /* draw(): implemented from base class Object */
    }

    double intersect(Ray, Color&, int);

    ~GeneralQuadricSurface() {
        /* destructor */
    }
};

double GeneralQuadricSurface::intersect(Ray ray, Color& color, int level) {
    /* finding intersecting tMin */
    /* reference: http://skuld.bmsc.washington.edu/people/merritt/graphics/quadrics.html */
    double a, b, c, tMin, tMax;

    a = coefficient.a*ray.getRD().getX()*ray.getRD().getX()+coefficient.b*ray.getRD().getY()*ray.getRD().getY()+coefficient.c*ray.getRD().getZ()*ray.getRD().getZ();
    a += coefficient.d*ray.getRD().getX()*ray.getRD().getY()+coefficient.e*ray.getRD().getX()*ray.getRD().getZ()+coefficient.f*ray.getRD().getY()*ray.getRD().getZ();

    b = 2.0*coefficient.a*ray.getRO().getX()*ray.getRD().getX()+2.0*coefficient.b*ray.getRO().getY()*ray.getRD().getY()+2.0*coefficient.c*ray.getRO().getZ()*ray.getRD().getZ();
    b += coefficient.d*(ray.getRO().getX()*ray.getRD().getY()+ray.getRD().getX()*ray.getRO().getY());
    b += coefficient.e*(ray.getRO().getX()*ray.getRD().getZ()+ray.getRD().getX()*ray.getRO().getZ());
    b += coefficient.f*(ray.getRO().getY()*ray.getRD().getZ()+ray.getRD().getY()*ray.getRO().getZ());
    b += coefficient.g*ray.getRD().getX()+coefficient.h*ray.getRD().getY()+coefficient.i*ray.getRD().getZ();

    c = coefficient.a*ray.getRO().getX()*ray.getRO().getX()+coefficient.b*ray.getRO().getY()*ray.getRO().getY()+coefficient.c*ray.getRO().getZ()*ray.getRO().getZ();
    c += coefficient.d*ray.getRO().getX()*ray.getRO().getY()+coefficient.e*ray.getRO().getX()*ray.getRO().getZ()+coefficient.f*ray.getRO().getY()*ray.getRO().getZ();
    c += coefficient.g*ray.getRO().getX()+coefficient.h*ray.getRO().getY()+coefficient.i*ray.getRO().getZ()+coefficient.j;

    if(a == 0.0) {
        tMin = (b == 0.0)? INF: -c/b;
        tMax = INF;
    } else {
        double discriminant = b*b-4.0*a*c;

        if(discriminant < 0.0) {
            tMin = tMax = INF;
        } else if(discriminant > 0.0) {
            tMax = -b/(2.0*a)+sqrt(discriminant)/(2.0*a);
            tMin = -b/(2.0*a)-sqrt(discriminant)/(2.0*a);
        } else {
            tMin = -b/(2.0*a);
            tMax = INF;
        }
    }

    /* clipping general quadric surface along the dimensions (if necessary) */
    if(tMin < INF) {
        if(tMax < INF) {
            if(tMin > 0.0) {
                Vector intersectionPoint = ray.getRO()+ray.getRD()*tMin;

                if((length!=0.0 && (intersectionPoint.getX()<cubeReferencePoint.getX() || intersectionPoint.getX()>cubeReferencePoint.getX()+length)) || (width!=0.0 && (intersectionPoint.getY()<cubeReferencePoint.getY() || intersectionPoint.getY()>cubeReferencePoint.getY()+width)) || (height!=0.0 && (intersectionPoint.getZ()<cubeReferencePoint.getZ() || intersectionPoint.getZ()>cubeReferencePoint.getZ()+height))) {
                    tMin = INF;
                }
            }
            if(tMax > 0.0) {
                Vector intersectionPoint = ray.getRO()+ray.getRD()*tMax;

                if((length!=0.0 && (intersectionPoint.getX()<cubeReferencePoint.getX() || intersectionPoint.getX()>cubeReferencePoint.getX()+length)) || (width!=0.0 && (intersectionPoint.getY()<cubeReferencePoint.getY() || intersectionPoint.getY()>cubeReferencePoint.getY()+width)) || (height!=0.0 && (intersectionPoint.getZ()<cubeReferencePoint.getZ() || intersectionPoint.getZ()>cubeReferencePoint.getZ()+height))) {
                    tMax = INF;
                }
            }
            tMin = (tMin>0.0 && tMin<tMax)? tMin: tMax;
        } else {
            if(tMin > 0.0) {
                Vector intersectionPoint = ray.getRO()+ray.getRD()*tMin;

                if((length!=0.0 && (intersectionPoint.getX()<cubeReferencePoint.getX() || intersectionPoint.getX()>cubeReferencePoint.getX()+length)) || (width!=0.0 && (intersectionPoint.getY()<cubeReferencePoint.getY() || intersectionPoint.getY()>cubeReferencePoint.getY()+width)) || (height!=0.0 && (intersectionPoint.getZ()<cubeReferencePoint.getZ() || intersectionPoint.getZ()>cubeReferencePoint.getZ()+height))) {
                    tMin = INF;
                }
            }
        }
    }

    if(level == 0) {
        return tMin;
    }

    /* illuminating with Phong Lighting Model */
    Vector intersectionPoint = ray.getRO()+ray.getRD()*tMin;
    Color intersectionPointColor = getColor();

    /* determining unit normal vector at intersection point on appropriate side of general quadric surface */
    double xNormal, yNormal, zNormal;

    xNormal = 2.0*coefficient.a*intersectionPoint.getX()+coefficient.d*intersectionPoint.getY();
    xNormal += coefficient.e*intersectionPoint.getZ()+coefficient.g;

    yNormal = 2.0*coefficient.b*intersectionPoint.getY()+coefficient.d*intersectionPoint.getX();
    yNormal += coefficient.f*intersectionPoint.getZ()+coefficient.h;

    zNormal = 2.0*coefficient.c*intersectionPoint.getZ()+coefficient.e*intersectionPoint.getX();
    zNormal += coefficient.f*intersectionPoint.getY()+coefficient.i;

    Vector normal(xNormal, yNormal, zNormal);
    normal.normalize();
    normal = ((ray.getRD()*(-1.0))/normal > 0.0)? normal: normal*(-1.0);

    /* computing ambient light component of reflected ray */
    computeAmbientLightComponent(color, intersectionPointColor);

    /* computing diffuse & specular reflection components of reflected ray */
    for(int i=0; i<lights.size(); i++) {
        Ray incidentRay(lights[i].getPosition(), intersectionPoint-lights[i].getPosition());

        /* checking if intersection point is in shadow */
        double t, tMinimum=INF;

        for(int j=0; j<objects.size(); j++) {
            Color dummyColor;  // color = black
            t = objects[j]->intersect(incidentRay, dummyColor, 0);

            if(t>0.0 && t<tMinimum) {
                tMinimum = t;
            }
        }

        Vector shadowIntersectionPoint = incidentRay.getRO()+incidentRay.getRD()*tMinimum;
        double epsilon = 0.0000001;  // for tuning light effect

        if(intersectionPoint.computeDistanceBetween(incidentRay.getRO())-epsilon > shadowIntersectionPoint.computeDistanceBetween(incidentRay.getRO())) {
            /* intersection point is, indeed, in shadow */
            continue;
        }

        /* computing diffuse & specular components of reflected ray */
        computeReflectionComponents(ray, color, intersectionPoint, intersectionPointColor, normal, lights[i], incidentRay);
    }

    /* handling recursive reflection */
    if(level >= recursionLevel) {
        return tMin;
    }

    /* incorporating concept of evil epsilon to recursive reflection computation */
    Vector reflectionDirection = ray.getRD()-normal*((ray.getRD()/normal)*2.0);
    reflectionDirection.normalize();
    Ray reflectedRay(intersectionPoint+reflectionDirection, reflectionDirection);

    /* finding nearest intersecting object (if available) */
    int nearest = INT_MAX;
    double t, tMinimum=INF;

    for(int i=0; i<objects.size(); i++) {
        Color dummyColor;  // color = black
        t = objects[i]->intersect(reflectedRay, dummyColor, 0);

        if(t>0.0 && t<tMinimum) {
            tMinimum = t;
            nearest = i;
        }
    }

    /* finding color component for reflected ray */
    Color reflectedColor;  // color = black

    if(nearest != INT_MAX) {
        tMinimum = objects[nearest]->intersect(reflectedRay, reflectedColor, level+1);
    }

    /* computing recursive reflection component of reflected ray */
    computeRecursiveReflectionComponent(color, reflectedColor);

    /* clipping the color values (if necessary) */
    color.red = (color.red > 1.0)? 1.0: ((color.red < 0.0)? 0.0: color.red);
    color.green = (color.green > 1.0)? 1.0: ((color.green < 0.0)? 0.0: color.green);
    color.blue = (color.blue > 1.0)? 1.0: ((color.blue < 0.0)? 0.0: color.blue);

    return tMin;
}

/* Floor class */
class Floor: public Object {
    double floorWidth;
    double tileWidth;

    /* considering color from base class Object as background color */
    Color foregroundColor;

public:
    Floor() {
        floorWidth = tileWidth = 0.0;
    }

    Floor(double floorWidth, double tileWidth, Color foregroundColor) {
        this->floorWidth = floorWidth;
        this->tileWidth = tileWidth;
        this->foregroundColor = foregroundColor;
    }

    void draw();
    double intersect(Ray, Color&, int);

    ~Floor() {
        /* destructor */
    }
};

void Floor::draw() {
    for(int i=0, row=(int) floorWidth/tileWidth, column=(int) floorWidth/tileWidth; i<row; i++) {
        for(int j=0; j<column; j++) {
            /* drawing square on a plane parallel to x-y plane */
            glColor3f(((i+j)%2 == 0)? getColor().red: foregroundColor.red, ((i+j)%2 == 0)? getColor().green: foregroundColor.green, ((i+j)%2 == 0)? getColor().blue: foregroundColor.blue);
            Vector leftBottomCorner(-floorWidth/2.0+tileWidth*j, -floorWidth/2.0+tileWidth*i, 0.0);

            glBegin(GL_QUADS);
            {
                glVertex3f(leftBottomCorner.getX(), leftBottomCorner.getY(), leftBottomCorner.getZ());
                glVertex3f(leftBottomCorner.getX()+tileWidth, leftBottomCorner.getY(), leftBottomCorner.getZ());
                glVertex3f(leftBottomCorner.getX()+tileWidth, leftBottomCorner.getY()+tileWidth, leftBottomCorner.getZ());
                glVertex3f(leftBottomCorner.getX(), leftBottomCorner.getY()+tileWidth, leftBottomCorner.getZ());
            }
            glEnd();
        }
    }
}

double Floor::intersect(Ray ray, Color& color, int level) {
    /* determining unit normal vector on appropriate side of floor */
    Vector normal(0.0, 0.0, 1.0);
    normal = (position/normal > 0.0)? normal: normal*(-1.0);

    /* finding intersecting tMin */
    double tMin = INF;

    if(normal/ray.getRD() != 0.0) {
        tMin = (-1.0)*(normal/ray.getRO())/(normal/ray.getRD());
    }

    if(tMin>0.0 && tMin<INF) {
        /*
            ray intersects the floor plane and is in front of the camera,
                but we need to make sure the intersection point is on the floor
        */
        Vector intersectionPoint = ray.getRO()+ray.getRD()*tMin;

        if(!((intersectionPoint.getX()>-floorWidth/2.0 && intersectionPoint.getX()<floorWidth/2.0) && (intersectionPoint.getY()>-floorWidth/2.0 && intersectionPoint.getY()<floorWidth/2.0))) {
            /* intersection point is not on the floor */
            tMin = INF;
        }
    }

    if(level == 0) {
        return tMin;
    }

    /* illuminating with Phong Lighting Model */
    Vector intersectionPoint = ray.getRO()+ray.getRD()*tMin;
    Vector referencePosition = intersectionPoint-Vector(-floorWidth/2.0, -floorWidth/2.0, 0.0);
    Color intersectionPointColor = (((int) (floor(referencePosition.getX()/tileWidth)+floor(referencePosition.getY()/tileWidth)))%2 == 0)? getColor(): foregroundColor;

    /* computing ambient light component of reflected ray */
    computeAmbientLightComponent(color, intersectionPointColor);

    /* computing diffuse & specular reflection components of reflected ray */
    for(int i=0; i<lights.size(); i++) {
        Ray incidentRay(lights[i].getPosition(), intersectionPoint-lights[i].getPosition());

        /* checking if intersection point is in shadow */
        double t, tMinimum=INF;

        for(int j=0; j<objects.size(); j++) {
            Color dummyColor;  // color = black
            t = objects[j]->intersect(incidentRay, dummyColor, 0);

            if(t>0.0 && t<tMinimum) {
                tMinimum = t;
            }
        }

        Vector shadowIntersectionPoint = incidentRay.getRO()+incidentRay.getRD()*tMinimum;
        double epsilon = 0.0000001;  // for tuning light effect

        if(intersectionPoint.computeDistanceBetween(incidentRay.getRO())-epsilon > shadowIntersectionPoint.computeDistanceBetween(incidentRay.getRO())) {
            /* intersection point is, indeed, in shadow */
            continue;
        }

        /* computing diffuse & specular components of reflected ray */
        computeReflectionComponents(ray, color, intersectionPoint, intersectionPointColor, normal, lights[i], incidentRay);
    }

    /* handling recursive reflection */
    if(level >= recursionLevel) {
        return tMin;
    }

    /* incorporating concept of evil epsilon to recursive reflection computation */
    Vector reflectionDirection = ray.getRD()-normal*((ray.getRD()/normal)*2.0);
    reflectionDirection.normalize();
    Ray reflectedRay(intersectionPoint+reflectionDirection, reflectionDirection);

    /* finding nearest intersecting object (if available) */
    int nearest = INT_MAX;
    double t, tMinimum=INF;

    for(int i=0; i<objects.size(); i++) {
        Color dummyColor;  // color = black
        t = objects[i]->intersect(reflectedRay, dummyColor, 0);

        if(t>0.0 && t<tMinimum) {
            tMinimum = t;
            nearest = i;
        }
    }

    /* finding color component for reflected ray */
    Color reflectedColor;  // color = black

    if(nearest != INT_MAX) {
        tMinimum = objects[nearest]->intersect(reflectedRay, reflectedColor, level+1);
    }

    /* computing recursive reflection component of reflected ray */
    computeRecursiveReflectionComponent(color, reflectedColor);

    /* clipping the color values (if necessary) */
    color.red = (color.red > 1.0)? 1.0: ((color.red < 0.0)? 0.0: color.red);
    color.green = (color.green > 1.0)? 1.0: ((color.green < 0.0)? 0.0: color.green);
    color.blue = (color.blue > 1.0)? 1.0: ((color.blue < 0.0)? 0.0: color.blue);

    return tMin;
}
