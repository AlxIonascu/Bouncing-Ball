#include "V2.h"


// comparaison sur des flottants... traitement spécial

bool operator == (const V2 & a, const V2 & b)
{
	V2 t = a - b;
	float epsilon = 0.001f;
	return t.norm() < epsilon;
}



// redéfinition des opérateurs standards

V2 operator + (const V2 & a, const V2 & b) { return V2(a.x + b.x, a.y + b.y); }
V2 operator - (const V2 & a, const V2 & b) { return V2(a.x - b.x, a.y - b.y); }
V2 operator * (float      a, const V2 & b) { return V2(a   * b.x, a   * b.y); }
V2 operator * (const V2 & a, float      b) { return V2(a.x * b, a.y * b); }
V2 operator * (const V2 & a, const V2 & b) { return V2(a.x*b.x, a.y*b.y); }
V2 operator / (const V2 & a, float      b) { return V2(a.x / b, a.y / b); }
float operator / (const V2& a, const V2& b) { return float(a.x * b.x + a.y * b.y); }//Scalaire
float operator ^ (const V2& a, const V2& b) { return float(a.x * b.y - a.y * b.x); }
std::ostream& operator << (std::ostream& os, V2& t) { return std::cout << "(" << t.x << "," << t.y << ")"; };