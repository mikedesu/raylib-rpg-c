#pragma once

#include <raylib.h>
#include <string>



using std::string;



typedef int room_id;




class room
{
private:
    room_id id;
    string name;
    string description;
    Rectangle area;

public:
    room_id get_id()
    {
        return id;
    }




    void set_id(room_id i)
    {
        id = i;
    }




    float get_h()
    {
        return area.height;
    }




    float get_w()
    {
        return area.width;
    }




    float get_y()
    {
        return area.y;
    }




    float get_x()
    {
        return area.x;
    }




    void set_area(Rectangle r)
    {
        area = r;
    }




    Rectangle get_area()
    {
        return area;
    }




    string get_description()
    {
        return description;
    }




    void set_description(string n)
    {
        description = n;
    }




    string get_name()
    {
        return name;
    }




    void set_name(string n)
    {
        name = n;
    }




    room(room_id i, string n, string d, Rectangle a)
        : id(i)
        , name(n)
        , description(d)
        , area(a)
    {
    }




    room(string n, string d, Rectangle a)
        : name(n)
        , description(d)
        , area(a)
    {
        id = -1;
    }




    room(string n, Rectangle a)
        : name(n)
        , area(a)
    {
        id = -1;
        description = "unnamed-room-description";
    }




    room(Rectangle a)
        : area(a)
    {
        id = -1;
        name = "unnamed-room";
        description = "unnamed-room-description";
    }




    room()
    {
        id = -1;
        name = "unnamed-room";
        description = "unnamed-room-description";
        area = Rectangle{0, 0, 0, 0};
    }
};
