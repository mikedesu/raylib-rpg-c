/** @file sprite.h
 *  @brief Lightweight animated sprite wrapper over a spritesheet texture.
 */

#pragma once

#include "massert.h"
#include "mprint.h"
#include <memory>
#include <raylib.h>

/**
 * @brief Runtime view into a texture atlas row/column animation.
 *
 * A `sprite` tracks the current frame and context row for a texture that is
 * arranged as `numcontexts` rows by `numframes` columns.
 */
class sprite {
private:
    Texture2D* texture;
    int width;
    int height;
    int numcontexts;
    int numframes;
    int currentframe;
    int currentcontext;
    int num_loops;
    bool stop_on_last_frame;
    bool is_animating;
    Rectangle src;

public:
    /** @brief Construct an empty sprite placeholder. */
    sprite() {
    }

    /**
     * @brief Construct a sprite from a spritesheet texture.
     * @param t Source texture.
     * @param nc Number of animation contexts/rows in the texture.
     * @param nf Number of animation frames/columns in the texture.
     */
    sprite(Texture* t, int nc, int nf)
        : texture(t)
        , numcontexts(nc)
        , numframes(nf) {
        massert(t, "texture t was null");
        currentframe = currentcontext = num_loops = 0;
        width = t->width / numframes;
        height = t->height / numcontexts;
        stop_on_last_frame = false;
        is_animating = true;
        // setting the source of the texture is about which frame and context we are on
        // context is the y access aka which row of sprites
        // frame is the x access aka which column of sprites
        // to calculate the source rectangle we need to know the width and height of the sprite
        // given the current frame and current context,
        // the width is the current frame times the width of the sprite
        src = Rectangle{
            static_cast<float>(currentframe * width), static_cast<float>(currentcontext * height), static_cast<float>(width), static_cast<float>(height)};
    }

    /** @brief Destroy the sprite wrapper without owning texture teardown. */
    ~sprite() {
    }

    /** @brief Advance the current animation frame and wrap or stop as configured. */
    void incr_frame() {
        if (is_animating) {
            currentframe++;
            if (stop_on_last_frame && currentframe == numframes - 1) {
                is_animating = false;
                src.x = width * currentframe;
            }
            else if (currentframe >= numframes) {
                currentframe = 0;
                num_loops++;
            }
            src.x = width * currentframe;
        }
    }

    /** @brief Return the width in pixels of one animation frame. */
    int get_width() {
        return width;
    }

    /** @brief Return the height in pixels of one animation context row. */
    int get_height() {
        return height;
    }

    /** @brief Override the cached frame width. */
    void set_width(int w) {
        width = w;
    }

    /** @brief Override the cached frame height. */
    void set_height(int h) {
        height = h;
    }

    /** @brief Return the number of context rows available in the spritesheet. */
    int get_numcontexts() {
        return numcontexts;
    }

    /** @brief Return the number of frames per context row. */
    int get_numframes() {
        return numframes;
    }

    /** @brief Return the active frame index. */
    int get_currentframe() {
        return currentframe;
    }

    /** @brief Return the active context-row index. */
    int get_currentcontext() {
        return currentcontext;
    }

    /** @brief Return how many times this animation has looped. */
    int get_num_loops() {
        return num_loops;
    }

    /** @brief Set the active context-row index without recomputing animation state. */
    void set_currentcontext(int n) {
        currentcontext = n;
    }

    /** @brief Set the active frame index without recomputing animation state. */
    void set_currentframe(int n) {
        currentframe = n;
    }

    /** @brief Override the tracked loop count. */
    void set_num_loops(int n) {
        num_loops = n;
    }

    /** @brief Reset the frame index and loop count to the start of the animation. */
    void zero_currentframe_numloops() {
        currentframe = 0;
        num_loops = 0;
    }

    /** @brief Return the backing texture pointer. */
    Texture2D* get_texture() {
        return texture;
    }

    /** @brief Return the source rectangle currently used for rendering. */
    Rectangle get_src() {
        return src;
    }

    /** @brief Replace the cached source rectangle. */
    void set_src(Rectangle r) {
        src = r;
    }

    /** @brief Override only the source-rectangle x coordinate. */
    void set_src_x(float x) {
        src.x = x;
    }

    /** @brief Override only the source-rectangle y coordinate. */
    void set_src_y(float y) {
        src.y = y;
    }

    /** @brief Return whether frame advancement is currently enabled. */
    bool get_is_animating() {
        return is_animating;
    }

    /** @brief Enable or disable frame advancement. */
    void set_is_animating(bool b) {
        is_animating = b;
    }

    /** @brief Return whether the animation should stop on the last frame. */
    bool get_stop_on_last_frame() {
        return stop_on_last_frame;
    }

    /** @brief Configure whether the animation halts on its last frame. */
    void set_stop_on_last_frame(bool b) {
        stop_on_last_frame = b;
    }

    /**
     * @brief Switch to a new context row and reset frame playback if animating.
     * @param ctx Zero-based context row within the spritesheet.
     */
    void set_context(int ctx) {
        massert(ctx >= 0, "set_context: ctx is less than 0: %d", ctx);
        massert(ctx < numcontexts, "set_context: ctx is greater than numcontexts: %d < %d", ctx, numcontexts);
        currentcontext = ctx % numcontexts;
        src.y = height * currentcontext;

        // we dont want to reset frame or src.x IFF...the sprite/entity is dead or set to not animating
        if (is_animating) {
            currentframe = 0;
            src.x = 0;
        }
    }
};
