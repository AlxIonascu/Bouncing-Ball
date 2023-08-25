 
#pragma warning( disable : 4996 )

#include <cstdlib>
#include <vector>
#include <iostream>
#include <string>
#include "G2D.h"
#include <math.h>

using namespace std;

struct Bumper
{
    float radius;
    V2 center;
    double time;
};

struct Segment{
    V2 P1;
    V2 P2;
    bool untouched;
};
// information de partie

struct GameData
{
    int idFrame = 0;
    int HeighPix = 800;
    int WidthPix = 600;
    float BallRadius = 15;
    V2 BallPos = V2(425, 300);
    V2 BallSpeed = V2(0, 600);
    int Score = 0;

    std::vector<Segment> segments;
    std::vector<Bumper>  bumpers;
   
};

GameData G;

V2 Rebond(V2 v, V2 n)
{
    n.normalize();
    V2 t(n.y, -n.x);  // rotation de 90° du vecteur n
    float vt = v / t;     // produit scalaire, vt est un float
    float vn = v / n;     // produit scalaire, vn est un float
    V2 r = vt * t - vn * n; // * entre un flottant et un V2
    return r;
}

float Distance(V2 point1, V2 point2)
{
    return sqrt((point1.x - point2.x) * (point1.x - point2.x) + (point1.y - point2.y) * (point1.y - point2.y));
}

bool SameSide(V2 A, V2 B, V2 M, V2 P) {
    float r1 = (B-A) ^ (M-A);
    float r2 = (B-A) ^ (P-A);
    if (r1 >= 0 && r2 >= 0) {
        return true;
    }
    if (r1 <= 0 && r2 <= 0) {
        return true;
    }
    return false;
}

// 0 : orthogonal
// + : same direction
// - : opposite direction
bool  SameDirection(V2 u, V2 v) {
  float dir= u.x * v.x + u.y * v.y;
  if (dir > 0) return true;
  else return false;
}

bool OppositeDirection(V2 u, V2 v) {
    float dir = u.x * v.x + u.y * v.y;
    if (dir < 0) return true;
    else return false;
}

float DistPointSegment(V2 AB, V2 A, V2 P) {
    V2 T = AB;
    V2 N = V2(T.y, -T.x);
    N.normalize();
    V2 AP = A - P;
    return abs(AP / N);

}

bool  InterSegmentCercle(V2 A, V2 B, V2 Center) {

    if (Distance(A, Center) <= G.BallRadius) return true;  // zone 1
    if (Distance(B, Center) <= G.BallRadius) return true;  // zone 3
    if (SameDirection(B - A, Center - A) && SameDirection(A-B, Center-B)) { // C en zone 2
        if (DistPointSegment(B-A, A, Center) <= G.BallRadius) return true;

           
    }
    return false;
}





void render()
{
    G2D::ClearScreen(Color::Black);
    G2D::DrawStringFontMono(V2(20, G.HeighPix - 70), string("Score: " + std::to_string(G.Score)), 30, 5, Color::Blue);
    G2D::DrawRectangle(V2(0, 0), V2(G.WidthPix, G.HeighPix), Color::Green);
    G2D::DrawCircle(G.BallPos, 15, Color::Red, true);

    for (int i = 0; i < G.bumpers.size(); i++) {
        if (G.bumpers[i].time > 0) {
            G2D::DrawCircle(G.bumpers[i].center, G.bumpers[i].radius + (G.bumpers[i].time * 5), Color::Red, true);
            G.bumpers[i].time -= G2D::ElapsedTimeFromLastCallbackSeconds();
        }
        G2D::DrawCircle(G.bumpers[i].center, G.bumpers[i].radius, Color::Yellow, true);
    }

    Color c = Color::Green;
    for (int i = 0; i < G.segments.size(); i++) {
        if (G.segments[i].untouched)
            c = Color::Red;
        else c = Color::Green;
        G2D::DrawLine(G.segments[i].P1, G.segments[i].P2, c);
    }
    G2D::Show();
}



void Logic() {
    G.idFrame += 1;
    double dt = G2D::ElapsedTimeFromLastCallbackSeconds();
    V2 NewPos = G.BallPos + G.BallSpeed * dt;
    V2 NewSpd = G.BallSpeed;
    bool collision = false;

    if (NewPos.y > 800)
    {
        collision = true;
        V2 n = V2(0, -1);
        NewPos = G.BallPos;
        NewSpd = Rebond(NewSpd, n);
    }

    if (NewPos.y < 0)
    {
        collision = true;
        V2 n = V2(0, 1);
        NewPos = G.BallPos;
        NewSpd = Rebond(NewSpd, n);
    }

    for (int i = 0; i < G.bumpers.size(); i++) {
        if (Distance(G.bumpers[i].center, NewPos) <= G.BallRadius + G.bumpers[i].radius)
        {
            collision = true;
            V2 n = G.BallPos - G.bumpers[i].center;
            NewPos = G.BallPos;
            NewSpd = Rebond(NewSpd, n);
            G.bumpers[i].time = 2;
            G.Score = G.Score + 100;
        }
    }
 
    if (NewPos.x - 15 <= 0) {
        collision = true;
        V2 n = V2(-1, 0);
        NewPos = G.BallPos;
        NewSpd = Rebond(NewSpd, n);
    }

    if (NewPos.x + 15 >= G.WidthPix) {
        collision = true;
        V2 n = V2(1, 0);
        NewPos = G.BallPos;
        NewSpd = Rebond(NewSpd, n);
    }

    for (int i = 0; i < G.segments.size(); i++) {
        if (InterSegmentCercle(G.segments[i].P1, G.segments[i].P2, NewPos) && G.segments[i].untouched) {
            collision = true;
            V2 n_neg = V2(1, 1);
            V2 n_pos = V2(-1, 1);
            //if angle of segment with abcissa is > 90 degrees, i.e. dotproduct is negative
            V2 seg = G.segments[i].P2 - G.segments[i].P1;
            V2 abscissa = V2(1, 0);
            float dotProd = seg / abscissa;
            if ( dotProd < 0) {
                if (OppositeDirection(seg, NewSpd))
                    n_neg = V2(-1, -1);
                NewSpd = Rebond(NewSpd, n_neg);
            }
            else {
                //if angle of segment with abscissa is < 90 degrees, i.e. dot product is positive
                if (OppositeDirection(seg, NewSpd)) 
                    n_pos = V2(1, -1);
                 NewSpd = Rebond(NewSpd, n_pos);
            }
            NewPos = G.BallPos;
            G.segments[i].untouched = false;
            G.Score = G.Score + 500;
        }
    }
     if (collision == true)
    {
        G.BallSpeed = NewSpd;
    }
    else
    {
        G.BallPos = NewPos;
        G.BallSpeed = NewSpd;
    }
}

int main(int argc, char* argv[])
{
    G2D::InitWindow(argc, argv, V2(G.WidthPix, G.HeighPix), V2(200, 200), string("Super Flipper 600 !!"));
    G.segments.push_back({ V2(75, 525), V2(125,575), true });
    G.segments.push_back({ V2(150, 600), V2(200,650), true });
    G.segments.push_back({ V2(225, 675), V2(275,725), true });
    G.segments.push_back({ V2(375, 675), V2(325,725), true });
    G.segments.push_back({ V2(450, 600), V2(400,650), true });
    G.segments.push_back({ V2(525, 525), V2(475,575), true });

    G.bumpers.push_back({40, V2(200,400), 0});
    G.bumpers.push_back({40, V2(400,400), 0});
    G.bumpers.push_back({ 40, V2(300,550), 0 });
    _sleep(3000);
    G2D::Run(Logic, render);
}
