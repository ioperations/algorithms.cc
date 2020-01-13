#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

class Painter {
    private:
        wxCoord x_offset_ = 10;
        wxCoord y_offset_ = 10;
    public:
        wxCoord x(double x) {
            return x + x_offset_;
        }
        wxCoord y(double y) {
            return y + y_offset_;
        }
        void draw_rect(wxDC& dc, wxCoord x, wxCoord y, wxCoord w, wxCoord h) {
            dc.DrawRectangle(this->x(x) - w / 2, this->y(y) - h / 2, w, h);
        }
        void draw_text(wxDC& dc, const wxString& label, wxCoord x, wxCoord y) {
            dc.DrawText(label, this->x(x), this->y(y));
        }
        void draw_line(wxDC& dc, wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2) {
            dc.DrawLine(x(x1), y(y1), x(x2), y(y2));
        }
        void shift_x_offset(wxCoord shift) { x_offset_ += shift; }
        void shift_y_offset(wxCoord shift) { y_offset_ += shift; }
};

class Canvas_widget : public wxWindow {
    protected:
        virtual void do_draw(wxDC& dc) = 0;
    public:
        Canvas_widget(wxWindow *parent, const wxPoint &pos=wxDefaultPosition,
                     const wxSize &size=wxDefaultSize, long style=0, const wxString &name=wxPanelNameStr);
};

class Application : public wxApp {
    private:
        bool OnInit() override;
        struct Properties {
            int x_;
            int y_;
            int width_;
            int height_;
            Properties(int x, int y, int width, int height)
                :x_(x), y_(y), width_(width), height_(height)
            {}
            Properties() :Properties(0, 0, 600, 800) {}
        };
    public:
        void write_properties(const Properties& properties);
        virtual wxWindow* create_canvas_widget(wxWindow* parent) = 0;
};

