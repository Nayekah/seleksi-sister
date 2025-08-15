#include "mandelbrot_gui.h"
#include <iostream>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <cmath>

#ifdef USE_CUDA
#include <cuda_runtime.h>
#endif

MandelbrotGUI::MandelbrotGUI() 
    : current_view(ViewMode::INPUT_PANEL), current_result_page(0),
      active_input(-1), render_width(DEFAULT_WIDTH), render_height(DEFAULT_HEIGHT), max_iterations(DEFAULT_ITERATIONS),
      x_min(-2.5), x_max(1.5), y_min(-2.0), y_max(2.0),
      original_x_min(-2.5), original_x_max(1.5), original_y_min(-2.0), original_y_max(2.0),
      mini_julia_enabled(true), last_mouse_pos(-1, -1), hover_julia_constant(0, 0),
      julia_rendered(false), julia_mouse_pos(400, 300),
      generator(nullptr), cuda_result(), rendering_in_progress(false),
      is_dragging(false), is_selecting_zoom(false), drag_start(0, 0), current_mouse_pos(0, 0), is_zoom_changed(false), zoom_factor(1.0f),
      julia_constant(-0.7, 0.27015),
      cuda_available(false), font_loaded(false) {
    
    input_strings[0] = std::to_string(DEFAULT_WIDTH);
    input_strings[1] = std::to_string(DEFAULT_HEIGHT);
    input_strings[2] = std::to_string(DEFAULT_ITERATIONS);
    
    selection_rect.setFillColor(sf::Color::Transparent);
    selection_rect.setOutlineColor(sf::Color::White);
    selection_rect.setOutlineThickness(2);
    
    cuda_available = check_cuda_availability();
    
    std::cout << "=== Mandelbrot CUDA Generator ===" << std::endl;
    std::cout << "CUDA Support: " << (cuda_available ? "YES" : "NO") << std::endl;
}

MandelbrotGUI::~MandelbrotGUI() {
    if (generator) {
        #ifdef USE_CUDA
        if (cuda_available) {
            generator->cleanup_cuda();
        }
        #endif
        delete generator;
    }
}

bool MandelbrotGUI::initialize() {
    window.create(sf::VideoMode(INPUT_WINDOW_WIDTH, INPUT_WINDOW_HEIGHT), "Mandelbrot CUDA Generator - Input");
    window.setFramerateLimit(60);
    
    font_loaded = initialize_font();
    setup_all_text_elements();
    setup_shapes_and_layout();
    create_output_directory();
    
    std::cout << "GUI initialized. Ready for input." << std::endl;
    return true;
}

bool MandelbrotGUI::initialize_font() {
    std::vector<std::string> font_paths = {
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/TTF/DejaVuSans.ttf", 
        "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf"
    };
    
    for (const auto& path : font_paths) {
        if (font.loadFromFile(path)) {
            return true;
        }
    }
    return false;
}

void MandelbrotGUI::setup_all_text_elements() {
    title_text.setString("Mandelbrot CUDA Generator");
    title_text.setCharacterSize(24);
    title_text.setFillColor(sf::Color::White);
    title_text.setPosition(250, 120);
    if (font_loaded) title_text.setFont(font);
    
    width_label.setString("Width:");
    width_label.setCharacterSize(20);
    width_label.setFillColor(sf::Color::White);
    width_label.setPosition(200, 200);
    if (font_loaded) width_label.setFont(font);
    
    height_label.setString("Height:");
    height_label.setCharacterSize(20);
    height_label.setFillColor(sf::Color::White);
    height_label.setPosition(200, 260);
    if (font_loaded) height_label.setFont(font);
    
    iterations_label.setString("Iterations:");
    iterations_label.setCharacterSize(20);
    iterations_label.setFillColor(sf::Color::White);
    iterations_label.setPosition(200, 320);
    if (font_loaded) iterations_label.setFont(font);
    
    width_input.setString(input_strings[0]);
    width_input.setCharacterSize(18);
    width_input.setFillColor(sf::Color::White);
    width_input.setPosition(348, 206);
    if (font_loaded) width_input.setFont(font);
    
    height_input.setString(input_strings[1]);
    height_input.setCharacterSize(18);
    height_input.setFillColor(sf::Color::White);
    height_input.setPosition(348, 266);
    if (font_loaded) height_input.setFont(font);
    
    iterations_input.setString(input_strings[2]);
    iterations_input.setCharacterSize(18);
    iterations_input.setFillColor(sf::Color::White);
    iterations_input.setPosition(348, 326);
    if (font_loaded) iterations_input.setFont(font);
    
    render_text.setString("RENDER");
    render_text.setCharacterSize(20);
    render_text.setFillColor(sf::Color::White);
    render_text.setPosition(350, 415);
    if (font_loaded) render_text.setFont(font);
    
    status_text.setString("Ready to render");
    status_text.setCharacterSize(16);
    status_text.setFillColor(sf::Color::White);
    status_text.setPosition(80, 720);
    if (font_loaded) status_text.setFont(font);
}

void MandelbrotGUI::setup_shapes_and_layout() {
    input_panel.setSize(sf::Vector2f(500, 400));
    input_panel.setPosition(150, 100);
    input_panel.setFillColor(sf::Color(40, 45, 55));
    input_panel.setOutlineColor(sf::Color(80, 90, 100));
    input_panel.setOutlineThickness(2);
    
    width_box.setSize(sf::Vector2f(180, 35));
    width_box.setPosition(340, 195);
    width_box.setFillColor(sf::Color(60, 65, 75));
    width_box.setOutlineColor(sf::Color(100, 110, 120));
    width_box.setOutlineThickness(1);
    
    height_box.setSize(sf::Vector2f(180, 35));
    height_box.setPosition(340, 255);
    height_box.setFillColor(sf::Color(60, 65, 75));
    height_box.setOutlineColor(sf::Color(100, 110, 120));
    height_box.setOutlineThickness(1);
    
    iterations_box.setSize(sf::Vector2f(180, 35));
    iterations_box.setPosition(340, 315);
    iterations_box.setFillColor(sf::Color(60, 65, 75));
    iterations_box.setOutlineColor(sf::Color(100, 110, 120));
    iterations_box.setOutlineThickness(1);
    
    render_button.setSize(sf::Vector2f(160, 60));
    render_button.setPosition(320, 390);
    render_button.setFillColor(sf::Color(60, 120, 60));
    render_button.setOutlineColor(sf::Color(80, 150, 80));
    render_button.setOutlineThickness(2);
}

void MandelbrotGUI::run() {
    std::cout << "Starting GUI main loop..." << std::endl;
    
    while (window.isOpen()) {
        handle_events();
        update();
        render();
    }
}

void MandelbrotGUI::handle_events() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
        
        if (current_view == ViewMode::INPUT_PANEL) {
            handle_input_panel_events(event);
        } else if (current_view == ViewMode::RESULTS_VIEW) {
            handle_results_view_events(event);
        }
    }
}

void MandelbrotGUI::update() {
    if (rendering_in_progress) {
        print_benchmark_header();
        render_all_methods();
        rendering_in_progress = false;
        finish_rendering();
    }
    
    if (current_view == ViewMode::RESULTS_VIEW && is_zoom_changed) {
        real_time_rerender();
    }
}

void MandelbrotGUI::render() {
    window.clear(sf::Color(20, 25, 35));
    
    if (current_view == ViewMode::INPUT_PANEL) {
        render_input_panel();
    } else if (current_view == ViewMode::RESULTS_VIEW) {
        render_results_view();
    }
    
    window.display();
}

void MandelbrotGUI::render_input_panel() {
    window.draw(input_panel);
    window.draw(title_text);
    window.draw(width_label);
    window.draw(height_label);
    window.draw(iterations_label);
    
    if (active_input == 0) {
        width_box.setOutlineColor(sf::Color(150, 180, 220));
        width_box.setOutlineThickness(2);
    } else {
        width_box.setOutlineColor(sf::Color(100, 110, 120));
        width_box.setOutlineThickness(1);
    }
    window.draw(width_box);
    window.draw(width_input);
    
    if (active_input == 1) {
        height_box.setOutlineColor(sf::Color(150, 180, 220));
        height_box.setOutlineThickness(2);
    } else {
        height_box.setOutlineColor(sf::Color(100, 110, 120));
        height_box.setOutlineThickness(1);
    }
    window.draw(height_box);
    window.draw(height_input);
    
    if (active_input == 2) {
        iterations_box.setOutlineColor(sf::Color(150, 180, 220));
        iterations_box.setOutlineThickness(2);
    } else {
        iterations_box.setOutlineColor(sf::Color(100, 110, 120));
        iterations_box.setOutlineThickness(1);
    }
    window.draw(iterations_box);
    window.draw(iterations_input);
    
    window.draw(render_button);
    window.draw(render_text);
    window.draw(status_text);
    
    sf::RectangleShape controls_box;
    controls_box.setSize(sf::Vector2f(640, 160));
    controls_box.setPosition(80, 520);
    controls_box.setFillColor(sf::Color(30, 35, 45));
    controls_box.setOutlineColor(sf::Color(70, 80, 90));
    controls_box.setOutlineThickness(1);
    window.draw(controls_box);
    
    sf::Text controls_title;
    if (font_loaded) controls_title.setFont(font);
    controls_title.setCharacterSize(18);
    controls_title.setFillColor(sf::Color(150, 180, 220));
    controls_title.setPosition(90, 530);
    controls_title.setString("Controls (after rendering):");
    window.draw(controls_title);
    
    sf::Text controls_info;
    if (font_loaded) controls_info.setFont(font);
    controls_info.setCharacterSize(14);
    controls_info.setFillColor(sf::Color(180, 200, 220));
    controls_info.setPosition(90, 560);
    controls_info.setString(
        "Mouse Wheel / +/- Keys: Zoom in/out (1x - 50x)\n"
        "Left Drag: Select zoom area\n"
        "Right Drag: Pan view (when zoomed in)\n"
        "Mouse Hover: Real-time Julia set preview\n"
        "\n"
        "S Key: Save current view     J Key: Generate Julia set\n"
        "R Key: Reset to original     ESC: Back to input panel"
    );
    window.draw(controls_info);
    
    status_text.setPosition(80, 700);
}

void MandelbrotGUI::handle_input_panel_events(const sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed) {
        int x = event.mouseButton.x;
        int y = event.mouseButton.y;
        
        active_input = -1;
        if (is_point_in_rect(x, y, width_box)) {
            active_input = 0;
        } else if (is_point_in_rect(x, y, height_box)) {
            active_input = 1;
        } else if (is_point_in_rect(x, y, iterations_box)) {
            active_input = 2;
        } else if (is_point_in_rect(x, y, render_button) && !rendering_in_progress) {
            validate_and_start_render();
        }
    }
    
    if (event.type == sf::Event::TextEntered && active_input != -1) {
        if (event.text.unicode == 8) {
            if (!input_strings[active_input].empty()) {
                input_strings[active_input].pop_back();
            }
        } else if (event.text.unicode >= 48 && event.text.unicode <= 57) {
            if (input_strings[active_input].length() < 8) {
                input_strings[active_input] += static_cast<char>(event.text.unicode);
            }
        }
        
        if (active_input == 0) {
            width_input.setString(input_strings[0]);
        } else if (active_input == 1) {
            height_input.setString(input_strings[1]);
        } else if (active_input == 2) {
            iterations_input.setString(input_strings[2]);
        }
    }
    
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Tab) {
            active_input = (active_input + 1) % 3;
        } else if (event.key.code == sf::Keyboard::Enter) {
            validate_and_start_render();
        }
    }
}

void MandelbrotGUI::validate_and_start_render() {
    if (!cuda_available) {
        status_text.setString("CUDA not available!");
        return;
    }
    
    if (!is_valid_input()) {
        status_text.setString("Invalid input! Max Resolution: " + std::to_string(MAX_RESOLUTION) + "x" + std::to_string(MAX_RESOLUTION) +
                              " Max Iterations: " + std::to_string(MAX_ITERATIONS));
        return;
    }
    
    render_width = std::stoi(input_strings[0]);
    render_height = std::stoi(input_strings[1]);
    max_iterations = std::stoi(input_strings[2]);
    
    start_rendering();
}

bool MandelbrotGUI::is_valid_input() {
    try {
        int w = std::stoi(input_strings[0]);
        int h = std::stoi(input_strings[1]);
        int iter = std::stoi(input_strings[2]);
        
        return w > 0 && w <= MAX_RESOLUTION && 
               h > 0 && h <= MAX_RESOLUTION && 
               iter > 0 && iter <= MAX_ITERATIONS;
    } catch (...) {
        return false;
    }
}

void MandelbrotGUI::start_rendering() {
    cuda_result.completed = false;
    cuda_result.render_time = 0.0;
    cuda_result.image_data.clear();
    
    if (generator) delete generator;
    generator = new MandelbrotGenerator(render_width, render_height, max_iterations);
    
    #ifdef USE_CUDA
    if (cuda_available) {
        generator->init_cuda();
    }
    #endif
    
    rendering_in_progress = true;
    
    x_min = original_x_min;
    x_max = original_x_max;
    y_min = original_y_min;
    y_max = original_y_max;
    zoom_factor = 1.0f;
    is_zoom_changed = false;
    
    status_text.setString("Rendering...");
    
    std::cout << "\n=== RENDERING STARTED ===" << std::endl;
    std::cout << "Resolution: " << render_width << "x" << render_height << std::endl;
    std::cout << "Iterations: " << max_iterations << std::endl;
    std::cout << "Total pixels: " << (render_width * render_height) << std::endl;
}

void MandelbrotGUI::print_benchmark_header() {
    std::cout << "\n=== PERFORMANCE BENCHMARK ===" << std::endl;
    std::cout << "Testing Serial, Parallel, and CUDA implementations..." << std::endl;
}

void MandelbrotGUI::render_all_methods() {
    // Serial CPU
    std::cout << "\n1. Serial CPU Implementation:" << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    generator->set_bounds(x_min, x_max, y_min, y_max);
    std::vector<Color> serial_image(render_width * render_height);
    generator->generate_serial(serial_image);
    auto end = std::chrono::high_resolution_clock::now();
    double serial_time = std::chrono::duration<double>(end - start).count();
    std::cout << "   Time: " << std::fixed << std::setprecision(3) << serial_time << " seconds" << std::endl;
    
    // Parallel CPU
    std::cout << "\n2. Parallel CPU Implementation:" << std::endl;
    start = std::chrono::high_resolution_clock::now();
    std::vector<Color> parallel_image(render_width * render_height);
    generator->generate_parallel_threads(parallel_image);
    end = std::chrono::high_resolution_clock::now();
    double parallel_time = std::chrono::duration<double>(end - start).count();
    std::cout << "   Time: " << std::fixed << std::setprecision(3) << parallel_time << " seconds" << std::endl;
    std::cout << "   Speedup: " << std::fixed << std::setprecision(2) << serial_time / parallel_time << "x" << std::endl;
    
    // CUDA GPU
    #ifdef USE_CUDA
    if (cuda_available) {
        std::cout << "\n3. CUDA GPU Implementation:" << std::endl;
        start = std::chrono::high_resolution_clock::now();
        cuda_result.image_data.resize(render_width * render_height);
        generator->generate_cuda(cuda_result.image_data);
        end = std::chrono::high_resolution_clock::now();
        cuda_result.render_time = std::chrono::duration<double>(end - start).count();
        cuda_result.completed = true;
        
        std::cout << "   Time: " << std::fixed << std::setprecision(3) << cuda_result.render_time << " seconds" << std::endl;
        std::cout << "   Speedup vs Serial: " << std::fixed << std::setprecision(2) << serial_time / cuda_result.render_time << "x" << std::endl;
        std::cout << "   Speedup vs Parallel: " << std::fixed << std::setprecision(2) << parallel_time / cuda_result.render_time << "x" << std::endl;
        
        // Performance summary
        std::cout << "\n=== PERFORMANCE SUMMARY ===" << std::endl;
        std::cout << "┌─────────────────────┬─────────────┬─────────────┬─────────────────┐" << std::endl;
        std::cout << "│ Method              │ Time (s)    │ Speedup     │ Throughput      │" << std::endl;
        std::cout << "├─────────────────────┼─────────────┼─────────────┼─────────────────┤" << std::endl;
        
        double pixels_per_second_serial = (render_width * render_height) / serial_time;
        std::cout << "│ Serial CPU          │ " << std::setw(11) << std::fixed << std::setprecision(3) << serial_time 
                  << " │ " << std::setw(11) << "1.00x" << " │ " << std::setw(11) << std::scientific << std::setprecision(2) 
                  << pixels_per_second_serial << " px/s │" << std::endl;
        
        double pixels_per_second_parallel = (render_width * render_height) / parallel_time;
        std::cout << "│ Parallel CPU        │ " << std::setw(11) << std::fixed << std::setprecision(3) << parallel_time 
                  << " │ " << std::setw(11) << std::fixed << std::setprecision(2) << serial_time / parallel_time << "x" 
                  << " │ " << std::setw(11) << std::scientific << std::setprecision(2) << pixels_per_second_parallel << " px/s │" << std::endl;
        
        double pixels_per_second_cuda = (render_width * render_height) / cuda_result.render_time;
        std::cout << "│ CUDA GPU            │ " << std::setw(11) << std::fixed << std::setprecision(3) << cuda_result.render_time 
                  << " │ " << std::setw(11) << std::fixed << std::setprecision(2) << serial_time / cuda_result.render_time << "x" 
                  << " │ " << std::setw(11) << std::scientific << std::setprecision(2) << pixels_per_second_cuda << " px/s │" << std::endl;
        
        std::cout << "└─────────────────────┴─────────────┴─────────────┴─────────────────┘" << std::endl;
    }
    #endif
}

void MandelbrotGUI::finish_rendering() {
    setup_results_view();
    current_view = ViewMode::RESULTS_VIEW;
    
    std::cout << "\n=== RENDERING COMPLETE ===" << std::endl;
    std::cout << "Window switched to Mandelbrot view" << std::endl;
    std::cout << "Controls: +/- or Mouse wheel=Zoom (1.0x-1000x), Left-drag=Area zoom, Right-drag=Pan, S=Save, J=Julia, R=Reset, ESC=Back" << std::endl;
}

void MandelbrotGUI::setup_results_view() {
    window.create(sf::VideoMode(render_width, render_height), 
                 "Mandelbrot " + std::to_string(render_width) + "x" + std::to_string(render_height));
    window.setFramerateLimit(60);
    
    if (cuda_result.completed) {
        convert_to_texture(cuda_result.image_data, result_texture);
        result_sprite.setTexture(result_texture);
        result_sprite.setPosition(0, 0);
    }
    
    setup_minimap();
    
    image_view.reset(sf::FloatRect(0, 0, render_width, render_height));
    image_view.setCenter(render_width / 2.0f, render_height / 2.0f);
}

void MandelbrotGUI::setup_minimap() {
    const int minimap_size = 120;
    const int margin = 10;
    
    minimap_frame.setSize(sf::Vector2f(minimap_size + 4, minimap_size + 4));
    minimap_frame.setPosition(render_width - minimap_size - margin - 2, margin);
    minimap_frame.setFillColor(sf::Color::Transparent);
    minimap_frame.setOutlineColor(sf::Color(150, 200, 255));
    minimap_frame.setOutlineThickness(2);
    
    minimap_bg.setSize(sf::Vector2f(minimap_size + 10, minimap_size + 35));
    minimap_bg.setPosition(render_width - minimap_size - margin - 5, margin - 5);
    minimap_bg.setFillColor(sf::Color(30, 30, 30, 200));
    minimap_bg.setOutlineColor(sf::Color(100, 100, 100));
    minimap_bg.setOutlineThickness(1);
    
    minimap_label.setString("Julia Preview");
    minimap_label.setCharacterSize(12);
    minimap_label.setFillColor(sf::Color::White);
    minimap_label.setPosition(render_width - minimap_size - margin + 10, margin - 2);
    if (font_loaded) minimap_label.setFont(font);
    
    minimap_coords.setString("Hover for preview");
    minimap_coords.setCharacterSize(10);
    minimap_coords.setFillColor(sf::Color(180, 200, 220));
    minimap_coords.setPosition(render_width - minimap_size - margin, margin + minimap_size + 8);
    if (font_loaded) minimap_coords.setFont(font);
    
    minimap_data.resize(minimap_size * minimap_size);

    std::complex<double> default_julia(-0.7, 0.27015);
    generate_mini_julia_preview(default_julia);
}

void MandelbrotGUI::render_results_view() {
    if (cuda_result.completed) {
        window.setView(image_view);
        window.draw(result_sprite);

        if (is_selecting_zoom) {
            sf::Vector2f start_view = window.mapPixelToCoords(drag_start, image_view);
            sf::Vector2f current_view = window.mapPixelToCoords(current_mouse_pos, image_view);
            
            float left = std::min(start_view.x, current_view.x);
            float top = std::min(start_view.y, current_view.y);
            float width = abs(current_view.x - start_view.x);
            float height = abs(current_view.y - start_view.y);

            selection_rect.setPosition(left, top);
            selection_rect.setSize(sf::Vector2f(width, height));
            selection_rect.setFillColor(sf::Color(255, 255, 255, 30));
            selection_rect.setOutlineColor(sf::Color::White);
            selection_rect.setOutlineThickness(1.5f);
            window.draw(selection_rect);

            float corner_size = std::min(width, height) * 0.05f;
            corner_size = std::max(2.0f, std::min(corner_size, 8.0f));
            
            sf::RectangleShape corner;
            corner.setFillColor(sf::Color::White);
            corner.setSize(sf::Vector2f(corner_size, corner_size));

            corner.setPosition(left - corner_size/2, top - corner_size/2);
            window.draw(corner);

            corner.setPosition(left + width - corner_size/2, top - corner_size/2);
            window.draw(corner);

            corner.setPosition(left - corner_size/2, top + height - corner_size/2);
            window.draw(corner);

            corner.setPosition(left + width - corner_size/2, top + height - corner_size/2);
            window.draw(corner);
        }

        window.setView(window.getDefaultView());
    }

    render_minimap();
}

void MandelbrotGUI::render_minimap() {
    window.draw(minimap_bg);
    window.draw(minimap_label);
    window.draw(minimap_frame);

    if (minimap_texture.getSize().x > 0) {
        minimap_sprite.setTexture(minimap_texture);
        minimap_sprite.setPosition(minimap_frame.getPosition().x + 2, minimap_frame.getPosition().y + 2);
        window.draw(minimap_sprite);
    }
    
    window.draw(minimap_coords);
}

void MandelbrotGUI::handle_results_view_events(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Escape) {
            window.create(sf::VideoMode(INPUT_WINDOW_WIDTH, INPUT_WINDOW_HEIGHT), "Mandelbrot CUDA Generator - Input");
            current_view = ViewMode::INPUT_PANEL;
            std::cout << "Returned to input panel" << std::endl;
            return;
        } else if (event.key.code == sf::Keyboard::S) {
            show_save_popup();
            return;
        } else if (event.key.code == sf::Keyboard::J) {
            show_julia_popup();
            return;
        } else if (event.key.code == sf::Keyboard::R) {
            reset_view();
            return;
        } else if (event.key.code == sf::Keyboard::Equal || event.key.code == sf::Keyboard::Add) {
            perform_zoom(1.1f);
            return;
        } else if (event.key.code == sf::Keyboard::Hyphen || event.key.code == sf::Keyboard::Subtract) {
            perform_zoom(1.0f / 1.1f);
            return;
        }
    }

    if (event.type == sf::Event::MouseWheelScrolled) {
        if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
            float zoom_factor = (event.mouseWheelScroll.delta > 0) ? 1.05f : (1.0f / 1.05f);
            perform_zoom(zoom_factor);
        }
        return;
    }
    
    if (event.type == sf::Event::MouseWheelMoved) {
        float zoom_factor = (event.mouseWheel.delta > 0) ? 1.05f : (1.0f / 1.05f);
        perform_zoom(zoom_factor);
        return;
    }
    
    handle_zoom_pan_events(event);
    
    if (event.type == sf::Event::MouseMoved) {
        sf::Vector2i mouse_pos(event.mouseMove.x, event.mouseMove.y);
        update_julia_preview(mouse_pos);
    }
}

void MandelbrotGUI::handle_zoom_pan_events(const sf::Event& event) {
    if (!cuda_result.completed) return;
    
    if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            is_selecting_zoom = true;
            drag_start = sf::Vector2i(event.mouseButton.x, event.mouseButton.y);
            current_mouse_pos = drag_start;
        } else if (event.mouseButton.button == sf::Mouse::Right) {
            if (zoom_factor > 1.0f) {
                is_dragging = true;
                drag_start = sf::Vector2i(event.mouseButton.x, event.mouseButton.y);
            }
        }
    }
    
    if (event.type == sf::Event::MouseMoved) {
        current_mouse_pos = sf::Vector2i(event.mouseMove.x, event.mouseMove.y);
        
        if (is_dragging && zoom_factor > 1.0f) {
            sf::Vector2f delta((current_mouse_pos.x - drag_start.x) * -1.0f, 
                              (current_mouse_pos.y - drag_start.y) * -1.0f);
            apply_pan(delta);
            drag_start = current_mouse_pos;
            is_zoom_changed = true;
        }
    }
    
    if (event.type == sf::Event::MouseButtonReleased) {
        if (event.mouseButton.button == sf::Mouse::Left && is_selecting_zoom) {
            is_selecting_zoom = false;
            
            int width = abs(current_mouse_pos.x - drag_start.x);
            int height = abs(current_mouse_pos.y - drag_start.y);
            
            if (width > 20 && height > 20) {
                zoom_to_selection();
                is_zoom_changed = true;
            }
        } else if (event.mouseButton.button == sf::Mouse::Right) {
            is_dragging = false;
        }
    }
}


void MandelbrotGUI::handle_mouse_wheel(float delta) {
    if (delta > 0) {
        perform_zoom(1.2f);
    } else if (delta < 0) {
        perform_zoom(1.0f / 1.2f);
    }
}

void MandelbrotGUI::perform_zoom(float factor) {
    if (!cuda_result.completed) return;
    
    float new_zoom_factor = zoom_factor * factor;
    
    const float MIN_ZOOM = 1.0f;
    const float MAX_ZOOM = 50.0f;
    
    if (new_zoom_factor < MIN_ZOOM || new_zoom_factor > MAX_ZOOM) {
        return;
    }
    
    sf::Vector2i mouse_pos = sf::Mouse::getPosition(window);
    if (mouse_pos.x < 0 || mouse_pos.x >= render_width || 
        mouse_pos.y < 0 || mouse_pos.y >= render_height) {
        mouse_pos.x = render_width / 2;
        mouse_pos.y = render_height / 2;
    }
    
    sf::Vector2f current_size = image_view.getSize();
    sf::Vector2f new_size = current_size;
    new_size.x /= factor;
    new_size.y /= factor;
    
    const float MIN_VIEW_SIZE = (float)std::min(render_width, render_height) / 50.0f;
    if (new_size.x < MIN_VIEW_SIZE || new_size.y < MIN_VIEW_SIZE) {
        return;
    }
    
    sf::Vector2f world_pos = window.mapPixelToCoords(mouse_pos, image_view);
    
    image_view.setSize(new_size);
    image_view.setCenter(world_pos);
    
    constrain_view_to_bounds();
    
    zoom_factor = new_zoom_factor;
    update_fractal_bounds();
    is_zoom_changed = true;
}

void MandelbrotGUI::zoom_to_selection() {
    sf::Vector2f start_world = window.mapPixelToCoords(drag_start, image_view);
    sf::Vector2f end_world = window.mapPixelToCoords(current_mouse_pos, image_view);

    float selection_left = std::min(start_world.x, end_world.x);
    float selection_right = std::max(start_world.x, end_world.x);
    float selection_top = std::min(start_world.y, end_world.y);
    float selection_bottom = std::max(start_world.y, end_world.y);
    
    float selection_width = selection_right - selection_left;
    float selection_height = selection_bottom - selection_top;

    sf::Vector2f selection_center(
        selection_left + selection_width / 2.0f,
        selection_top + selection_height / 2.0f
    );

    float window_width = (float)render_width;
    float window_height = (float)render_height;

    float scale_x = selection_width / window_width;
    float scale_y = selection_height / window_height;

    float scale = std::max(scale_x, scale_y);

    scale *= 1.05f;

    sf::Vector2f new_view_size(window_width * scale, window_height * scale);

    sf::Vector2f current_view_size = image_view.getSize();
    float new_zoom_factor = zoom_factor * (current_view_size.x / new_view_size.x);

    const float MAX_ZOOM = 50.0f;
    if (new_zoom_factor > MAX_ZOOM) {
        float limit_scale = MAX_ZOOM / new_zoom_factor;
        new_view_size.x *= (1.0f / limit_scale);
        new_view_size.y *= (1.0f / limit_scale);
        new_zoom_factor = MAX_ZOOM;
    }

    image_view.setSize(new_view_size);
    image_view.setCenter(selection_center);

    zoom_factor = new_zoom_factor;

    constrain_view_to_bounds();
    update_fractal_bounds();
    
    std::cout << "[SELECTION] Zoomed to area: " << std::fixed << std::setprecision(1) 
              << selection_width << "x" << selection_height << " → " << std::setprecision(2) 
              << zoom_factor << "x" << std::endl;
}

void MandelbrotGUI::zoom_at_point(sf::Vector2i point, float factor) {
    sf::Vector2f world_pos = window.mapPixelToCoords(point, image_view);
    
    sf::Vector2f view_size = image_view.getSize();
    view_size.x /= factor;
    view_size.y /= factor;

    float new_zoom_factor = zoom_factor * factor;
    if (new_zoom_factor < 1.0f) {
        return;
    }
    
    image_view.setSize(view_size);
    image_view.setCenter(world_pos);
    
    zoom_factor = new_zoom_factor;
    update_fractal_bounds();
}

void MandelbrotGUI::constrain_view_to_bounds() {
    sf::Vector2f view_center = image_view.getCenter();
    sf::Vector2f view_size = image_view.getSize();

    float view_left = view_center.x - view_size.x / 2.0f;
    float view_right = view_center.x + view_size.x / 2.0f;
    float view_top = view_center.y - view_size.y / 2.0f;
    float view_bottom = view_center.y + view_size.y / 2.0f;

    float image_left = 0.0f;
    float image_right = (float)render_width;
    float image_top = 0.0f;
    float image_bottom = (float)render_height;

    if (view_size.x >= render_width) {
        view_center.x = render_width / 2.0f;
    } else {
        if (view_left < image_left) {
            view_center.x = image_left + view_size.x / 2.0f;
        } else if (view_right > image_right) {
            view_center.x = image_right - view_size.x / 2.0f;
        }
    }
    
    if (view_size.y >= render_height) {
        view_center.y = render_height / 2.0f;
    } else {
        if (view_top < image_top) {
            view_center.y = image_top + view_size.y / 2.0f;
        } else if (view_bottom > image_bottom) {
            view_center.y = image_bottom - view_size.y / 2.0f;
        }
    }
    
    image_view.setCenter(view_center);
}

void MandelbrotGUI::apply_pan(sf::Vector2f delta) {
    sf::Vector2f center = image_view.getCenter();
    image_view.setCenter(center + delta);

    constrain_view_to_bounds();
    update_fractal_bounds();
}

void MandelbrotGUI::reset_view() {
    if (zoom_factor <= 1.0f) {
        std::cout << "[RESET] Already at original size (1.0x)" << std::endl;
        return;
    }
    
    image_view.reset(sf::FloatRect(0, 0, render_width, render_height));
    image_view.setCenter(render_width / 2.0f, render_height / 2.0f);
    
    x_min = original_x_min;
    x_max = original_x_max;
    y_min = original_y_min;
    y_max = original_y_max;
    
    zoom_factor = 1.0f;
    is_zoom_changed = true;
    
    std::cout << "[RESET] View reset to original size (1.0x)" << std::endl;
}

void MandelbrotGUI::update_fractal_bounds() {
    sf::Vector2f view_center = image_view.getCenter();
    sf::Vector2f view_size = image_view.getSize();
    
    float left = view_center.x - view_size.x / 2;
    float top = view_center.y - view_size.y / 2;
    float right = view_center.x + view_size.x / 2;
    float bottom = view_center.y + view_size.y / 2;
    
    float scale_x = (original_x_max - original_x_min) / render_width;
    float scale_y = (original_y_max - original_y_min) / render_height;

    x_min = original_x_min + left * scale_x;
    x_max = original_x_min + right * scale_x;
    y_min = original_y_min + top * scale_y;
    y_max = original_y_min + bottom * scale_y;
    
    const double BOUND_LIMIT = 100.0;

    x_min = std::max(x_min, -BOUND_LIMIT);
    x_max = std::min(x_max, BOUND_LIMIT);
    y_min = std::max(y_min, -BOUND_LIMIT);
    y_max = std::min(y_max, BOUND_LIMIT);

    const double MIN_BOUND_SIZE = 1e-10;
    if ((x_max - x_min) < MIN_BOUND_SIZE) {
        double center_x = (x_min + x_max) / 2.0;
        x_min = center_x - MIN_BOUND_SIZE / 2.0;
        x_max = center_x + MIN_BOUND_SIZE / 2.0;
    }
    
    if ((y_max - y_min) < MIN_BOUND_SIZE) {
        double center_y = (y_min + y_max) / 2.0;
        y_min = center_y - MIN_BOUND_SIZE / 2.0;
        y_max = center_y + MIN_BOUND_SIZE / 2.0;
    }
}

void MandelbrotGUI::real_time_rerender() {
    if (!generator || !cuda_available || !cuda_result.completed || !is_zoom_changed) return;
    
    std::cout << "[RERENDER] Real-time CUDA recalculation..." << std::endl;
    std::cout << "[RERENDER] Zoom factor: " << zoom_factor << "x" << std::endl;
    std::cout << "[RERENDER] Bounds: x[" << std::fixed << std::setprecision(6) << x_min << ", " << x_max << "] y[" << y_min << ", " << y_max << "]" << std::endl;
    
    generator->set_bounds(x_min, x_max, y_min, y_max);

    auto start = std::chrono::high_resolution_clock::now();
    generator->generate_cuda(cuda_result.image_data);
    auto end = std::chrono::high_resolution_clock::now();
    
    convert_to_texture(cuda_result.image_data, result_texture);
    result_sprite.setTexture(result_texture);
    
    double render_time = std::chrono::duration<double>(end - start).count();
    cuda_result.render_time = render_time;

    std::cout << "[RERENDER] CUDA recalculation complete: " << std::fixed << std::setprecision(3) 
              << render_time << "s (zoom: " << std::setprecision(2) << zoom_factor << "x)" << std::endl;
    
    is_zoom_changed = false;
}

void MandelbrotGUI::update_julia_preview(sf::Vector2i mouse_pos) {
    if (mouse_pos.x < 0 || mouse_pos.x >= render_width || 
        mouse_pos.y < 0 || mouse_pos.y >= render_height) {
        minimap_coords.setString("Hover for preview");
        return;
    }

    std::complex<double> preview_constant = screen_to_complex(mouse_pos);

    std::stringstream ss;
    ss << "c = " << std::fixed << std::setprecision(6) 
       << preview_constant.real() << " + " << preview_constant.imag() << "i";
    minimap_coords.setString(ss.str());

    generate_mini_julia_preview(preview_constant);
}

void MandelbrotGUI::generate_mini_julia_preview(std::complex<double> c) {
    const int minimap_size = 120;

    const double julia_x_min = -2.0;
    const double julia_x_max = 2.0; 
    const double julia_y_min = -2.0;
    const double julia_y_max = 2.0;

    int preview_iterations = std::min(100, max_iterations);
    double color_scale = (double)max_iterations / preview_iterations;
    
    for (int y = 0; y < minimap_size; y++) {
        for (int x = 0; x < minimap_size; x++) {
            double real = julia_x_min + (julia_x_max - julia_x_min) * x / (minimap_size - 1);
            double imag = julia_y_min + (julia_y_max - julia_y_min) * y / (minimap_size - 1);
            
            std::complex<double> z(real, imag);
            int iterations = 0;

            while (std::abs(z) <= 2.0 && iterations < preview_iterations) {
                z = z * z + c;
                iterations++;
            }

            if (iterations == preview_iterations) {
                iterations = max_iterations;
            } else {
                iterations = (int)(iterations * color_scale);
            }

            Color color;
            if (iterations == max_iterations) {
                color = Color(0, 0, 0);
            } else {
                double t = (double)iterations / max_iterations;
                color.r = (unsigned char)(9 * (1 - t) * t * t * t * 255);
                color.g = (unsigned char)(15 * (1 - t) * (1 - t) * t * t * 255);
                color.b = (unsigned char)(8.5 * (1 - t) * (1 - t) * (1 - t) * t * 255);
            }
            
            minimap_data[y * minimap_size + x] = color;
        }
    }

    std::vector<sf::Uint8> pixel_data;
    pixel_data.resize(minimap_data.size() * 4);
    
    for (size_t i = 0; i < minimap_data.size(); i++) {
        pixel_data[i * 4] = minimap_data[i].r;
        pixel_data[i * 4 + 1] = minimap_data[i].g;
        pixel_data[i * 4 + 2] = minimap_data[i].b;
        pixel_data[i * 4 + 3] = 255;
    }
    
    minimap_texture.create(minimap_size, minimap_size);
    minimap_texture.update(pixel_data.data());
}

void MandelbrotGUI::show_save_popup() {
    std::cout << "\n=== AUTO SAVE ===" << std::endl;
    save_current_view();
}

void MandelbrotGUI::show_julia_popup() {
    sf::Vector2i mouse_pos = sf::Mouse::getPosition(window);
    
    if (mouse_pos.x < 0 || mouse_pos.x >= render_width || 
        mouse_pos.y < 0 || mouse_pos.y >= render_height) {
        std::cout << "\n=== JULIA SET ===" << std::endl;
        std::cout << "Please hover mouse over mandelbrot area first!" << std::endl;
        return;
    }

    julia_constant = screen_to_complex(mouse_pos);
    
    std::cout << "\n=== AUTO JULIA GENERATION ===" << std::endl;
    std::cout << "Julia constant: " << std::fixed << std::setprecision(12) 
              << julia_constant.real() << " + " << julia_constant.imag() << "i" << std::endl;
    generate_julia_set();
}

void MandelbrotGUI::generate_julia_set() {
    if (!generator) return;
    
    std::cout << "Generating Julia set with EXACT preview constant..." << std::endl;
    std::cout << "Julia constant: " << std::fixed << std::setprecision(12) 
              << julia_constant.real() << " + " << julia_constant.imag() << "i" << std::endl;
    
    julia_image_data.resize(render_width * render_height);
    
    auto start = std::chrono::high_resolution_clock::now();
    
    #ifdef USE_CUDA
    if (cuda_available) {
        generator->generate_julia_cuda(julia_image_data, julia_constant);
    } else {
        generator->generate_julia_serial(julia_image_data, julia_constant);
    }
    #else
    generator->generate_julia_serial(julia_image_data, julia_constant);
    #endif
    
    auto end = std::chrono::high_resolution_clock::now();
    double render_time = std::chrono::duration<double>(end - start).count();
    
    julia_rendered = true;
    
    std::cout << "Julia set generated in " << render_time << " seconds" << std::endl;

    std::stringstream ss;
    ss << "../output/julia_" << render_width << "x" << render_height 
       << "_iter" << max_iterations;

    if (julia_constant.real() >= 0) {
        ss << "_c+" << std::fixed << std::setprecision(6) << julia_constant.real();
    } else {
        ss << "_c" << std::fixed << std::setprecision(6) << julia_constant.real();
    }
    
    if (julia_constant.imag() >= 0) {
        ss << "+i" << std::fixed << std::setprecision(6) << julia_constant.imag();
    } else {
        ss << "i" << std::fixed << std::setprecision(6) << julia_constant.imag();
    }
    
    ss << "_" << get_timestamp() << ".bmp";
    
    std::string filename = ss.str();
    
    generator->save_bmp(julia_image_data, filename);
    std::cout << "Julia set saved: " << filename << std::endl;
    std::cout << "Preview constant matches saved constant exactly!" << std::endl;
}

void MandelbrotGUI::save_current_view() {
    if (!cuda_result.completed) return;
    
    std::string filename = "../output/mandelbrot_" + 
                          std::to_string(render_width) + "x" + std::to_string(render_height) + 
                          "_zoom" + std::to_string((int)zoom_factor) + "x_" +
                          get_timestamp() + ".bmp";
    
    generator->save_bmp(cuda_result.image_data, filename);
    
    std::cout << "Current view saved: " << filename << std::endl;
}

void MandelbrotGUI::convert_to_texture(const std::vector<Color>& image_data, sf::Texture& texture) {
    std::vector<sf::Uint8> pixel_data;
    pixel_data.resize(image_data.size() * 4);
    
    for (size_t i = 0; i < image_data.size(); i++) {
        pixel_data[i * 4] = image_data[i].r;
        pixel_data[i * 4 + 1] = image_data[i].g;
        pixel_data[i * 4 + 2] = image_data[i].b;
        pixel_data[i * 4 + 3] = 255;
    }
    
    texture.create(render_width, render_height);
    texture.update(pixel_data.data());
}

void MandelbrotGUI::create_output_directory() {
    try {
        std::filesystem::create_directories("../output");
    } catch (const std::exception& e) {
        std::cerr << "Failed to create output directory: " << e.what() << std::endl;
    }
}

std::string MandelbrotGUI::get_timestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y%m%d_%H%M%S");
    return ss.str();
}

bool MandelbrotGUI::is_point_in_rect(int x, int y, const sf::RectangleShape& rect) {
    sf::Vector2f pos = rect.getPosition();
    sf::Vector2f size = rect.getSize();
    
    return x >= pos.x && x <= pos.x + size.x &&
           y >= pos.y && y <= pos.y + size.y;
}

bool MandelbrotGUI::check_cuda_availability() {
    #ifdef USE_CUDA
    int device_count;
    cudaError_t err = cudaGetDeviceCount(&device_count);
    return (err == cudaSuccess && device_count > 0);
    #else
    return false;
    #endif
}

void MandelbrotGUI::update_julia_constant_from_mouse(sf::Vector2i mouse_pos) {
    double relative_x = (double)mouse_pos.x / (render_width - 1);
    double relative_y = (double)mouse_pos.y / (render_height - 1);
    
    double real = x_min + (x_max - x_min) * relative_x;
    double imag = y_min + (y_max - y_min) * relative_y;

    julia_constant = std::complex<double>(real, imag);
}

std::complex<double> MandelbrotGUI::screen_to_complex(sf::Vector2i screen_pos) {
    if (screen_pos.x < 0 || screen_pos.x >= render_width || 
        screen_pos.y < 0 || screen_pos.y >= render_height) {
        return std::complex<double>(0, 0);
    }

    double relative_x = (double)screen_pos.x / (render_width - 1);
    double relative_y = (double)screen_pos.y / (render_height - 1);
    
    double real = x_min + (x_max - x_min) * relative_x;
    double imag = y_min + (y_max - y_min) * relative_y;
    
    return std::complex<double>(real, imag);
}

bool MandelbrotGUI::is_mouse_over_image(sf::Vector2i mouse_pos) {
    return mouse_pos.x >= 0 && mouse_pos.x < render_width &&
           mouse_pos.y >= 0 && mouse_pos.y < render_height;
}