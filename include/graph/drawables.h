#pragma once

#include "canvas_app.h"
#include "fr_layout.h"

class Vertical_drawable_block : public Drawable {
   private:
    std::vector<Drawable*> drawables_;

   public:
    Vertical_drawable_block() = default;
    Vertical_drawable_block(const std::initializer_list<Drawable*>& l);
    ~Vertical_drawable_block();
    void add(Drawable* drawable);
    void calculate_size(wxDC& dc) override;
    void draw(wxDC& dc, Painter& painter) override;
};

class Drawable_text : public Drawable {
   private:
    wxString text_;

   public:
    Drawable_text(const std::string& text);
    void calculate_size(wxDC& dc) override;
    void draw(wxDC& dc, Painter& painter) override;
};

class Drawable_graph : public Drawable {
   private:
    struct Text_block {
        const Graph::Layout::Vertex_position* vertex_position_;
        wxCoord rect_width_;
        wxCoord rect_height_;
        wxCoord half_width_;
        wxCoord half_height_;
        wxCoord x_;
        wxCoord y_;

        Text_block(const Graph::Layout::Vertex_position* v, wxDC& dc);
        Text_block() = default;
    };

    using Layout = Graph::Layout::Layout;
    Layout layout_;
    std::vector<Text_block> text_blocks_;
    wxCoord x_offset_;
    wxCoord y_offset_;

   public:
    Drawable_graph(Layout&& layout);
    void calculate_size(wxDC& dc) override;
    void draw(wxDC& dc, Painter& painter) override;
};
