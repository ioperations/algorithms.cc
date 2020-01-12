#include "fr_layout.h"

#include "canvas_app.h"

// int main() {
//     Graph graph;
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
//     graph.calculate_layout(100, 100);

//     graph.positions().iterate([](auto p) {
//         std::cout << p.label_ << " " << p.x_ << " " << p.y_ << std::endl;
//     });
// }

class Graph_widget : public Canvas_widget {
    private:
        Graph graph_;
    public:
        Graph_widget(wxWindow *parent) :Canvas_widget(parent) {
            auto v1 = graph_.add_vertex("1");
            auto v2 = graph_.add_vertex("2");
            auto v3 = graph_.add_vertex("3");
            auto v4 = graph_.add_vertex("4");
            auto v5 = graph_.add_vertex("5");
            auto v6 = graph_.add_vertex("6");

            graph_.add_edge(v1, v2)
                .add_edge(v2, v3)
                .add_edge(v3, v4)
                .add_edge(v1, v3)
                .add_edge(v2, v4)
                .add_edge(v2, v5)
                .add_edge(v2, v6);
            graph_.calculate_layout(100, 100);
        }
    protected:
        void do_draw(wxDC& dc) override {
            auto draw_r = [this, &dc](const auto& v) {
                wxCoord text_w;
                wxCoord text_h;
                dc.GetTextExtent(v.label_, &text_w, &text_h);
                draw_rect(dc, v.x_, v.y_, text_w + 8, text_h);
                draw_text(dc, v.label_, v.x_ - text_w / 2, v.y_ - text_h / 2);
            };
            graph_.positions().iterate_edges([this, &dc, draw_r](const auto& edge) {
                draw_line(dc, edge.first.x_, edge.first.y_, edge.second.x_, edge.second.y_);
                draw_r(edge.first);
                draw_r(edge.second);
            });
        }
};

class Graph_application : public Application {
    wxWindow* create_canvas_widget(wxWindow* parent) override {
        return new Graph_widget(parent);
    }
};
wxIMPLEMENT_APP(Graph_application);

