#pragma once

#include "canvas_app.h"

class Vertical_drawable_block : public Drawable {
    private:
        std::vector<Drawable*> drawables_;
    public:
        Vertical_drawable_block() = default;
        Vertical_drawable_block(const std::initializer_list<Drawable*>& l)
            :drawables_(l)
        {}
        ~Vertical_drawable_block() {
            for (auto d : drawables_)
                delete d;
        }
        void add(Drawable* drawable) {
            drawables_.push_back(drawable);
        }
        void draw(wxDC& dc, Painter& painter) override {
            for (auto d : drawables_) {
                d->draw(dc, painter);
                painter.shift_y_offset(d->height());
            }
        }
};

class Drawable_text : public Drawable {
    private:
        wxString text_;
        bool sizes_calculated_;
    public:
        Drawable_text(const std::string& text) :text_(text), sizes_calculated_(false) {}
        void draw(wxDC& dc, Painter& painter) override {
            if (!sizes_calculated_) {
                dc.GetTextExtent(text_, &width_, &height_);
                sizes_calculated_ = true;
            }
            painter.draw_text(dc, text_, 0, 0);
        }
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
            Text_block(const Graph::Layout::Vertex_position* v)
                :vertex_position_(v)
            {}
            Text_block() = default;
            void calculate_sizes(wxDC& dc) {
                dc.GetTextExtent(vertex_position_->label_, &half_width_, &half_height_);
                rect_width_ = half_width_ + 8;
                rect_height_ = half_height_;
                half_width_ /= 2;
                half_height_ /= 2;
            }
        };
        using Layout = Graph::Layout::Layout;
        Layout layout_;
        std::vector<Text_block> text_blocks_;
        bool sizes_calculated_;
        wxCoord x_offset_;
        wxCoord y_offset_;
    public:
        Drawable_graph(Layout&& layout) 
            :layout_(std::move(layout)), 
            text_blocks_(layout_.vertices_count()),
            sizes_calculated_(false), x_offset_(0), y_offset_(0) {}
        void draw(wxDC& dc, Painter& painter) override {
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
};


