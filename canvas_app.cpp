#include "canvas_app.h"

#include <wx/dcbuffer.h>

#include <iostream>

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

    class Canvas_panel : public wxPanel {
        public:
            Canvas_panel(wxWindow* parent, Application* const application) :wxPanel(parent) {
                auto w = application->create_canvas_widget(this);
                wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
                sizer->Add(w, 1, wxEXPAND);
                SetSizer(sizer);
            }
    };
    class Main_frame : public wxFrame {
        public:
            Main_frame(Application* const application, const wxString title)
                :wxFrame(NULL, wxID_ANY, title) {
                    SetSize(800, 600);
                    new Canvas_panel(this, application);
                }
    };
    (new Main_frame(this, "Canvas"))->Show();

    return true;
}

