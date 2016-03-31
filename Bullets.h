#ifndef Point_H
#define Point_H

#include<stdio.h>
#include<cmath>

using namespace std;
class Bullets
{
  private:
            double xPos;
            double yPos;
            double zPos;
            double xDest;
            double yDest;
            double zDest;
            bool   draw;
            

  public:    
    Bullets(){
        xPos = yPos = zPos = xDest = yDest = zDest = 0.0;
    }
    
    void reset(){
        xPos = yPos = zPos = xDest = yDest = zDest = 0.0;
        draw = false;
    }
    void setPosition(double a, double b, double c){
        xPos = a;
        yPos = b;
        zPos = c;
    }
    
    void setDraw(bool a){
        draw = a;
    }
    
    bool getDraw(){
        return draw;
    }
    
    void setDestination(double a, double b, double c){
        xDest = a;
        yDest = b;
        zDest = c;
    }
    
    double getxDest(){
        return xDest;
    }
    double getyDest(){
        return yDest;
    }
    double getzDest(){
        return zDest;
    }
    double getxPos(){
        return xPos;
    }
    double getyPos(){
        return yPos;
    }
    double getzPos(){
        return zPos;
    }
};
#endif
