#pragma once

#include <nlohmann/json.hpp>
#include <fstream>
#include <filesystem>
#include <string>
#include <thread>
#include <atomic>
#include <future>
#include <memory>

namespace Game{

	class ConfigHandler{
	public:

		using json = nlohmann::json;

		ConfigHandler(std::atomic<int>& team_size, std::atomic_flag& game_over);

		void MotitorTheConfig(std::filesystem::path config_file);

	private:
		std::atomic<int>& team_size_;
		std::atomic_flag& game_over_;
	};


	class Player{
	public:
		Player(int num, int cur_team, std::atomic<int>& ball,
				std::atomic<int>& team_size, std::atomic<int>& striker);
		
		void PingPong();
		
		void to_terminate();
			
	private:
		int num_;
		int team_;
		std::string action_;
		std::atomic<int>& ball_;
		std::atomic<int>& team_size_;
		std::atomic<int>& striker_;
	
		std::atomic_flag terminated_;
	};

	class Team{
	public:
		Team(int num, std::atomic<int>& ball);
	
		void ChangePlayersNumber(int size);
			
	private:
		int team_num_;
	
		std::atomic<int>& ball_;
		std::atomic<int> team_size_{0};
		std::atomic<int> striker_{0};
	
		std::vector<std::shared_ptr<Player>> players_;
		std::vector<std::jthread> threads_;
	
	};

	class PingPongHandler{
	public:
		
		PingPongHandler();
		
		void start(std::string config_file);
		
	private:
		std::atomic<int> team_size_{0};
		std::atomic_flag game_over_;
	
		std::atomic<int> ball_{1};
		
		std::shared_ptr<ConfigHandler> config_handler_;

	};

}

