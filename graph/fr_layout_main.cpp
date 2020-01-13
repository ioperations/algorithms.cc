#include "fr_layout.h"

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

class Drawable_graph : public Drawable {
    private:
        using Layout = Graph::Layout::Layout;
        Layout layout_;
    public:
        Drawable_graph(Layout&& layout) : layout_(std::move(layout)) {
            // for (auto v = layout_.vertices_cbegin(); v != layout_.vertices_cend(); ++v) {
            //     height_ = std::max(height_, v->y_);
            //     width_ = std::max(width_, v->x_);
            // }
            // height_ += 
        }
        void draw(wxDC& dc, Painter& painter) override {
            for (auto e = layout_.edges_cbegin(); e != layout_.edges_cend(); ++e) {
                auto s = e->first; auto t = e->second;
                painter.draw_line(dc, s->x_, s->y_, t->x_, t->y_);
            }
            for (auto v = layout_.vertices_cbegin(); v != layout_.vertices_cend(); ++v) {
                wxCoord text_w; wxCoord text_h;
                dc.GetTextExtent(v->label_, &text_w, &text_h);
                painter.draw_rect(dc, v->x_, v->y_, text_w + 8, text_h);
                painter.draw_text(dc, v->label_, v->x_ - text_w / 2, v->y_ - text_h / 2);
            }
        }
};

class Graph_widget : public Canvas_widget {
    private:
        std::vector<std::unique_ptr<Drawable>> drawables_;
    public:
        Graph_widget(wxWindow *parent) :Canvas_widget(parent) {
            Graph::Layout::Calculator graph;
            auto v1 = graph.add_vertex("1");
            auto v2 = graph.add_vertex("2");
            auto v3 = graph.add_vertex("3");
            auto v4 = graph.add_vertex("4");
            auto v5 = graph.add_vertex("5");

            graph.add_edge(v1, v2)
                .add_edge(v2, v3)
                .add_edge(v3, v4)
                .add_edge(v1, v3)
                .add_edge(v2, v4)
                .add_edge(v2, v5);
            auto layout = graph.calculate_layout_2(100, 100);
            drawables_.push_back(std::make_unique<Drawable_graph>(std::move(layout)));
        }
    protected:
        void do_draw(wxDC& dc) override {
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

