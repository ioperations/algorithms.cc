#pragma once

#include "fr_layout.h"
#include "canvas_app.h"

class Vertical_drawable_block : public Drawable {
    private:
        std::vector<Drawable*> drawables_;
    public:
        Vertical_drawable_block() = default;
        Vertical_drawable_block(const std::initializer_list<Drawable*>& l);
        ~Vertical_drawable_block();
        void add(Drawable* drawable);
        void draw(wxDC& dc, Painter& painter) override;
};

class Drawable_text : public Drawable {
    private:
        wxString text_;
        bool sizes_calculated_;
    public:
        Drawable_text(const std::string& text);
        void draw(wxDC& dc, Painter& painter) override;
};

class Drawable_graph : public Drawable {
    private:
        using Layout = Graph::Layout::Layout;
        struct Text_block {
            const Graph::Layout::Vertex_position* vertex_position_;
            wxCoord rect_width_;
            wxCoord rect_height_;
            wxCoord half_width_;
            wxCoord half_height_;
            wxCoord x_;
            wxCoord y_;

            Text_block(const Graph::Layout::Vertex_position* v);
            Text_block() = default;
            void calculate_sizes(wxDC& dc);
        };

        Layout layout_;
        std::vector<Text_block> text_blocks_;
        bool sizes_calculated_;
        wxCoord x_offset_;
        wxCoord y_offset_;
    public:
        Drawable_graph(Layout&& layout);
        void draw(wxDC& dc, Painter& painter) override;
};


