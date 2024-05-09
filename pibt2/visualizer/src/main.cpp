#include "ofMain.h"
#include "../include/ofApp.hpp"
#include <iostream>
#include <fstream>
#include <regex>
#include "../include/mapfplan.hpp"

void readSetResult(const std::string& result_file, MAPFPlan* plan);
void readSetNode(const std::string& s, Config& config, Grid* G,
                 Config& targets, std::vector<bool>& assigned);
void readSetNode(const std::string& s, Config& config, Grid* G);

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cout << "Put your result file as the first arg." << std::endl;
    return 0;
  }

  MAPFPlan* solution = new MAPFPlan;  // deleted in ofApp destructor
  readSetResult(argv[1], solution);
  ofSetupOpenGL(100, 100, OF_WINDOW);
  ofRunApp(new ofApp(solution));
  return 0;
}

void readSetNode(const std::string& s, Config& config, Grid* G)
{
  Config targets;
  std::vector<bool> assigned;
  readSetNode(s, config, G, targets, assigned);
}

void readSetNode(const std::string& s, Config& config, Grid* G,
                 Config& targets, std::vector<bool>& assigned)
{
  if (G == nullptr) {
    std::cout << "error@main, no graph" << std::endl;
    std::exit(1);
  }
  std::regex r_pos = std::regex(R"(\((\d+),(\d+)\),)");
  std::regex r_pos_targets = std::regex(R"(\((\d+),(\d+)\)->\((\d+),(\d+)\):(-1|\d+),)");
  std::smatch m;

  // mapf
  auto iter = s.cbegin();
  while (std::regex_search(iter, s.cend(), m, r_pos)) {
    iter = m[0].second;
    int x = std::stoi(m[1].str());
    int y = std::stoi(m[2].str());
    if (!G->existNode(x, y)) {
      std::cout << "error@main, node does not exist" << std::endl;
      delete G;
      std::exit(1);
    }
    config.push_back(G->getNode(x, y));
  }

  if (!config.empty()) return;

  // mapd
  iter = s.cbegin();
  while (std::regex_search(iter, s.cend(), m, r_pos_targets)) {
    iter = m[0].second;
    int x = std::stoi(m[1].str());
    int y = std::stoi(m[2].str());
    if (!G->existNode(x, y)) {
      std::cout << "error@main, node does not exist" << std::endl;
      delete G;
      std::exit(1);
    }
    config.push_back(G->getNode(x, y));

    x = std::stoi(m[3].str());
    y = std::stoi(m[4].str());
    if (!G->existNode(x, y)) {
      std::cout << "error@main, node does not exist" << std::endl;
      delete G;
      std::exit(1);
    }
    targets.push_back(G->getNode(x, y));
    assigned.push_back(std::stoi(m[5].str()) != -1);
  }
}

void readSetResult(const std::string& result_file, MAPFPlan* plan)
{
  std::ifstream file(result_file);
  if (!file) {
    std::cout << "error@main," << "file " << result_file << " is not found." << std::endl;
    std::exit(1);
  };

  std::regex r_map       = std::regex(R"(map_file=(.+))");
  std::regex r_agents    = std::regex(R"(agents=(.+))");
  std::regex r_solver    = std::regex(R"(solver=(.+))");
  std::regex r_solved    = std::regex(R"(solved=(\d))");
  std::regex r_soc       = std::regex(R"(soc=(\d+))");
  std::regex r_makespan  = std::regex(R"(makespan=(\d+))");
  std::regex r_comp_time = std::regex(R"(comp_time=(\d+))");
  std::regex r_starts    = std::regex(R"(starts=(.+))");
  std::regex r_goals     = std::regex(R"(goals=(.+))");
  std::regex r_sol       = std::regex(R"(solution=)");
  std::regex r_config    = std::regex(R"(\d+:(.+))");
  std::regex r_service_time = std::regex(R"(service_time=(.+))");

  std::string line;
  std::smatch results;
  while (getline(file, line)) {
    // read map
    if (std::regex_match(line, results, r_map)) {
      plan->G = new Grid(results[1].str());  // deleted in destructor of MAPFPlan
      continue;
    }
    // set agent num
    if (std::regex_match(line, results, r_agents)) {
      plan->num_agents = std::stoi(results[1].str());
      continue;
    }
    // solver
    if (std::regex_match(line, results, r_solver)) {
      plan->solver = results[1].str();
      continue;
    }
    // solved?
    if (std::regex_match(line, results, r_solved)) {
      plan->solved = (bool)std::stoi(results[1].str());
      continue;
    }
    // soc
    if (std::regex_match(line, results, r_soc)) {
      plan->soc = std::stoi(results[1].str());
      continue;
    }
    // makespan
    if (std::regex_match(line, results, r_makespan)) {
      plan->makespan = std::stoi(results[1].str());
      continue;
    }
    // comp_time
    if (std::regex_match(line, results, r_comp_time)) {
      plan->comp_time = std::stoi(results[1].str());
      continue;
    }
    // service time
    if (std::regex_match(line, results, r_service_time)) {
      plan->service_time = std::stof(results[1].str());
      continue;
    }
    // starts
    if (std::regex_match(line, results, r_starts)) {
      readSetNode(results[1].str(), plan->config_s, plan->G);
      continue;
    }
    // goals
    if (std::regex_match(line, results, r_goals)) {
      readSetNode(results[1].str(), plan->config_g, plan->G);
      continue;
    }
    // solution
    if (std::regex_match(line, results, r_sol)) {
      while (getline(file, line)) {
        if (std::regex_match(line, results, r_config)) {
          Config c, targets;
          std::vector<bool> assigned;
          readSetNode(results[1].str(), c, plan->G, targets, assigned);
          plan->transitions.push_back(c);
          if (!targets.empty()) plan->targets.push_back(targets);
          if (!assigned.empty()) plan->assigned.push_back(assigned);
        }
      }
    }
  }
}
