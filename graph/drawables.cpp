#include "drawables.h"

Vertical_drawable_block::Vertical_drawable_block(const std::initializer_list<Drawable*>& l)
    :drawables_(l)
{}

Vertical_drawable_block::~Vertical_drawable_block() {
    for (auto d : drawables_)
        delete d;
}

void Vertical_drawable_block::add(Drawable* drawable) {
    drawables_.push_back(drawable);
}

void Vertical_drawable_block::draw(wxDC& dc, Painter& painter) {
    for (auto d : drawables_) {
        d->draw(dc, painter);
        painter.shift_y_offset(d->height());
    }
}


Drawable_text::Drawable_text(const std::string& text) :text_(text), sizes_calculated_(false) {}

void Drawable_text::draw(wxDC& dc, Painter& painter) {
    if (!sizes_calculated_) {
        dc.GetTextExtent(text_, &width_, &height_);
        sizes_calculated_ = true;
    }
    painter.draw_text(dc, text_, 0, 0);
}


Drawable_graph::Drawable_graph(Layout&& layout) 
    :layout_(std::move(layout)), 
    text_blocks_(layout_.vertices_count()),
    sizes_calculated_(false), x_offset_(0), y_offset_(0) 
{}

void Drawable_graph::draw(wxDC& dc, Painter& painter) {
    if (!sizes_calculated_) {
        auto text_block = text_blocks_.begin();
        for (auto v = layout_.vertices_cbegin(); v != layout_.vertices_cend(); ++v) {
            *text_block = Text_block(&*v);
            text_block->calculate_sizes(dc);
            x_offset_ = std::max<wxCoord>(x_offset_, text_block->rect_width_ / 2 - v->x_);
            y_offset_ = std::max<wxCoord>(y_offset_, text_block->half_height_ - v->y_);
            width_ = std::max<wxCoord>(width_, v->x_ + text_block->rect_width_ / 2);
            height_ = std::max<wxCoord>(height_, v->y_ + text_block->half_height_);
            ++text_block;
        }
        for (auto& tb : text_blocks_) {
            tb.x_ = tb.vertex_position_->x_ + x_offset_;
            tb.y_ = tb.vertex_position_->y_ + y_offset_;
        }
        width_ += x_offset_;
        height_ += y_offset_;
        sizes_calculated_ = true;
    }
    for (auto e = layout_.edges_cbegin(); e != layout_.edges_cend(); ++e) {
        auto s = e->first; auto t = e->second;
        painter.draw_line(dc, s->x_ + x_offset_, s->y_ + y_offset_, t->x_ + x_offset_, t->y_ + y_offset_);
    }
    for (auto tb = text_blocks_.cbegin(); tb != text_blocks_.cend(); ++tb) {
        auto v = tb->vertex_position_;
        painter.draw_rect(dc, tb->x_, tb->y_, tb->rect_width_, tb->rect_height_);
        painter.draw_text(dc, v->label_, tb->x_ - tb->half_width_, tb->y_ - tb->half_height_);
    }
}


Drawable_graph::Text_block::Text_block(const Graph::Layout::Vertex_position* v)
    :vertex_position_(v)
{}

void Drawable_graph::Text_block::calculate_sizes(wxDC& dc) {
    dc.GetTextExtent(vertex_position_->label_, &half_width_, &half_height_);
    rect_width_ = half_width_ + 8;
    rect_height_ = half_height_;
    half_width_ /= 2;
    half_height_ /= 2;
}

