#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#define IMPLEMENT_CANVAS_APP(f) \
    class Canvas_application : public Canvas_application_base { \
        public: \
                Canvas_application() :Canvas_application_base(f()) {} \
    }; \
    wxIMPLEMENT_APP(Canvas_application); \

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

class Drawable {
    protected:
        wxCoord height_;
        wxCoord width_;
    public:
        Drawable() :height_(0), width_(0) {}
        virtual void calculate_size(wxDC& dc) = 0;
        virtual void draw(wxDC& dc, Painter& painter) = 0;
        virtual ~Drawable() {}
        wxCoord height() { return height_; }
        wxCoord width() { return width_; }
};

class Canvas_application_base : public wxApp {
    private:
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
        Drawable* const drawable_;
        bool OnInit() override;
    public:
        Canvas_application_base(Drawable* const drawable) :drawable_(drawable) {}
        ~Canvas_application_base() { delete drawable_; }
        void write_properties(const Properties& properties);
};

