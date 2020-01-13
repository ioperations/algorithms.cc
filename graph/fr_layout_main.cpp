#include "fr_layout.h"

#include "graph.h"
#include "adjacency_matrix.h"
#include "canvas_app.h"

// #include <forward_list>
// #include <iostream>

// int main() {
//     Graph::Layout::Calculator graph;
//     auto v1 = graph.add_vertex("1");
//     auto v2 = graph.add_vertex("2");
//     auto v3 = graph.add_vertex("3");
//     auto v4 = graph.add_vertex("4");
//     auto v5 = graph.add_vertex("5");

//     graph.add_edge(v1, v2)
//         .add_edge(v2, v3)
//         .add_edge(v3, v4)
//         .add_edge(v1, v3)
//         .add_edge(v2, v4)
//         .add_edge(v2, v5);
//     auto layout = graph.calculate_layout_2(100, 100);

//     for (auto e = layout.cbegin(); e != layout.cend(); ++e)
//         std::cout << e->first->label_ << " " << e->second->label_ << std::endl;
// }

class Drawable {
    protected:
        wxCoord height_;
        wxCoord width_;
    public:
        virtual void draw(wxDC& dc, Painter& painter) = 0;
        virtual ~Drawable() {}
};

class Drawable_text : public Drawable {
    void draw(wxDC& dc, Painter& painter) override {
        painter.draw_text(dc, "test", 0, 0);
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
            void calculate_sizes(wxDC& dc) {
                dc.GetTextExtent(vertex_position_->label_, &half_width_, &half_height_);
                rect_width_ = half_width_ + 8;
                rect_height_ = half_height_;
                half_width_ /= 2;
                half_height_ /= 2;
            }
            Text_block() = default;
        };
        using Layout = Graph::Layout::Layout;
        Layout layout_;
        std::vector<Text_block> text_blocks_;
        bool text_extents_calculated_;
        wxCoord x_offset_;
        wxCoord y_offset_;
    public:
        Drawable_graph(Layout&& layout) 
            :layout_(std::move(layout)), 
            text_blocks_(layout_.vertices_count()),
            text_extents_calculated_(false) 
        {}
        void draw(wxDC& dc, Painter& painter) override {
            if (!text_extents_calculated_) {
                auto text_blocks_it = text_blocks_.begin();
                for (auto v = layout_.vertices_cbegin(); v != layout_.vertices_cend(); ++v) {
                    *text_blocks_it = Text_block(&*v);
                    text_blocks_it->calculate_sizes(dc);
                    x_offset_ = std::max<wxCoord>(x_offset_, text_blocks_it->rect_width_ / 2 - v->x_);
                    y_offset_ = std::max<wxCoord>(y_offset_, text_blocks_it->half_height_ - v->y_);
                    ++text_blocks_it;
                }
                for (auto& tb : text_blocks_) {
                    tb.x_ = tb.vertex_position_->x_ + x_offset_;
                    tb.y_ = tb.vertex_position_->y_ + y_offset_;
                }
                text_extents_calculated_ = true;
            }
            for (auto e = layout_.edges_cbegin(); e != layout_.edges_cend(); ++e) {
                auto s = e->first; auto t = e->second;
                painter.draw_line(dc, s->x_ + x_offset_, s->y_ + y_offset_, t->x_ + x_offset_, t->y_ + y_offset_);
            }
            for (auto tb = text_blocks_.cbegin(); tb != text_blocks_.cend(); ++tb) {
                auto v = tb->vertex_position_;
                painter.draw_rect(dc, v->x_, tb->y_, tb->rect_width_, tb->rect_height_);
                painter.draw_text(dc, v->label_, tb->x_ - tb->half_width_, tb->y_ - tb->half_height_);
            }
        }
};

class Graph_widget : public Canvas_widget {
    private:
        std::vector<std::unique_ptr<Drawable>> drawables_;
    public:
        Graph_widget(wxWindow *parent) :Canvas_widget(parent) {

            Graph::Adjacency_matrix<int> graph;
            Graph::Constructor constructor(graph);
            constructor
                .add_edge(0, 1)
                .add_edge(0, 2)
                .add_edge(0, 5)
                .add_edge(0, 6)
                .add_edge(1, 2)
                .add_edge(2, 3)
                .add_edge(2, 4)
                .add_edge(3, 4)
                .add_edge(4, 5)
                .add_edge(4, 6);

            Graph::Layout::Calculator calculator;
            std::map<size_t, Graph::Layout::Calculator::vertex_descriptor> map;
            Graph::dfs(graph,
                       [&calculator, &map](const auto& v) {
                           map[v.index()] = calculator.add_vertex(std::to_string(v.value()));
                       },
                       [&calculator, &map](const auto& v, const auto& w) {
                           if (v.index() < w.index())
                               calculator.add_edge(map.find(v.index())->second, map.find(w.index())->second);
                       });

            auto layout = calculator.calculate_layout_2(100, 100);

            drawables_.push_back(std::make_unique<Drawable_text>());
            drawables_.push_back(std::make_unique<Drawable_graph>(std::move(layout)));
        }
    protected:
        void do_draw(wxDC& dc) override {
                // painter_.draw_rect(dc, 0, 0, 100, 100);
            dc.DrawRectangle(10, 10, 100, 100);
            for (auto& drawable : drawables_)
                drawable->draw(dc, painter_);
        }
};

class Graph_application : public Application {
    wxWindow* create_canvas_widget(wxWindow* parent) override {
        return new Graph_widget(parent);
    }
};
wxIMPLEMENT_APP(Graph_application);

