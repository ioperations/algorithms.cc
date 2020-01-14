#include "canvas_app.h"

#include <wx/dcbuffer.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

#include <iostream>
#include <fstream>

class Canvas_widget : public wxWindow {
    public:
        Canvas_widget(wxWindow* parent, Drawable* const drawable, const wxPoint &pos=wxDefaultPosition,
                      const wxSize &size=wxDefaultSize, long style=0, const wxString &name=wxPanelNameStr)
            :wxWindow(parent, wxID_ANY, pos, size, style, name)
        {
            SetBackgroundStyle(wxBG_STYLE_PAINT);
            SetBackgroundColour(wxColour("white"));
            wxAutoBufferedPaintDC dc(this);
            drawable->calculate_size(dc);
            SetMinSize({drawable->width() + 20, drawable->height() + 20});

            Bind(wxEVT_PAINT, [this, drawable](wxPaintEvent&) {
                wxAutoBufferedPaintDC dc(this);
                PrepareDC(dc);
                dc.Clear();
                Painter painter;
                drawable->draw(dc, painter);
            });
            Bind(wxEVT_ERASE_BACKGROUND, [](wxEraseEvent&) {
                // event is intercepted to prevent background from erasing
            });
            Bind(wxEVT_SIZE, [this](wxSizeEvent&) { Refresh(); });
        }
};

bool Canvas_application_base::OnInit() {
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

    class Canvas_panel : public wxScrolled<wxPanel> {
        public:
            Canvas_panel(wxWindow* parent, Drawable* const drawable) :wxScrolled<wxPanel>(parent) {
                wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
                sizer->Add(new Canvas_widget(this, drawable), 1, wxEXPAND);
                SetSizer(sizer);
                SetScrollRate(5, 5); 
            }
    };

    class Main_frame : public wxFrame {
        public:
            Main_frame(Canvas_application_base* const application, Drawable* const drawable,
                       const wxString& title, const Properties& properties)
                :wxFrame(NULL, wxID_ANY, title) {
                    SetPosition({properties.x_, properties.y_});
                    SetSize(properties.width_, properties.height_);
                    new Canvas_panel(this, drawable);

                    Bind(wxEVT_CLOSE_WINDOW, [this, application](wxCloseEvent& e) {
                        Properties properties;
                        GetPosition(&properties.x_, &properties.y_);
                        GetSize(&properties.width_, &properties.height_);
                        application->write_properties(properties);
                        e.Skip();
                    });
                }
    };
    (new Main_frame(this, drawable_, "Canvas", properties))->Show();

    return true;
}

void Canvas_application_base::write_properties(const Properties& properties) {
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
