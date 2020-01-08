#include "fr_layout.h"

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/dcbuffer.h>

#include <iostream>

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

class Graph_widget : public wxWindow {
    public:
        Graph_widget(wxWindow *parent, const wxPoint &pos=wxDefaultPosition,
                     const wxSize &size=wxDefaultSize, long style=0, const wxString &name=wxPanelNameStr)
            :wxWindow(parent, wxID_ANY, pos, size, style, name)
        {
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

            SetBackgroundStyle(wxBG_STYLE_PAINT);
            SetBackgroundColour(wxColour("white"));
            Bind(wxEVT_PAINT, [this](wxPaintEvent&) {
                wxAutoBufferedPaintDC dc(this);
                PrepareDC(dc);
                dc.Clear();

                auto draw_r = [this, &dc](const auto& v) {
                    wxCoord text_w;
                    wxCoord text_h;
                    dc.GetTextExtent(v.label_, &text_w, &text_h);
                    draw_rect(dc, v.x_, v.y_, text_w + 7, text_h);
                    draw_text(dc, v.label_, v.x_ - text_w / 2, v.y_ - text_h / 2);
                };
                graph_.positions().iterate_edges([this, &dc, draw_r](const auto& edge) {
                    draw_line(dc, edge.first.x_, edge.first.y_, edge.second.x_, edge.second.y_);
                    draw_r(edge.first);
                    draw_r(edge.second);
                });
            });
            Bind(wxEVT_ERASE_BACKGROUND, [](wxEraseEvent&) {
                // event is intercepted to prevent background from erasing
            });
            Bind(wxEVT_SIZE, [this](wxSizeEvent&) { Refresh(); });
        }
    private:
        Graph graph_;

        wxCoord x(double x) {
            return x + 10;
        }
        wxCoord y(double y) {
            return y + 10;
        }
        void draw_rect(wxAutoBufferedPaintDC& dc, wxCoord x, wxCoord y, wxCoord w, wxCoord h) {
            dc.DrawRectangle(this->x(x) - w / 2, this->y(y) - h / 2, w, h);
        }
        void draw_text(wxAutoBufferedPaintDC& dc, const std::string& label, wxCoord x, wxCoord y) {
            dc.DrawText(label, this->x(x), this->y(y));
        }
        void draw_line(wxAutoBufferedPaintDC& dc, wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2) {
            dc.DrawLine(x(x1), y(y1), x(x2), y(y2));
        }
};

class Graph_panel : public wxPanel {
    public:
        Graph_panel(wxWindow* parent) :wxPanel(parent) {
            auto w = new Graph_widget(this);
            wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
            sizer->Add(w, 1, wxEXPAND);
            SetSizer(sizer);
        }
};

class Main_frame : public wxFrame {
    public:
        Main_frame(const wxString title) :wxFrame(NULL, wxID_ANY, title) {
            SetSize(800, 600);
            new Graph_panel(this);
        }
};

class Application : public wxApp {
    bool OnInit() override {
        std::setlocale(LC_ALL, "en_US.UTF-8");
        (new Main_frame("Graph"))->Show();
        return true;
    }
};

wxIMPLEMENT_APP(Application);

