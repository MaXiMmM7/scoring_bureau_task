#include "PingPong.h"
#include <iostream>
#include <utility>
#include <exception>

namespace Game{
	
	ConfigHandler::ConfigHandler(std::atomic<int>& team_size, std::atomic_flag& game_over):
				  team_size_(team_size), game_over_(game_over) {}
	
	void ConfigHandler::MotitorTheConfig(std::filesystem::path config_file){
		
		json data;
		bool val = false;
		int old_size = 0;
		int new_size = 0;
		
		while(true){
			
			try{
				std::ifstream f(config_file);
				data = json::parse(f);
				f.close();
				new_size = data.at("team_size");
				val = data.at("game_over");

				if(val){
					game_over_.test_and_set();
					return;
				}
				if(new_size != old_size){
					old_size = new_size;
					if(new_size >= 0){
						team_size_.store(new_size, std::memory_order_release);
					}
				}
			}catch(std::exception& ex){
				std::cerr << ex.what() << std::endl;
			}
			catch(...){}
			
		}
	}
	

	Player::Player(int num, int cur_team, std::atomic<int>& ball, std::atomic<int>& team_size, std::atomic<int>& striker):
				num_(num), team_(cur_team), action_(team_ == 1 ? "ping" : "pong"),
				ball_(ball), team_size_(team_size), striker_(striker) {}
	
	void Player::PingPong(){

		while(true){

			while(!((striker_.load(std::memory_order_acquire) == num_) && (ball_.load(std::memory_order_acquire) == team_))){
				std::this_thread::yield();
				if(terminated_.test()){
					striker_.compare_exchange_strong(num_,0, std::memory_order_release, std::memory_order_relaxed);
					return;
				}
			}

			int team_size = team_size_.load(std::memory_order_acquire);
			if(num_ < team_size){
				std::cout << (num_+1) << "/" << team_size << ", " << team_ << ": " << action_ << std::endl;
				ball_.store(team_ == 1 ? 2 : 1, std::memory_order_release);
			}

			striker_.store(num_+1 < team_size ? num_+1 : 0, std::memory_order_release);

			if(terminated_.test()){
				return;
			}

		}
		
	}
	
	void Player::to_terminate(){
		terminated_.test_and_set();
	}
	

	Team::Team(int num, std::atomic<int>& ball): team_num_(num), ball_(ball){}

	void Team::ChangePlayersNumber(int size){

		int old_size = team_size_.load();
		team_size_.store(size, std::memory_order_release);

		if(size > old_size){
			for(int i = old_size; i < size; ++i){
				std::shared_ptr<Player> ptr = std::make_shared<Player>(i, team_num_, ball_, team_size_, striker_);
				threads_.emplace_back(&Player::PingPong, ptr);
				players_.push_back(std::move(ptr));
			}
		}else{

			for(int i = old_size-1; i >= size; --i){
				players_[i]->to_terminate();
				threads_.pop_back();
				players_.pop_back();
			}
		}
	}
		
		
	PingPongHandler::PingPongHandler(): config_handler_(std::make_shared<ConfigHandler>(team_size_, game_over_)){}
		
	void PingPongHandler::start(std::string config_file){
			std::filesystem::path config_path(std::move(config_file));
			if(!std::filesystem::exists(config_path)){
				std::cerr << "Configuration file doesn't exist." << std::endl;
				return;
			}

			std::jthread t_config(&ConfigHandler::MotitorTheConfig, config_handler_, std::move(config_path));
			
			int old_size = 0;
			int size = 0;
			Team team1(1, ball_);
			Team team2(2, ball_);
			while(!game_over_.test()){

				if((size = team_size_.load(std::memory_order_acquire)) != old_size){
					team1.ChangePlayersNumber(size);
					team2.ChangePlayersNumber(size);
					old_size = size;
				}
			}
			team1.ChangePlayersNumber(0);
			team2.ChangePlayersNumber(0);
		}
		
}
