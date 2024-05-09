#include "../include/ofApp.hpp"
#include "../include/params.hpp"


ofApp::ofApp(MAPFPlan* _P): P(_P)
{
  flg_autoplay = true;
  flg_loop = true;
  flg_goal = true;
  flg_font = false;
  flg_focus = false;
  flg_logo_gen = false;
  line_mode = LINE_MODE::STRAIGHT;
}

ofApp::~ofApp()
{
  delete P;
}

void ofApp::setup()
{
  // setup size
  int map_w = P->G->getWidth();
  int map_h = P->G->getHeight();
  int window_max_w = BufferSize::default_screen_width
    - BufferSize::screen_x_buffer * 2 - BufferSize::window_x_buffer * 2;
  int window_max_h = BufferSize::default_screen_height
    - BufferSize::window_y_top_buffer - BufferSize::window_y_bottom_buffer;
  scale = std::min(window_max_w / map_w, window_max_h / map_h) + 1;
  int w = map_w * scale + 2 * BufferSize::window_x_buffer;
  int h = map_h * scale
    + BufferSize::window_y_top_buffer + BufferSize::window_y_bottom_buffer;

  agent_rad = std::max((float)scale/std::sqrt(2)/2, 3.0);
  goal_rad  = std::max((float)scale/4.0, 2.0);
  font_size = std::max(scale/8, 6);

  ofSetWindowShape(w, h);
  ofBackground(Color::bg);
  ofSetCircleResolution(32);
  ofSetFrameRate(30);

  std::string font_name = "MuseoModerno-VariableFont_wght.ttf";
  font.load(font_name, font_size);
  font_info.load(font_name, 10);

  // setup gui
  gui.setup();
  gui.add(timestep_slider.setup("time step", 0, 0, P->makespan));
  gui.add(speed_slider.setup("speed", 0.1, 0, 1));
  gui.add(agent_slider.setup("agent", 0, 0, P->num_agents-1));

  printKeys();
}

void ofApp::update()
{
  if (!flg_autoplay) return;

  // t <- t + speed
  float t;
  t = timestep_slider + speed_slider;
  if (t <= P->makespan) {
    timestep_slider = t;
  } else {
    timestep_slider = 0;
    if (flg_loop) {
      timestep_slider = 0;
    } else {
      timestep_slider = P->makespan;
    }
  }
}

void ofApp::draw()
{
  // draw nodes
  ofSetLineWidth(1);
  ofFill();
  for (int x = 0; x < P->G->getWidth(); ++x) {
    for (int y = 0; y < P->G->getHeight(); ++y) {
      if (!P->G->existNode(x, y)) continue;
      ofSetColor(Color::node);
      int x_draw = x*scale-scale/2+0.5
        + BufferSize::window_x_buffer + scale/2;
      int y_draw = y*scale-scale/2+0.5
        + BufferSize::window_y_top_buffer + scale/2;
      if (!flg_logo_gen) {  // default
        ofDrawRectangle(x_draw, y_draw, scale-0.5, scale-0.5);
      } else {
        ofDrawRectangle(x_draw, y_draw, scale, scale);
      }
      if (flg_font) {
        ofSetColor(Color::font);
        font.drawString(std::to_string(y*P->G->getWidth()+x),
                        x_draw + 1, y_draw + font_size + 1);
      }
    }
  }

  // draw goals
  if (flg_goal) {
    for (int i = 0; i < P->num_agents; ++i) {
      if (flg_focus && i != agent_slider) continue;
      ofSetColor(Color::agents[i % Color::agents.size()]);
      Node* g;
      int t1 = (int)timestep_slider;
      if (!P->config_g.empty()) {
        g = P->config_g[i];  // mapf
      } else if (!P->targets.empty() && t1 <= P->targets.size() - 1) {
        g = P->targets[t1][i];  // mapd
      } else {
        std::cout << "invalid result file" << std::endl;
        std::exit(1);
      }

      Pos pos1 = g->pos * scale;
      int x = pos1.x + BufferSize::window_x_buffer + scale/2;
      int y = pos1.y + BufferSize::window_y_top_buffer + scale/2;
      ofDrawRectangle(x - goal_rad/2, y - goal_rad/2, goal_rad, goal_rad);
    }
  }

  // draw agents
  for (int i = 0; i < P->num_agents; ++i) {
    if (flg_focus && i != agent_slider) continue;
    ofSetColor(Color::agents[i % Color::agents.size()]);
    int t1 = (int)timestep_slider;
    int t2 = t1 + 1;

    // agent position
    Node* v = P->transitions[t1][i];
    Pos pos1 = v->pos;
    float x = pos1.x;
    float y = pos1.y;

    if (t2 <= P->makespan) {
      Pos pos2 = P->transitions[t2][i]->pos;
      x += (pos2.x - x) * (timestep_slider - t1);
      y += (pos2.y - y) * (timestep_slider - t1);
    }
    x *= scale;
    y *= scale;
    x += BufferSize::window_x_buffer + scale/2;
    y += BufferSize::window_y_top_buffer + scale/2;

    if (!flg_logo_gen) { // default
      ofDrawCircle(x, y, agent_rad);
    } else {
      ofSetColor(Color::bg);
      ofDrawRectangle(x-scale/2,y-scale/2,scale,scale);
    }

    // goal
    if (line_mode == LINE_MODE::STRAIGHT) {
      Pos pos3 = v->pos;
      if (!P->config_g.empty()) {
        pos3 = P->config_g[i]->pos * scale;
      } else if (!P->targets.empty() && t1 <= P->targets.size() - 1) {
        pos3 = P->targets[t1][i]->pos * scale;
      } else {
        std::cout << "invalid result file" << std::endl;
        std::exit(1);
      }
      ofDrawLine(pos3.x + BufferSize::window_x_buffer + scale/2,
                 pos3.y + BufferSize::window_y_top_buffer + scale/2, x, y);
    } else if (line_mode == LINE_MODE::PATH) {
      // next loc
      ofSetLineWidth(2);
      Pos pos2(x-BufferSize::window_x_buffer-scale/2, y-BufferSize::window_y_top_buffer-scale/2);
      for (int t = t1; t < P->makespan; ++t) {
        Pos pos3 = P->transitions[t+1][i]->pos * scale;
        if (pos3 == pos2) continue;
        ofDrawLine(pos2.x + BufferSize::window_x_buffer + scale/2,
                   pos2.y + BufferSize::window_y_top_buffer + scale/2,
                   pos3.x + BufferSize::window_x_buffer + scale/2,
                   pos3.y + BufferSize::window_y_top_buffer + scale/2);
        pos2 = pos3;
      }
      ofSetLineWidth(1);
    }


    // agent at goal
    if (!P->config_g.empty() && v == P->config_g[i] && !flg_logo_gen) {
      ofSetColor(255,255,255);
      ofDrawCircle(x, y, agent_rad-2);
    } else if (!P->assigned.empty() && !P->assigned[t1].empty() && P->assigned[t1][i]) {
      ofSetColor(255,255,255);
      ofDrawCircle(x, y, agent_rad-2);
    }

    // id
    if (flg_font) {
      ofSetColor(Color::font);
      font.drawString(std::to_string(i), x-font_size/2, y+font_size/2);
    }
  }

  // info
  ofSetColor(Color::font_info);
  int x = 220;
  int y = 5;
  font_info.drawString("solved by "
                       + P->solver
                       + (P->solved ? ", success" : ", failed"),
                       x, y+=15);
  font_info.drawString("agents: "
                       + std::to_string(P->num_agents)
                       + ", map: " + P->G->getMapFileName(),
                       x, y+=15);
  font_info.drawString("comp_time: "
                       + std::to_string(P->comp_time) + " ms",
                       x, y+=15);
  font_info.drawString("soc: " + std::to_string(P->soc)
                       + ", makespan: " + std::to_string(P->makespan)
                       + ", service_time(ave):" + std::to_string((int)P->service_time),
                       x, y+=15);

  gui.draw();
}

void ofApp::keyPressed(int key) {
  if (key == 'r') timestep_slider = 0;  // reset
  if (key == 'p') flg_autoplay = !flg_autoplay;
  if (key == 'l') flg_loop = !flg_loop;
  if (key == 'v') {
    line_mode = static_cast<LINE_MODE>(((int)line_mode + 1)%(int)LINE_MODE::NUM);
  }
  if (key == 'a') flg_focus = !flg_focus;
  if (key == 'g') flg_goal = !flg_goal;
  if (key == 'f') {
    flg_font = !flg_font;
    flg_font &= (scale - font_size > 6);
  }
  if (key == 'i') {
    if (!flg_logo_gen) {
      flg_logo_gen = true;
      flg_focus = false;
      flg_goal = false;
      flg_font = false;
      line_mode = LINE_MODE::NONE;
    } else {
      flg_logo_gen = false;
      flg_goal = true;
    }
  }

  float t;
  if (key == OF_KEY_RIGHT) {
    t = timestep_slider + speed_slider;
    timestep_slider = std::min((float)P->makespan, t);
  }
  if (key == OF_KEY_LEFT) {
    t = timestep_slider - speed_slider;
    timestep_slider = std::max((float)0, t);
  }
  if (key == OF_KEY_UP) {
    t = speed_slider + 0.001;
    speed_slider = std::min(t, (float)speed_slider.getMax());
  }
  if (key == OF_KEY_DOWN) {
    t = speed_slider - 0.001;
    speed_slider = std::max(t, (float)speed_slider.getMin());
  }
  if (key == '+') {
    agent_slider = std::min(agent_slider+1, P->num_agents);
  }
  if (key == '-') {
    agent_slider = std::max(agent_slider-1, 0);
  }
}

void ofApp::dragEvent(ofDragInfo dragInfo) {}
void ofApp::keyReleased(int key) {}
void ofApp::mouseMoved(int x, int y) {}
void ofApp::mouseDragged(int x, int y, int button) {}
void ofApp::mousePressed(int x, int y, int button) {}
void ofApp::mouseReleased(int x, int y, int button) {}
void ofApp::mouseEntered(int x, int y) {}
void ofApp::mouseExited(int x, int y) {}
void ofApp::windowResized(int w, int h) {}
void ofApp::gotMessage(ofMessage msg) {}

void ofApp::printKeys()
{
  std::cout << "keys for visualizer" << std::endl;
  std::cout << "- p : play or pause" << std::endl;
  std::cout << "- l : loop or not" << std::endl;
  std::cout << "- r : reset" << std::endl;
  std::cout << "- v : show virtual line to goals" << std::endl;
  std::cout << "- f : show agent & node id" << std::endl;
  std::cout << "- g : show goals" << std::endl;
  std::cout << "- right : progress" << std::endl;
  std::cout << "- left  : back" << std::endl;
  std::cout << "- up    : speed up" << std::endl;
  std::cout << "- down  : speed down" << std::endl;
  std::cout << "- a : show single agent" << std::endl;
  std::cout << "- + : increment single agent id" << std::endl;
  std::cout << "- - : decrement single agent id" << std::endl;
  std::cout << "- i : monotonic" << std::endl;
  std::cout << "- esc : terminate" << std::endl;
}
