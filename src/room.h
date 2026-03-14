/** @file room.h
 *  @brief Lightweight named room metadata and rectangle ownership.
 */

#pragma once

#include <raylib.h>
#include <string>



using std::string;

/// @brief Integer identifier used to distinguish rooms on a floor.
typedef int room_id;

/**
 * @brief Named rectangular room descriptor used during dungeon generation.
 *
 * Stores a room id, display metadata, and the rectangle the room occupies.
 */
class room
{
private:
    room_id id;
    string name;
    string description;
    Rectangle area;

public:
    /** @brief Return the room id. */
    room_id get_id() const
    {
        return id;
    }

    /** @brief Assign the room id. */
    void set_id(room_id i)
    {
        id = i;
    }

    /** @brief Return the room height in tiles/pixels matching the rectangle units. */
    float get_h() const
    {
        return area.height;
    }

    /** @brief Return the room width in tiles/pixels matching the rectangle units. */
    float get_w() const
    {
        return area.width;
    }

    /** @brief Return the room's top-left y coordinate. */
    float get_y() const
    {
        return area.y;
    }

    /** @brief Return the room's top-left x coordinate. */
    float get_x() const
    {
        return area.x;
    }

    /** @brief Replace the room rectangle. */
    void set_area(Rectangle r)
    {
        area = r;
    }

    /** @brief Return the room rectangle. */
    Rectangle get_area() const
    {
        return area;
    }

    /** @brief Return the room description text. */
    string get_description() const
    {
        return description;
    }

    /** @brief Set the room description text. */
    void set_description(string n)
    {
        description = n;
    }

    /** @brief Return the room name. */
    string get_name() const
    {
        return name;
    }

    /** @brief Set the room name. */
    void set_name(string n)
    {
        name = n;
    }

    /** @brief Construct a fully specified room. */
    room(room_id i, string n, string d, Rectangle a)
        : id(i)
        , name(n)
        , description(d)
        , area(a)
    {
    }

    /** @brief Construct a room with an unspecified id. */
    room(string n, string d, Rectangle a)
        : name(n)
        , description(d)
        , area(a)
    {
        id = -1;
    }

    /** @brief Construct a room with a default description and unspecified id. */
    room(string n, Rectangle a)
        : name(n)
        , area(a)
    {
        id = -1;
        description = "unnamed-room-description";
    }

    /** @brief Construct an unnamed room for a rectangle. */
    room(Rectangle a)
        : area(a)
    {
        id = -1;
        name = "unnamed-room";
        description = "unnamed-room-description";
    }

    /** @brief Construct a default empty unnamed room. */
    room()
    {
        id = -1;
        name = "unnamed-room";
        description = "unnamed-room-description";
        area = Rectangle{0, 0, 0, 0};
    }
};
