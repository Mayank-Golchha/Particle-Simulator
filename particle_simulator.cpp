#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstdlib>
#include <ctime> 
#include <vector>
#include <math.h>
#include <vector>

using namespace std;

// float TIME_STAMP = 0.08;
float TIME_STAMP = 0.015;
// int FRAME_RATE = 240;
// int FRAME_RATE = 440;
int FRAME_RATE = 2000;
unsigned int WIDTH = 700;
unsigned int HEIGHT = 600;

class Ball{
    public:
        float x,y,radius,vx = 0,vy = 0;
        sf::Color color;

        Ball(float x_,float y_,float radius_,sf::Color color_){
            x = x_;
            y = y_;
            radius = radius_;
            color = color_;
        }

        void stop_particle(){
            if (abs(vx) < 0.05) vx = 0;
            if (abs(vy) < 0.05) vy = 0;
        }

        void update(float E,float force,float f_coor_x,float f_coor_y,int clicked){
            float ay = 10.0;
            float ax = 0;

            if (force && f_coor_x >=0 && f_coor_y >= 0 && clicked){
                float thetha = atan2(y-f_coor_y,x-f_coor_x);
                ax += force*cos(thetha);
                ay += force*sin(thetha);
            }

            vx += ax*TIME_STAMP;
            vy += ay*TIME_STAMP;

            // vx *= 0.98f;
            // vy *= 0.98f;

            x += vx * TIME_STAMP + 0.5*ax*TIME_STAMP*TIME_STAMP; 
            y += vy * TIME_STAMP + 0.5*ay*TIME_STAMP*TIME_STAMP; 
            
            if (y > HEIGHT - 2*radius){
                y = HEIGHT-2*radius;
                vy *= -1*E;
            }
            else if (y < radius){
                y = radius;
                vy *= -1*E;
            }

            if (x > WIDTH - 2*radius){
                x = WIDTH-2*radius;
                vx *= -1*E;
            }
            else if (x < radius){
                x = radius;
                vx *= -1*E;
            }
            stop_particle();
        }

        void collide(Ball*,float);

        sf::CircleShape ball(){
            sf::CircleShape b(radius);
            b.setFillColor(color);
            b.setPosition({x,y});
            return b;
        }

};

class Vector{
    public:
        float x;
        float y;
        Vector(float x_,float y_){
            x = x_;
            y = y_;
        }

        float distance(Vector particle){
            float dx = particle.x - x;
            float dy = particle.y - y;
            return sqrt(dx*dx + dy*dy);
        }

        float dot(Vector particle){
            return particle.x*x+particle.y*y;
        }

        float magnitude(){
            return sqrt(x*x+y*y);
        }

        Vector multiply(float no){
            return Vector(x*no,y*no);
        }

        Vector add(Vector particle){
            return Vector(particle.x + x,particle.y + y);
        }

        Vector sub(Vector particle){
            return Vector(x - particle.x,y - particle.y);
        }

        Vector div(float a){
            return Vector(x / a,y / a);
        }

        Vector normalize(){
            //to calculate unit vector
            float m = magnitude();
            return Vector(x/m,y/m); // now magnitude = 1
        }
};


void Ball::collide(Ball *particle,float E){
    float dx = particle->x - x;
    float dy = particle->y - y;
    float distance = sqrt(dx*dx + dy*dy);

    if (distance < particle->radius + radius){
        float c = (1+E)/2;
        float overlap = particle->radius + radius - distance;
        float nx = dx / distance;
        float ny = dy / distance;

        Vector V1(vx,vy);
        Vector V2(particle->vx,particle->vy);
        Vector X1(x,y);
        Vector X2(particle->x,particle->y);

        //dynamic collision calculation
        Vector normal = X2.sub(X1);
        normal = normal.normalize();

        Vector Vrel = V1.sub(V2);

        Vector V1NEW = V1.sub(normal.multiply(Vrel.dot(normal)).multiply(c));
        Vrel = Vrel.multiply(-1);
        Vector V2NEW = V2.sub(normal.multiply(Vrel.dot(normal)).multiply(c));

        vx = V1NEW.x;
        vy = V1NEW.y;
        particle->vx = V2NEW.x;
        particle->vy = V2NEW.y;

        //static collision calculation
        x -= overlap * nx/2;
        y -= overlap * ny/2;
        particle->x += overlap * nx/2;
        particle->y += overlap * ny/2;

        Ball::stop_particle();
    }
}

vector<Ball> random_points(int count,float radius){
    //to generate random particles
    vector<Ball> particles;
    srand(time(0));
    for (int i = 0;i<count;i++){
        int r = rand()%256;
        int g = rand()%256;
        int b = rand()%256;
        float x = ((int)rand()%(WIDTH - (int)radius));
        float y = ((int)rand()%(HEIGHT - (int)radius));
        particles.push_back(Ball(x,y,radius,sf::Color(r,g,b)));
    }
    return particles;
}

int main(){
    sf::RenderWindow window(sf::VideoMode({WIDTH,HEIGHT}),"particle simulator");
    int count = 200;
    float radius = 5;
    float E = 0.8;
    float attractive_force = -50;
    int clicked = 0;
    int right_clicked = 0;

    int a = 10*radius,b = HEIGHT/radius + 1,c = WIDTH/radius + 1;

    sf::Vector2i mousePos({-1,-1});
    // Ball particles[2] = {Ball(100.f,100.f,radius,sf::Color::White),Ball(100.f,30.f,radius,sf::Color::White)};
    vector<Ball> particles = random_points(count,radius);
    

    window.clear(sf::Color::Black);
    // window.setFramerateLimit(80);
    // window.setFramerateLimit(FRAME_RATE);


    while (window.isOpen()){
        while (const std::optional event = window.pollEvent()){
            if (event->is<sf::Event::Closed>()) window.close();
            else if (event->is<sf::Event::MouseMoved>()){
                mousePos = sf::Mouse::getPosition(window);
            }
            else if (const auto* mousebuttonp = event->getIf<sf::Event::MouseButtonPressed>()){
                if (mousebuttonp->button == sf::Mouse::Button::Left){
                    mousePos = sf::Mouse::getPosition(window);
                    clicked = 1;
                }
                else if (mousebuttonp->button == sf::Mouse::Button::Right){
                    //right mouse click to produce a white ball
                    right_clicked = 1;
                }

            }
            else if (event->is<sf::Event::MouseButtonReleased>()){
                clicked = 0;
                right_clicked = 0;
            }
        }
        window.clear(sf::Color::Black);
        if (right_clicked){
            mousePos = sf::Mouse::getPosition(window);
            Ball p(mousePos.x,mousePos.y,radius,sf::Color::White);
            p.vy = 50;
            particles.push_back(p);
            count++;
        }
        for (int i = 0;i<count;i++){
            for (int j = i+1;j<count;j++){
            // for (int j = 0;j<count;j++){
                if (i != j) particles[i].collide(&particles[j],E);
            }
        }

        for (int i = 0;i<count;i++){
            window.draw(particles[i].ball());
        }

        for (int i = 0;i<count;i++){
            particles[i].update(E,attractive_force,mousePos.x,mousePos.y,clicked);
        }
        
        window.display();
    }
}