#include "canvas_app.h"

#include <wx/dcbuffer.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

#include <iostream>
#include <fstream>

Canvas_widget::Canvas_widget(wxWindow *parent, const wxPoint& pos,
                             const wxSize& size, long style, const wxString& name)
    :wxWindow(parent, wxID_ANY, pos, size, style, name)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    SetBackgroundColour(wxColour("white"));
    Bind(wxEVT_PAINT, [this](wxPaintEvent&) {
        wxAutoBufferedPaintDC dc(this);
        PrepareDC(dc);
        dc.Clear();
        do_draw(dc);
    });
    Bind(wxEVT_ERASE_BACKGROUND, [](wxEraseEvent&) {
        // event is intercepted to prevent background from erasing
    });
    Bind(wxEVT_SIZE, [this](wxSizeEvent&) { Refresh(); });
}


bool Application::OnInit() {
    std::setlocale(LC_ALL, "en_US.UTF-8");

    Properties properties;
    std::ifstream ifs("canvas_app.ini");
    if (ifs.is_open()) {
        namespace pt = boost::property_tree;
        pt::ptree ptree;
        pt::read_ini(ifs, ptree);
        properties.x_ = ptree.get<int>("window.x");
        properties.y_ = ptree.get<int>("window.y");
        properties.width_ = ptree.get<int>("window.width");
        properties.height_ = ptree.get<int>("window.height");
    } else {
        write_properties(properties);
    }

    class Canvas_panel : public wxPanel {
        public:
            Canvas_panel(wxWindow* parent, Application* const application) :wxPanel(parent) {
                wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
                sizer->Add(application->create_canvas_widget(this), 1, wxEXPAND);
                SetSizer(sizer);
            }
    };
    class Main_frame : public wxFrame {
        public:
            Main_frame(Application* const application, const wxString& title, const Properties& properties)
                :wxFrame(NULL, wxID_ANY, title) {
                    SetPosition({properties.x_, properties.y_});
                    SetSize(properties.width_, properties.height_);
                    new Canvas_panel(this, application);

                    Bind(wxEVT_CLOSE_WINDOW, [this, application](wxCloseEvent& e) {
                        Properties properties;
                        GetPosition(&properties.x_, &properties.y_);
                        GetSize(&properties.width_, &properties.height_);
                        application->write_properties(properties);
                        e.Skip();
                    });
                }
    };
    (new Main_frame(this, "Canvas", properties))->Show();

    return true;
}

void Application::write_properties(const Properties& properties) {
    namespace pt = boost::property_tree;
    pt::ptree window_ptree;
    window_ptree.put("x", properties.x_);
    window_ptree.put("y", properties.y_);
    window_ptree.put("width", properties.width_);
    window_ptree.put("height", properties.height_);

    pt::ptree ptree;
    ptree.push_back(pt::ptree::value_type("window", window_ptree));
    std::ofstream osf("canvas_app.ini");
    pt::write_ini(osf, ptree);
}
