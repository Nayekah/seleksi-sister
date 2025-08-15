#ifndef MANDELBROT_GUI_H
#define MANDELBROT_GUI_H

#include "mandelbrot.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <array>

enum class ViewMode {
    INPUT_PANEL,
    RESULTS_VIEW
};

enum class RenderMethod {
    CUDA = 0
};

struct RenderResult {
    std::vector<Color> image_data;
    double render_time;
    std::string method_name;
    bool completed;
    
    RenderResult() : render_time(0.0), completed(false) {}
};

class MandelbrotGUI {
private:
    sf::RenderWindow window;
    sf::Font font;
    
    ViewMode current_view;
    int current_result_page;
    
    // Input panel elements
    sf::RectangleShape input_panel;
    sf::Text title_text;
    sf::Text width_label, height_label, iterations_label;
    sf::Text width_input, height_input, iterations_input;
    sf::RectangleShape width_box, height_box, iterations_box;
    sf::RectangleShape render_button;
    sf::Text render_text;
    
    // Results view elements
    sf::Texture result_texture;
    sf::Sprite result_sprite;
    
    // Mini Julia preview
    sf::Texture mini_julia_texture;
    sf::Sprite mini_julia_sprite;
    sf::RectangleShape mini_julia_frame;
    sf::Text mini_julia_label;
    sf::Text mini_julia_coords;
    std::vector<Color> mini_julia_data;
    bool mini_julia_enabled;
    sf::Vector2i last_mouse_pos;
    std::complex<double> hover_julia_constant;
    
    // Minimap (top-right corner Julia preview)
    sf::RectangleShape minimap_frame;
    sf::RectangleShape minimap_bg;
    sf::Text minimap_label;
    sf::Text minimap_coords;
    sf::Texture minimap_texture;
    sf::Sprite minimap_sprite;
    std::vector<Color> minimap_data;
    
    // Julia set rendering
    bool julia_rendered;
    sf::Vector2i julia_mouse_pos;
    std::vector<Color> julia_image_data;
    
    // Rendering
    MandelbrotGenerator* generator;
    RenderResult cuda_result;
    bool rendering_in_progress;
    
    // Status
    sf::Text status_text;
    
    // Input handling
    std::string input_strings[3];
    int active_input;
    
    // Fractal parameters
    int render_width, render_height, max_iterations;
    double x_min, x_max, y_min, y_max;
    double original_x_min, original_x_max, original_y_min, original_y_max;
    
    // Zoom and pan
    bool is_dragging;
    bool is_selecting_zoom;
    sf::Vector2i drag_start;
    sf::Vector2i current_mouse_pos;
    sf::RectangleShape selection_rect;
    sf::View image_view;
    bool is_zoom_changed;
    float zoom_factor;
    
    // Julia constant (at end to match constructor order)
    std::complex<double> julia_constant;
    
    bool cuda_available;
    bool font_loaded;
    
    // Constants
    static const int MAX_RESOLUTION = 2000;
    static const int MAX_ITERATIONS = 10000;
    static const int DEFAULT_WIDTH = 800;
    static const int DEFAULT_HEIGHT = 600;
    static const int DEFAULT_ITERATIONS = 500;
    static const int INPUT_WINDOW_WIDTH = 800;
    static const int INPUT_WINDOW_HEIGHT = 800;
    
public:
    MandelbrotGUI();
    ~MandelbrotGUI();
    
    bool initialize();
    void run();
    
private:
    // Main flow
    void handle_events();
    void update();
    void render();
    
    // Input panel methods
    void render_input_panel();
    void handle_input_panel_events(const sf::Event& event);
    void validate_and_start_render();
    
    // Results view methods
    void setup_results_view();
    void render_results_view();
    void handle_results_view_events(const sf::Event& event);
    
    // Minimap methods (top-right Julia preview)
    void setup_minimap();
    void render_minimap();
    void update_julia_preview(sf::Vector2i mouse_pos);
    void generate_mini_julia_preview(std::complex<double> c);
    
    // Pop-up dialogs
    void show_save_popup();
    void show_julia_popup();
    void save_current_view();
    
    // Benchmark methods
    void print_benchmark_header();
    void render_all_methods();
    
    // Coordinate mapping
    bool is_mouse_over_image(sf::Vector2i mouse_pos);
    std::complex<double> screen_to_complex(sf::Vector2i screen_pos);
    
    // Rendering methods
    void start_rendering();
    void finish_rendering();
    void real_time_rerender();
    
    // Image handling
    void convert_to_texture(const std::vector<Color>& image_data, sf::Texture& texture);
    
    // Save functionality
    void create_output_directory();
    
    // Zoom and pan functionality
    void handle_zoom_pan_events(const sf::Event& event);
    void handle_mouse_wheel(float delta);
    void perform_zoom(float factor);
    void zoom_to_selection();
    void zoom_at_point(sf::Vector2i point, float factor);
    void apply_pan(sf::Vector2f delta);
    void reset_view();
    void update_fractal_bounds();
    void constrain_view_to_bounds();
    
    // Julia set functionality
    void update_julia_constant_from_mouse(sf::Vector2i mouse_pos);
    void generate_julia_set();
    
    // UI helpers  
    bool is_point_in_rect(int x, int y, const sf::RectangleShape& rect);
    
    // Font initialization
    bool initialize_font();
    void setup_all_text_elements();
    void setup_shapes_and_layout();
    
    // Utility methods
    bool check_cuda_availability();
    bool is_valid_input();
    std::string get_timestamp();
};

#endif // MANDELBROT_GUI_H