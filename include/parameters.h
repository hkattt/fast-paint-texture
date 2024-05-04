#pragma once 

namespace ProgramParameters {
        const int min_brush_size = 2;     // Smallest brush radius
        
        const int min_stroke_length = 4;  // Minimum stroke length (number of control points)
        const int max_stroke_length = 16; // Maximum stroke length 

        const float blur_factor = 2.0f;      // Radius of blurring in reference image to brush radius

        const float filter_fac = 1.0f;    // IIR filter factor for stroke control points
}