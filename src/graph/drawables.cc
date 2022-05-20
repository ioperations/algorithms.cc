#include "drawables.h"

Vertical_drawable_block::Vertical_drawable_block(
    const std::initializer_list<Drawable*>& l)
    : drawables_(l) {}

Vertical_drawable_block::~Vertical_drawable_block() {
    for (auto d : drawables_) delete d;
}

void Vertical_drawable_block::add(Drawable* drawable) {
    drawables_.push_back(drawable);
}

void Vertical_drawable_block::calculate_size(wxDC& dc) {
    for (auto d : drawables_) {
        d->calculate_size(dc);
        width_ = std::max<wxCoord>(width_, d->width());
        height_ += d->height();
    }
}

void Vertical_drawable_block::draw(wxDC& dc, Painter& painter) {
    for (auto d : drawables_) {
        d->draw(dc, painter);
        painter.shift_y_offset(d->height());
    }
}

Drawable_text::Drawable_text(const std::string& text) : text_(text) {}

void Drawable_text::calculate_size(wxDC& dc) {
    dc.GetTextExtent(text_, &width_, &height_);
}

void Drawable_text::draw(wxDC& dc, Painter& painter) {
    painter.draw_text(dc, text_, 0, 0);
}

Drawable_graph::Drawable_graph(Layout&& layout)
    : layout_(std::move(layout)), text_blocks_(layout_.vertices_count()) {}

void Drawable_graph::calculate_size(wxDC& dc) {
    {
        auto text_block = text_blocks_.begin();
        auto v = layout_.vertices_cbegin();
        if (v != layout_.vertices_cend()) {
            {
                *text_block = Text_block(&*v, dc);
                x_offset_ = text_block->rect_width_ / 2 - v->x_;
                y_offset_ = text_block->half_height_ - v->y_;
                width_ = v->x_ + text_block->rect_width_ / 2;
                height_ = v->y_ + text_block->half_height_;
                ++text_block;
            }
            for (++v; v != layout_.vertices_cend(); ++v) {
                *text_block = Text_block(&*v, dc);
                x_offset_ = std::max<wxCoord>(
                    x_offset_, text_block->rect_width_ / 2 - v->x_);
                y_offset_ = std::max<wxCoord>(y_offset_,
                                              text_block->half_height_ - v->y_);
                width_ = std::max<wxCoord>(width_,
                                           v->x_ + text_block->rect_width_ / 2);
                height_ = std::max<wxCoord>(height_,
                                            v->y_ + text_block->half_height_);
                ++text_block;
            }
        }
    }
    for (auto& tb : text_blocks_) {
        tb.x_ = tb.vertex_position_->x_ + x_offset_;
        tb.y_ = tb.vertex_position_->y_ + y_offset_;
    }
    width_ += x_offset_;
    height_ += y_offset_;
}

void Drawable_graph::draw(wxDC& dc, Painter& painter) {
    for (auto e = layout_.edges_cbegin(); e != layout_.edges_cend(); ++e) {
        auto s = e->first;
        auto t = e->second;
        painter.draw_line(dc, s->x_ + x_offset_, s->y_ + y_offset_,
                          t->x_ + x_offset_, t->y_ + y_offset_);
    }
    for (auto tb = text_blocks_.cbegin(); tb != text_blocks_.cend(); ++tb) {
        auto v = tb->vertex_position_;
        painter.draw_rect(dc, tb->x_, tb->y_, tb->rect_width_,
                          tb->rect_height_);
        painter.draw_text(dc, v->label_, tb->x_ - tb->half_width_,
                          tb->y_ - tb->half_height_);
    }
}

Drawable_graph::Text_block::Text_block(const Graph::Layout::Vertex_position* v,
                                       wxDC& dc)
    : vertex_position_(v) {
    dc.GetTextExtent(vertex_position_->label_, &half_width_, &half_height_);
    rect_width_ = half_width_ + 8;
    rect_height_ = half_height_;
    half_width_ /= 2;
    half_height_ /= 2;
}
