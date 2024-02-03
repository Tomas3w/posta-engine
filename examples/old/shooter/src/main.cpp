#include "Player.h"
#include "Meat.h"
#include "WeaponItem.h"
#include <engine/include/Util/ConfigFile.h>
#include "Mapa.h"
#include "Text.h"
#include <engine/include/Util/TCPConnToServer.h>
#include <engine/include/Util/TCPConnToClient.h>
#include <engine/include/Util/TCPConnOfServer.h>
#include "PackagesTypes.h"
#include <engine/include/App.h>
#include <engine/include/Util/General.h>
#include <engine/include/Util/Assets.h>
#include <engine/include/Component/StaticMesh.h>
#include <engine/include/Util/Shader.h>
#include <engine/include/Component/Transform.h>
#include <engine/include/Entity/Camera.h>
#include <engine/include/Component/Texture.h>
#include "engine/include/Component/SphereRigidbody.h"
#include "engine/include/Component/PlaneRigidbody.h"
#include "engine/include/Component/BoxRigidbody.h"
#include "engine/include/Component/CapsuleRigidbody.h"
#include "engine/include/Component/MeshStaticRigidbody.h"
#include "engine/include/Component/Skeleton.h"
#include <engine/include/Util/LoggingMacro.h>
#include <engine/include/UI/Button.h>
#include <engine/include/UI/Checkbox.h>
#include <engine/include/UI/Textbox.h>
#include <engine/include/LuaAPI/LuaState.h>
#include <engine/include/LuaAPI/LuaAPI.h>
#include <fstream>
#include <memory>
#include <bitset>

using namespace std;

vector<string> test_player_names = {"Jhon", "Carlos", "Roberto", "Esteban", "Steve", "Gil", "Guillermo", "Rex", "Maracon", "Malloc", "Free"};

enum Scene
{
	MENU,
	SERVER_WAITING,
	CLIENT_WAITING,
	SERVER,
	CLIENT,
};

class App : public Engine::App
{
	public:
		Engine::ThreeDShader& shader;

		Engine::Component::StaticMesh static_mesh_cube;
		Engine::Component::StaticMesh static_mesh_capsule;
		Engine::Component::StaticMesh static_mesh_sphere;
		Engine::Component::Texture texture;
		Engine::Component::Texture blank_texture;

		vector<unique_ptr<Engine::Component::SphereRigidbody>> spheres;
		vector<unique_ptr<Engine::Component::BoxRigidbody>> boxes;
		vector<unique_ptr<Engine::Component::CapsuleRigidbody>> capsules;

		vector<unique_ptr<WeaponItem>> weapons_items;

		unordered_map<string, unique_ptr<Player>> players;
		string controlling; // player being controlled;
		vector<unique_ptr<Meat>> meats; // from the players that have died


		Scene scene;

		Mapa mapa;

		// Default font
		Engine::Font font;

		// Messages and networks connections
		vector<unique_ptr<Engine::TCPConnToClient>> connections;
		vector<PlayerConnectionData> players_connection_data;
		PlayerConnectionData own_server_connection_data;
		vector<string> connections_names;
		vector<PlayerMovData> player_move_data_prevs;
		size_t number_of_connections_with_names;
		Text center_message;
		string nickname;
		// Server
		int count_of_players;
		unique_ptr<Engine::TCPConnOfServer> server_con;
		// client
		PlayerMovData player_move_data; // this is sent to the server
		unique_ptr<Engine::TCPConnToServer> client_con;
		IPaddress address;

		// Test
		Engine::Component::Skeleton skeleton_test;
		Engine::Component::StaticMesh static_mesh_test;
		Engine::Component::Skeleton::AnimationPlayer anim_player;
		Engine::Component::StaticMesh static_mesh_test_cargador;
		Engine::Component::Texture test_cargador_texture;

		Engine::Component::Image button_image;
		Engine::UI::Button button;
		Engine::Component::Image checkbox_off;
		Engine::Component::Image checkbox_on;
		Engine::UI::Checkbox checkbox;
		Engine::UI::CheckboxGroup checkbox_group;

		Engine::UI::Textbox textbox;
		Text text_of_textbox;
		Engine::Component::Image textbox_image;
		Engine::Component::Image textbox_bar;

		App() :
			shader(*shader3d),
			static_mesh_cube(Engine::Assets::load_obj("assets/cube3.obj")),
			static_mesh_capsule(Engine::Assets::load_obj("assets/capsule.obj")),
			static_mesh_sphere(Engine::Assets::load_obj("assets/sphere.obj")),
			
			texture("assets/texture.png"),
			blank_texture("common/blank.png"),
			mapa("assets/maps/0"),
			font("common/monogram.ttf"),
			//font("common/jfont.otf"),
			//font("common/OxygenMono-Regular.ttf"),
			center_message("Waiting for player...", font, 32),

			skeleton_test("assets/test"),
			static_mesh_test(Engine::Assets::load_obj_with_bones("assets/test/test")),
			static_mesh_test_cargador(Engine::Assets::load_obj("assets/test/cargador.obj")),
			test_cargador_texture("assets/test/cargador.png"),

			button_image("assets/button.png"),
			button(Engine::UI::Rect(0, 0, button_image.w, button_image.h)),
			checkbox_off("assets/checkbox.png"),
			checkbox_on("assets/checkbox2.png"),
			checkbox(Engine::UI::Rect(0, 0, 0, 0)),
			textbox(Engine::UI::Rect(0, 200, 300, 50), &font, 50),
			text_of_textbox(" ", font, 32),
			textbox_image("assets/textbox.png"),
			textbox_bar("assets/textbox_bar.png")
		{
			LOG("Running lua script:");
			{
				lua_state->run_file("assets/scripts/init.lua");
			}

			textbox.add_text("hola!");
			LOG("Construyendo app");
			// test checkbox group
			for (size_t i = 0; i < 5; i++)
				checkbox_group.checkboxes.emplace_back(Engine::UI::Rect(0, 0, checkbox_off.w, checkbox_off.h));
			// test skeleton
			skeleton_test.add_animation("assets/test/anim1.anm");
			skeleton_test.add_animation("assets/test/anim2.anm");
			anim_player.time = 0;
			anim_player.animations.push_back({"anim2", 0.8f});
			anim_player.animations.push_back({"anim1", 0.2f});

			// camera
			camera->transform.set_position(glm::vec3(0, 2, 3));

			// Default color
			shader.global_color = {1, 1, 1, 1};

			// Gravity
			physics->world->setGravity(Engine::to_btVector3(glm::vec3(0, -20, 0)));

			// Default player
			controlling = -1;

			// Weapons
			mapa.fill_items(weapons_items);

			// Deactivating the bones in the shader
			shader.use_bones = false;

			// Setting program side
			Engine::ConfigFile config("assets/conexion.txt");
			number_of_connections_with_names = 0;
			nickname = config["nickname"];
			if (config["cliente"] == "no")
			{
				count_of_players = stoi(config["players"]);
				scene = SERVER_WAITING;
				server_con.reset(new Engine::TCPConnOfServer(stoi(config["puerto"])));
				LOG("SERVER");
				scene = SERVER; // DEBUG
				controlling = "s"; // DEBUG
			}
			else
			{
				scene = CLIENT_WAITING;
				address.port = stoi(config["puerto"]);
				address.host = Engine::string_to_ip(config["ip"]);
				LOG("ip read: ", Engine::ip_to_string(address.host));
				LOG("CLIENTE");
			}
			/*/ Testing
			// a.position vec3(5.000000, 5.000000, 9.000000) a.rotation quat(0.940881, {0.000000, 0.000000, 0.338738}) a.scale vec3(1.000000, 1.000000, 1.000000)
			// b.position vec3(8.000000, 1.000000, 9.000000) b.rotation quat(0.637424, {0.000000, 0.770513, 0.000000}) b.scale vec3(1.000000, 1.000000, 1.000000)
			//Engine::Component::Transform a({5, 5, 9});
			//a.set_rotation(glm::quat(0.940881, 0.000000, 0.000000, 0.338738));
			//Engine::Component::Transform b({8, 1, 9});
			//b.set_rotation(glm::quat(0.637424, 0.000000, 0.770513, 0.000000));
			Engine::Component::Transform a({rand() % 10, rand() % 10, rand() % 10});
			a.set_rotation(glm::rotate(a.get_rotation(), static_cast<float>(M_PI * (rand() % 100 / 100.0f)), glm::vec3(0, 0, 1)));
			a.set_rotation(glm::rotate(a.get_rotation(), static_cast<float>(M_PI * (rand() % 100 / 100.0f)), glm::vec3(0, 1, 0)));
			a.set_rotation(glm::rotate(a.get_rotation(), static_cast<float>(M_PI * (rand() % 100 / 100.0f)), glm::vec3(1, 0, 0)));
			Engine::Component::Transform b({rand() % 10, rand() % 10, rand() % 10});
			b.set_rotation(glm::rotate(b.get_rotation(), static_cast<float>(M_PI * (rand() % 100 / 100.0f)), glm::vec3(0, 1, 0)));
			b.set_rotation(glm::rotate(b.get_rotation(), static_cast<float>(M_PI * (rand() % 100 / 100.0f)), glm::vec3(0, 0, 1)));
			b.set_rotation(glm::rotate(b.get_rotation(), static_cast<float>(M_PI * (rand() % 100 / 100.0f)), glm::vec3(1, 0, 0)));
			Engine::Component::Transform c = b.as_local_to(a);
			Engine::Component::Transform d = a.as_origin_to(c);
			// d should be equal to b
			PRINT("a.position " << to_string(a.get_position()) << " a.rotation " << to_string(a.get_rotation()) << " a.scale " << to_string(a.get_scale()));
			PRINT("b.position " << to_string(b.get_position()) << " b.rotation " << to_string(b.get_rotation()) << " b.scale " << to_string(b.get_scale()));
			PRINT("c.position " << to_string(c.get_position()) << " c.rotation " << to_string(c.get_rotation()) << " c.scale " << to_string(c.get_scale()));
			PRINT("d.position " << to_string(d.get_position()) << " d.rotation " << to_string(d.get_rotation()) << " d.scale " << to_string(d.get_scale()));
			exit_loop = true;*/
		}

		~App()
		{
			LOG("Destruyendo app");
		}

		void on_frame() override
		{
			// Keyboard mapping
			for (pair<size_t, string> e : unordered_map<size_t, string>({{SDL_SCANCODE_D, "right"}, {SDL_SCANCODE_A, "left"}, {SDL_SCANCODE_W, "forward"}, {SDL_SCANCODE_S, "backward"}, {SDL_SCANCODE_LCTRL, "ctrl"}, {SDL_SCANCODE_SPACE, "space"}}))
			{
				if (keyboard[e.first]) input[e.second] = 1;
				else input[e.second] = 0;
			}

			// Camera
			if (!players.count(controlling))
			{
				glm::vec3 front = camera->transform.front();
				if (input["right"] + input["left"] + input["forward"] + input["backward"] + input["ctrl"] + input["space"])
				{
					front = glm::normalize(glm::vec3(front.x, 0, front.z));
					glm::vec3 right = camera->transform.right();

					glm::vec3 move = right * (input["right"] - input["left"]) + front * (input["backward"] - input["forward"]);
					camera->transform.set_position(camera->transform.get_position() + (move + glm::vec3(0, -1, 0) * input["ctrl"] + glm::vec3(0, 1, 0) * input["space"]) * 0.5f);
				}
				front = camera->transform.front();
			}
			else
			{
				Engine::Component::Transform& transform = players[controlling]->transform;
				camera->transform.set_position(transform.get_position());
				camera->transform.set_rotation(transform.get_rotation());
			}

			switch (scene)
			{
			case MENU:
				break;
			case CLIENT_WAITING:
				camera->transform = Engine::Component::Transform();
				camera->transform.set_position({0, 1, 0});
				//cout << "hola!" << endl;
				if (!client_con)
				{
					center_message.reload("Waiting to connect to server...", font);
					Engine::TCPConnToServer* con = Engine::TCPConnToServer::connect_to(address);
					if (con)
					{
						LOG("Connection to server done!");
						client_con.reset(con);
						scene = CLIENT;

						StringPackage package(nickname);
						client_con->send(package);
						
						// Setting controlling to actual character being controlled
						controlling = nickname;
					}
				}
				//cout << "parte pasada!" << endl;
				break;
			case CLIENT:
				{
					// Player input
					if (players.count(controlling))
					{
						// movement
						bool move_flags[4] = {
							(app->keyboard[SDL_SCANCODE_W] ? true:false),
							(app->keyboard[SDL_SCANCODE_S] ? true:false),
							(app->keyboard[SDL_SCANCODE_D] ? true:false),
							(app->keyboard[SDL_SCANCODE_A] ? true:false),
						};
						player_move_data.set_movement(move_flags);
					}
					// Sending data to the server
					//PRINT("(C)mov data sent: " << std::bitset<8>(player_move_data.action_flags) << ", and x " << player_move_data.mov_x << " y " << player_move_data.mov_y);
					client_con->send(player_move_data);
					player_move_data.mov_x = player_move_data.mov_y = 0;
					// Getting data from the server
					uint32_t data_type;
					array<uint8_t, 512> data;
					//auto start = std::chrono::system_clock::now();
					while (client_con->recv(data_type, Engine::make_span(data)))
					{
						switch (static_cast<NetworkPackageType>(data_type))
						{
							case NetworkPackageType::SIMPLE_STRING:
								{
									StringPackage str;
									str.deserialize(Engine::make_span(data));
									LOG("got ", str.str);
								}
								break;
							case NetworkPackageType::PLAYERS_VISUAL_DATA:
								{
									PlayersVisualData pvd;
									pvd.deserialize(Engine::make_span(data));
									for (auto& v : pvd.visual_data)
									{
										if (!players.count(v.nickname))
										{
											players[v.nickname] = make_unique<Player>(v.transform.get_position(), v.nickname);
											players[v.nickname]->make_all_guns_available();
										}
										players[v.nickname]->body_transform = v.transform;
										players[v.nickname]->transform = v.transform;
										//PRINT("[" << v.nickname << "]" << to_string(v.transform.get_position()));
										players[v.nickname]->set_weapon_selected(v.weapon_selected);
										players[v.nickname]->set_life(std::max(v.life, 0.01f));
										players[v.nickname]->set_recharge_time(v.recharge_time);
										players[v.nickname]->set_ammunition(v.charged_ammunition, v.reserved_ammunition);
										players[v.nickname]->loop();
										players[v.nickname]->set_gun_offset(v.gun_offset);
									}
								}
								//PRINT("visual data received");
								break;
							case NetworkPackageType::PLAYER_DIED_DATA:
								{
									PlayerDiedData str;
									str.deserialize(Engine::make_span(data));
									if (players.count(str.str))
										players[str.str]->reduce_life(101);
								}
								break;
							case NetworkPackageType::PLAYER_WEAPON_0_SOUND:
								{
									PlayerWeapon0Sound str;
									str.deserialize(Engine::make_span(data));
									if (players.count(str.str))
									{
										players[str.str]->play_weapon_firing_sound();
									}
								}
								break;
							case NetworkPackageType::WEAPON_ITEM_CHANGE:
								{
									WeaponItemChangePackage wicp;
									wicp.deserialize(Engine::make_span(data));
									for (uint32_t& e : wicp.vec)
									{
										if ((1ul << 31) & e)
											mapa.revive_weapon_item(weapons_items, e & ~(1ul << 31));
										else
											mapa.remove_weapon_item(weapons_items, e);
									}
								}
								break;
							default:
								LOG("warning: strange package type was received in the client");
								break;
						}
					}
					//auto end = std::chrono::system_clock::now();
					//auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
					//PRINT(elapsed.count());
					mapa.loop_for_client(weapons_items);
				}
				break;
			case SERVER_WAITING:
				//camera->transform = Engine::Component::Transform();
				//camera->transform.set_position({0, 1, 0});
				center_message.reload("Waiting for players... (" + to_string(connections.size()) + "/" + to_string(count_of_players) + ")", font, 32);
				if (connections.size() < static_cast<size_t>(count_of_players))
				{
					Engine::TCPConnToClient* con = server_con->accept_incoming_connection();
					if (con)
					{
						LOG("Accepted connection successfully!");
						connections.push_back(std::unique_ptr<Engine::TCPConnToClient>(con));
					}
					if (connections.size() == static_cast<size_t>(count_of_players))
					{
						scene = SERVER;
						connections_names.resize(connections.size());
						player_move_data_prevs.resize(connections.size());
						players_connection_data.resize(connections.size());
						controlling = nickname;
					}
				}
				break;
			case SERVER:
				if (number_of_connections_with_names == connections.size())
				{
					/// Sending visual data to all connections
					PlayersVisualData pvd(players);
					for (size_t i = 0; i < connections.size(); i++)
						connections[i]->send(pvd);
					/// Receiving data from connections
					for (size_t i = 0; i < connections.size(); i++)
					{
						uint32_t data_type;
						std::array<uint8_t, 512> data;
						
						while (connections[i]->recv(data_type, Engine::make_span(data)))
						{
							if (static_cast<NetworkPackageType>(data_type) == NetworkPackageType::PLAYER_MOV_DATA)
							{
								if (players.count(connections_names[i]))
								{
									player_move_data_prevs[i].deserialize(Engine::make_span(data));
									player_move_data_prevs[i].apply(*players[connections_names[i]]);
								}
							}
						}
					}
				}
				else
				{
					/// Getting connections nicknames
					for (size_t i = 0; i < connections.size(); i++)
					{
						auto& con = connections[i];
						uint32_t data_type;
						array<uint8_t, 512> data;
						if (con->recv(data_type, Engine::make_span(data)))
						{
							StringPackage str;
							str.deserialize(Engine::make_span(data));
							connections_names[i] = str.str;
							LOG("got ", str.str);
							number_of_connections_with_names++;
						}
					}
					if (number_of_connections_with_names == connections.size())
					{
						vector<string> names = connections_names;
						names.push_back(nickname);
						mapa.fill_players(names, players);
					}
				}

				// Player input
				if (players.count(controlling))
				{
					Player& player = *players[controlling].get();
					// view
					float y = (app->keyboard[SDL_SCANCODE_LEFT] ? -1:0) + (app->keyboard[SDL_SCANCODE_RIGHT] ? 1:0);
					float s = -M_PI / 1000.0f * 10;
					float x = (app->keyboard[SDL_SCANCODE_UP] ? -1:0) + (app->keyboard[SDL_SCANCODE_DOWN] ? 1:0);
					player.rotate_view(x * s, y * s);
					// movement
					bool move_flags[4] = {
						(app->keyboard[SDL_SCANCODE_W] ? true:false),
						(app->keyboard[SDL_SCANCODE_S] ? true:false),
						(app->keyboard[SDL_SCANCODE_D] ? true:false),
						(app->keyboard[SDL_SCANCODE_A] ? true:false),
					};
					player.set_movement(move_flags);
				}
				// Server side players
				for (auto it = players.begin(), ft = players.end(); it != ft; it++)
				{
					auto& player = it->second;
					player->loop();
					if (player->should_fire())
					{
						player->play_weapon_firing_sound();
						PlayerWeapon0Sound pw0s(player->nickname);
						for (size_t i = 0; i < connections.size(); i++)
							connections[i]->send(pw0s);
						btVector3 origin = Engine::to_btVector3(player->transform.get_position());
						btVector3 destination = Engine::to_btVector3(player->transform.get_position() - player->transform.front() * 100.0f);
						btCollisionWorld::ClosestRayResultCallback callback(origin, destination);
						physics->world->rayTest(origin, destination, callback);
						if (callback.hasHit())
						{
							for (auto& other : players)
							{
								if (other.second.get() != player.get() && callback.m_collisionObject == other.second->body.get_body())
								{
									other.second->reduce_life(22.0f);
									other.second->stunt();
									if (!other.second->is_alive())
									{
										size_t killer;
										size_t killed;
										for (size_t i = 0; i < connections_names.size(); i++)
										{
											if (player->nickname == connections_names[i])
												killer = i;
										}
										for (size_t i = 0; i < connections_names.size(); i++)
										{
											if (other.second->nickname == connections_names[i])
												killed = i;
										}
										if (player->nickname == nickname)
										{
											players_connection_data[killed].respawn_time = 10;
											own_server_connection_data.kill_count++;
										}
										else if (other.second->nickname == nickname)
										{
											own_server_connection_data.respawn_time = 10;
											players_connection_data[killer].kill_count++;
										}
										else
										{
											players_connection_data[killed].respawn_time = 10;
											players_connection_data[killer].kill_count++;
										}
									}
								}
							}
						}
					}
				}
				if (own_server_connection_data.respawn_time < 0)
					mapa.spawn_player(nickname, players);
				else
					own_server_connection_data.respawn_time -= delta_time;
				for (size_t i = 0; i < players_connection_data.size(); i++)
				{
					if (players_connection_data[i].respawn_time < 0)
						mapa.spawn_player(connections_names[i], players);
					else
						players_connection_data[i].respawn_time -= delta_time;
				}
				// Map and items
				WeaponItemChangePackage wicp = mapa.loop(weapons_items, players);
				// sendind messages to clients about missing weapon_items and new ones
				if (!wicp.vec.empty())
				{
					for (auto& con : connections)
						con->send(wicp);
				}
				break;
			}

			vector<string> players_to_delete;
			for (auto it = players.begin(), ft = players.end(); it != ft; it++)
			{
				auto& player = it->second;
				if (!player->is_alive())
				{
					for (int i = 0; i < 5; i++)
						meats.push_back(make_unique<Meat>(player->body_transform.get_position(), player->velocity));
					players_to_delete.push_back(it->first);
				}
			}
			for (auto& player : players_to_delete)
			{
				if (scene == SERVER)
				{
					for (auto& con : connections)
					{
						PlayerDiedData pdd(player);
						con->send(pdd);
					}
				}
				players.erase(player);
			}

			// Meat
			for (auto it = meats.begin(), ft = meats.end(); it != ft; it++)
			{
				auto& meat = (*it);
				meat->loop();
				if (!meat->is_alive())
				{
					it = meats.erase(it) - 1;
					ft = meats.end();
				}
			}

			// Button test
			if (button.loop(0, 0))
			{
				std::optional<size_t> checkbox_selected = checkbox_group.get_checked_index();
				if (checkbox_selected)
				{
					LOG("checkbox will loose: ", *checkbox_selected);
				}
				else
				{
					LOG("checkbox group has nothing");
				}
			}
			// Checkbox
			checkbox.loop(0, 0);
			// Checkbox group
			checkbox_group.loop(0, 0);
			//*
			for (size_t i = 0; i < checkbox_group.checkboxes.size(); i++)
			{
				checkbox_group.checkboxes[i].get_rect().x = i * checkbox_off.w;
				//LOG(checkbox_group.checkboxes[i].button.x);
				checkbox_group.checkboxes[i].get_rect().y = get_height() - checkbox_off.h;
			}//*/
			// Textbox
			textbox.loop(0, 0);
		}

		int bone_to_use_to_use = 0;
		void on_draw() override
		{
			shader.bind();
			// Bind camera matrix
			shader.projection_view = camera->get_projection_view_matrix();
			shader.global_color = {1, 1, 1, 1};

			// Spheres
			blank_texture.bind();
			for (auto& sphere : spheres)
			{
				glm::mat4 matrix = sphere->get_matrix();

				shader.model = matrix;
				shader.normal_model = glm::mat3(glm::transpose(glm::inverse(matrix)));
				static_mesh_sphere.draw();
			}
			// Capsules
			for (auto& capsule : capsules)
			{
				glm::mat4 matrix = capsule->get_matrix();

				shader.model = matrix;
				shader.normal_model = glm::mat3(glm::transpose(glm::inverse(matrix)));
				static_mesh_capsule.draw();
			}
			// Boxes
			texture.bind();
			for (auto& box : boxes)
			{
				glm::mat4 matrix = box->get_matrix();

				shader.model = matrix;
				shader.normal_model = glm::mat3(glm::transpose(glm::inverse(matrix)));
				static_mesh_cube.draw();
			}
			// Map
			mapa.draw(shader);

			// Players
			for (auto& player : players)
			{
				blank_texture.bind();
				if (player.first != controlling)
					player.second->draw(shader);
			}

			// Meats of players
			for (auto& meat : meats)
				meat->draw(shader);

			// Player fpv
			if (players.count(controlling) && false)
			{
				Weapon* weapon = players[controlling]->get_weapon_selected();
				if (weapon != nullptr)
				{
					Engine::Component::Transform transform;
					//cout << to_string(players[controlling]->get_gun_offset()) << endl;
 					transform.set_position(camera->transform.get_position() + camera->transform.to_local(glm::vec3(0.1f, -0.1f, -0.3f) + players[controlling]->get_gun_offset()));
					if (players[controlling]->get_recharge_time() > 0)
						transform.set_rotation(glm::rotate(camera->transform.get_rotation(), static_cast<float>(M_PI * 2 * (players[controlling]->get_recharge_time() / weapon->charge_time)), glm::vec3(0, 0, 1)));
					else
						transform.set_rotation(camera->transform.get_rotation());
					shader.model = transform.get_matrix();
					shader.normal_model = transform.get_normal_matrix();
					weapon->draw();
				}
			}
			// Items
			if (scene != SERVER_WAITING && scene != CLIENT_WAITING)
			{
				for (auto& item : weapons_items)
				{
					if (item)
						item->draw(shader);
				}
			}//*/

			// Test
			if (players.count(controlling))
			{
				Weapon* weapon = players[controlling]->get_weapon_selected();
				if (weapon != nullptr)
				{
					float tt = SDL_GetTicks() / 1000.0f;
					anim_player.animations[0].weight = abs(sin(tt));
					anim_player.animations[1].weight = abs(cos(tt));

					auto camera_transform = camera->transform;
					camera_transform.set_rotation(glm::rotate(camera_transform.get_rotation(), float(M_PI), glm::vec3(0, 1, 0)));
					//glm::inverse(camera_transform.get_rotation()));
					camera_transform = Engine::Component::Transform({0, -0.265469, 0}).as_local_to(camera_transform);
					//camera_transform = Engine::Component::Transform();

					auto matrices_of_bones = skeleton_test.get_animation_matrices(anim_player, false);
					shader.bones_matrices = matrices_of_bones;
					if (players[controlling]->get_recharge_time() > 0)
						anim_player.time = (1.0f - players[controlling]->get_recharge_time() / weapon->charge_time) * skeleton_test.get_animation_duration("anim2");//bone_to_use_to_use / float(24);
					else
						anim_player.time = 0.0f;
					

					Engine::Component::Transform tr({0, 0, 0});
					tr = tr.as_local_to(camera_transform);
					shader.model = tr.get_matrix();
					shader.normal_model = tr.get_normal_matrix();
					
					/*
					if (bone_to_use_to_use > matrices_of_bones.size())
					{
						PRINT("reach last of bone_to_use_to_use");
						bone_to_use_to_use = 0;
					}
					shader.model = matrices_of_bones[bone_to_use_to_use];
					shader.normal_model = glm::mat3(glm::transpose(glm::inverse(matrices_of_bones[bone_to_use_to_use])));*/

					shader.global_color = {1, 1, 1, 1};
					blank_texture.bind();

					shader.use_bones = true;
					static_mesh_test.draw();
					shader.use_bones = false;

					// drawing weapon
					auto* bone = Engine::Component::Skeleton::Bone::search_in_bones("mano.L", skeleton_test.root_bones);
					if (bone == nullptr)
						throw std::logic_error("couldn't find bone: 'mano.L'");
					tr = skeleton_test.bone_transforms[bone->index];
					Engine::Component::Transform _tr;
					_tr.set_scale({1, 1, 1.616});
					_tr.set_position(glm::vec3(0.009727999567985535, -0.05475499853491783, -0.003959000110626221));
					_tr.set_position(_tr.get_position() + glm::vec3(0, players[controlling]->get_gun_offset().z / 5.0f, 0));
					_tr.set_rotation(glm::quat(-0.534125804901123, -0.539588451385498, -0.4753764271736145, 0.44448980689048767));
					auto final_tr = _tr.as_local_to(tr).as_local_to(camera_transform);
					shader.model = final_tr.get_matrix();
					shader.normal_model = final_tr.get_normal_matrix();
					Weapon::get_weapon(0)->draw();
					// drawing weapon cargador
					bone = Engine::Component::Skeleton::Bone::search_in_bones("cargador", skeleton_test.root_bones);
					if (bone == nullptr)
						throw std::logic_error("couldn't find bone: 'cargador'");
					tr = skeleton_test.bone_transforms[bone->index];
					_tr = Engine::Component::Transform();
					_tr.set_position(glm::vec3(0.009727999567985535, -0.05475499853491783, -0.003959000110626221));
					_tr.set_rotation(glm::rotate(_tr.get_rotation(), float(M_PI / 2), glm::vec3(1, 0, 0)));
					final_tr = _tr.as_local_to(tr).as_local_to(camera_transform);
					shader.model = final_tr.get_matrix();
					shader.normal_model = final_tr.get_normal_matrix();
					test_cargador_texture.bind();
					static_mesh_test_cargador.bind();
					static_mesh_test_cargador.draw();
				}
			}
			//*/

		}

		void on_draw_2d() override
		{
			// Test script
			{
				//LOG("Running lua script (loop):");
				lua_state->run_file("assets/scripts/draw.lua");
				//LOG("Stopped lua script (loop)")
			}
			// Button test
			button.get_rect().w = button_image.w;
			button.get_rect().h = button_image.h;
			button.get_rect().x = get_width() / 2 - button.get_rect().w / 2;
			button.get_rect().y = get_height() / 2 - button.get_rect().h / 2;
			glm::vec4 __color(0.77f, 0.77f, 0.77f, 1);
			if (button.is_highlighted())
				__color = glm::vec4(1, 1, 1, 1);
			if (button.is_pressed())
				__color = glm::vec4(0.2f, 0.2f, 0.2f, 1);
			shader2d->global_color = __color;
			shader2d->model = button_image.get_matrix(button.get_rect().x, button.get_rect().y);
			button_image.draw();
			// Checkbox test
			checkbox.get_rect().w = checkbox_off.w;
			checkbox.get_rect().h = checkbox_off.h;
			checkbox.get_rect().x = get_width() / 2 + checkbox.get_rect().w / 2;
			checkbox.get_rect().y = get_height() / 2 - checkbox.get_rect().h / 2;
			__color = glm::vec4(0.77f, 0.77f, 0.77f, 1);
			if (checkbox.is_highlighted())
				__color = glm::vec4(1, 1, 1, 1);
			shader2d->global_color = __color;
			shader2d->model = checkbox_off.get_matrix(checkbox.get_rect().x, checkbox.get_rect().y);
			if (checkbox.is_checked())
				checkbox_on.draw();
			else
				checkbox_off.draw();
			// Checkbox group
			//*
			for (auto& item : checkbox_group.checkboxes)
			{
				__color = glm::vec4(0.77f, 0.77f, 0.77f, 1);
				if (item.is_highlighted())
					__color = glm::vec4(1, 1, 1, 1);
				shader2d->global_color = __color;
				shader2d->model = checkbox_off.get_matrix(item.get_rect().x, item.get_rect().y);
				if (item.is_checked())
					checkbox_on.draw();
				else
					checkbox_off.draw();
			}
			//*/
			// Textbox
			disable_depth_test();
			shader2d->global_color = glm::vec4(0.75f, 0.75f, 0.75f, 1);
			shader2d->model = textbox_image.get_matrix(textbox.get_rect().x, textbox.get_rect().y);
			textbox_image.draw();
			if (textbox.has_change)
			{
				if (textbox.get_text().empty())
					text_of_textbox.reload(" ", font, 32);
				else
					text_of_textbox.reload(textbox.get_text(), font, textbox.font_size);
				textbox.has_change = false;
			}

			shader2d->global_color = glm::vec4(1, 1, 1, 1);
			shader2d->model = textbox_bar.get_matrix(textbox.get_rect().x + textbox.get_cursor_offset(), textbox.get_rect().y);
			textbox_bar.draw();
			text_of_textbox.draw(*shader2d, textbox.get_rect().x, textbox.get_rect().y, glm::vec4(0, 0, 0, 1));
			// Text
			//center_message.reload(textbox.get_text(), font, 32);
			if (scene == SERVER_WAITING || scene == CLIENT_WAITING)
				center_message.draw(*shader2d, 0, 20, glm::vec4(0, 0, 0, 1));
		}

		void on_event(SDL_Event& event) override
		{
			//cout << "on event..." << endl;
			glm::vec3 vel = camera->transform.front() * -1800.0f;
			manage_textbox_input(event, textbox);
			switch (event.type)
			{
				//case SDL_TEXTINPUT:
				//	textbox.add_text(event.text.text);
				//	break;
				case SDL_KEYUP:
					switch (event.key.keysym.sym)
					{
						case SDLK_r:
							if (scene == CLIENT)
								player_move_data.stop_recharging();
							break;
					}
					break;
				case SDL_KEYDOWN:
					switch (event.key.keysym.sym)
					{
						/*case SDLK_RIGHT:
							textbox.next_cursor_pos();
							break;
						case SDLK_LEFT:
							textbox.prev_cursor_pos();
							break;

						case SDLK_DELETE:
							textbox.erase_next_character();
							break;
						case SDLK_BACKSPACE:
							textbox.erase_character();
							break;*/
						case SDLK_r:
							if (scene == CLIENT)
								player_move_data.recharge();
							else if (players.count(controlling))
								players[controlling]->recharge();
							break;
						#define NODEF
						#ifdef NODEF
						case SDLK_y:
							if (players.count(controlling))
							{
								Engine::Component::Transform transform;
								transform.set_position({14.145670, 1.499992, 0.462562});
								players[controlling]->body_transform = transform;
								glm::vec3 vel = {-11.157936, 0.000203, 6.247448};
								players[controlling]->velocity = vel; 
							}
							break;
						case SDLK_t:
							//std::cout << controlling << " " << to_string(camera->transform.get_position()) << std::endl;
							bone_to_use_to_use++;
							/*if (players.count(controlling))
							{
								short r, c;
								players[controlling]->get_ammunition(r, c);
								cout << "ammunition(" << r << ", " << c << ")" << endl;
							}*/
							break;
						//
						case SDLK_o:
							controlling = "";
							break;
						case SDLK_i:
							if (!players.empty())
							{
								controlling = players.begin()->first;
								for (auto& player : players)
								{
									if (rand() % 2)
									{
										controlling = player.first;
										break;
									}
								}
							}
							break;
						case SDLK_e:
							mapa.fill_players(test_player_names, players);
							break;
						case SDLK_q:
							for (string& name : test_player_names)
							{
								if (!players.count(name))
								{
									players[name] = make_unique<Player>(glm::vec3(0, 3, -2), name);
									break;
								}
							}
							break;
						case SDLK_x:
							spheres.push_back(unique_ptr<Engine::Component::SphereRigidbody>(new Engine::Component::SphereRigidbody(camera->transform.get_position(), 1.0f, 0.2f, vel)));
							break;
						case SDLK_c:
							boxes.push_back(unique_ptr<Engine::Component::BoxRigidbody>(new Engine::Component::BoxRigidbody(camera->transform.get_position(), 1.0f, {1, 1, 1}, vel)));
							break;
						case SDLK_v:
							capsules.push_back(unique_ptr<Engine::Component::CapsuleRigidbody>(new Engine::Component::CapsuleRigidbody(camera->transform.get_position(), 1.0f, 1.0f, 2.0f, vel)));
							break;
						//*/
						case SDLK_z:
							cout << "delta_time: " << delta_time << endl;
							break;
						#endif
					}
				case SDL_MOUSEBUTTONUP:
					switch (event.button.button)
					{
						case SDL_BUTTON_LEFT:
							if (scene == CLIENT)
								player_move_data.stop_shooting();
							else if (players.count(controlling))
								players[controlling]->stop_shooting();
							break;
					}
					break;
				case SDL_MOUSEBUTTONDOWN:
					switch (event.button.button)
					{
						case SDL_BUTTON_RIGHT:
							SDL_SetRelativeMouseMode(SDL_GetRelativeMouseMode() == SDL_TRUE ? SDL_FALSE:SDL_TRUE);
							break;
						case SDL_BUTTON_LEFT:
							if (scene == CLIENT)
								player_move_data.shoot();
							else if (players.count(controlling))
								players[controlling]->shoot();
							break;
					}
					break;
				case SDL_MOUSEMOTION:
					float xrel = event.motion.xrel, yrel = event.motion.yrel;
					float s = -M_PI / 1000.0f;
					if (SDL_GetRelativeMouseMode())
					{
						if (!players.count(controlling))
						{
							glm::quat rot = camera->transform.get_rotation();
							float m = (1 - (glm::dot(glm::vec3(0, -1, 0), rot * glm::vec3(0, 0, -1)) + 1) / 2.0f) * M_PI;

							glm::vec3 up = {0, 1, 0};
							glm::quat first_rotation = glm::rotate(rot, std::min(std::max(yrel * s, -m), static_cast<float>(M_PI - m)), glm::vec3(1, 0, 0));
							camera->transform.set_rotation(glm::rotate(first_rotation, xrel * s, glm::toMat3(glm::inverse(first_rotation)) * up));
						}
						else if (scene == CLIENT)
						{
							player_move_data.mov_x = xrel * s * 2;
							player_move_data.mov_y = yrel * s * 2;
						}
						else
							players[controlling]->rotate_view(yrel * s, xrel * s);
					}
					break;
			}
		}

};

std::unique_ptr<App> app = nullptr;

///
int main(int argc, char* argv[])
{
	app.reset(new App());
	app->loop();
	return EXIT_SUCCESS;
}
/*/

int _main(int argc, char* argv[])
{
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_Window* window = SDL_CreateWindow("H", 100, 100, 640, 480, SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);

	//initpart
	SDL_GLContext context = SDL_GL_CreateContext(window);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	glewExperimental = GL_TRUE;
	glewInit();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);

	//glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// Enable alpha blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Enable face culling
	//glEnable(GL_CULL_FACE);

	glViewport(0, 0, 640, 480);

	glClearColor(0, 0, 0, 1);

	/// meshpart
	float vertices[] = {
		-0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		 0.0f,  0.5f, 0.0f
	};
	GLuint vao, vbo;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	///

	Engine::Component::StaticMesh static_mesh(Engine::Assets::load_obj("assets/cube.obj"));

	Engine::Shader shader("assets/shaders/sbasic");
	shader.bind();
	GLint model_uniform = glGetUniformLocation(shader.get_program(), "model");
	cout << "model_uniform = " << model_uniform << endl;
	GLint projection_view_uniform = glGetUniformLocation(shader.get_program(), "projection_view");
	cout << "projection_view_uniform = " << projection_view_uniform << ", also... " << shader.get_program() << endl;
	
	SDL_Event event;
	bool salir = false;
	while (!salir)
	{
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		//glBindVertexArray(vao);
		//glDrawArrays(GL_TRIANGLES, 0, 3);

		//shader.bind();
		static_mesh.draw();
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				case SDL_QUIT:
					salir = true;
					break;
			}
		}
		SDL_GL_SwapWindow(window);
	}

	SDL_DestroyWindow(window);
	SDL_GL_DeleteContext(context);
	SDL_Quit();
	return EXIT_SUCCESS;
}
*/
