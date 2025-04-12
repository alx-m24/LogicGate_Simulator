#include <SFML/Graphics.hpp>
#include <filesystem>
#include <iostream>
#include <imgui.h>
#include <imgui-SFML.h>
#include <imgui_stdlib.h>
#include "Headers/Simulation.hpp"
#include "Headers/gui/Style/myStyle.hpp"
#include "Headers/gui/nfd.h"
#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>

const std::string currentVersion = "1.2.1";

namespace fs = std::filesystem;
std::map<sf::Keyboard::Key, bool> keys = std::map<sf::Keyboard::Key, bool>();

static std::string ShowFileDialog(std::string currentDir)
{
	std::string path = "";
	nfdchar_t* outPath = nullptr;
	nfdresult_t result = NFD_OpenDialog("json;", std::string(currentDir + "\\Saves\\").c_str(), &outPath);
	if (result == NFD_OKAY)
	{
		path = outPath;
		free(outPath);
		return path;
	}
	else if (result == NFD_CANCEL)
	{
		//puts("User pressed cancel.");
	}
	else
	{
		printf("Error: %s\n", NFD_GetError());
	}

	return "";
}

// Callback function to handle the data received from the server
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

static std::string checkUpdates() {
	CURL* curl;
	CURLcode res;
	std::string readBuffer;

	curl_global_init(CURL_GLOBAL_DEFAULT);
	curl = curl_easy_init();

	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, "https://alx-m24-github-io.pages.dev/LogicGateSim/Updates/latestVersion");
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback); // set callback
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);       // pass string buffer

		res = curl_easy_perform(curl);

		if (res != CURLE_OK)
			std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;

		curl_easy_cleanup(curl);
	}

	curl_global_cleanup();

	return readBuffer;
}

int main() {
#pragma region Initialize
	std::string currentDir = fs::current_path().string();

	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;

	sf::RenderWindow window(sf::VideoMode(1000, 600), "LogicGate Simulator", sf::Style::Default, settings);
	sf::Image icon;
	icon.loadFromFile(currentDir + "\\Logic Gate.png");
	window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
	window.setVerticalSyncEnabled(true);

	char* appDataPath = nullptr;
	size_t len;
	errno_t err = _dupenv_s(&appDataPath, &len, "APPDATA"); // Get %APPDATA% path

	if (err || appDataPath == nullptr) {
		std::cerr << "Error retrieving APPDATA path!\n";
	}


	std::string path(appDataPath);
	path += "\\LogicGateSim";
	free(appDataPath);

	// Initialize ImGui-SFML
	ImGui::SFML::Init(window);
	applyStyle();

	checkUpdates();

	std::string newestVersion = checkUpdates();
	bool newVersion = newestVersion != currentVersion;
#pragma endregion

#pragma region Objects
	Simulation simulation(currentDir + "\\src", 100u);
	simulation.zoom(window, false);
#pragma endregion

#pragma region Main loop
	ImVec4 color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	sf::Clock deltaClock;
	std::string name = "Untitled";
	while (window.isOpen()) {
#pragma region Input
		sf::Event event;
		while (window.pollEvent(event)) {
			ImGui::SFML::ProcessEvent(event);  // Pass events to ImGui
			switch (event.type)
			{
			case sf::Event::Closed:
				window.close();
				break;
			case sf::Event::Resized:
				window.setView(sf::View(sf::FloatRect(0.0f, 0.0f, static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y))));
				simulation.zoom(window, false);
				break;
			case sf::Event::MouseWheelScrolled:
				simulation.gridSize -= event.mouseWheelScroll.delta;
				simulation.zoom(window, true);
				break;
			case sf::Event::KeyPressed:
				keys[event.key.code] = true;
				break;
			case sf::Event::KeyReleased:
				keys[event.key.code] = false;
				break;
			default:
				break;
			}
		}
#pragma endregion

#pragma region Update
		ImGui::SFML::Update(window, deltaClock.restart());
		simulation.update(window);

		{
			if (newVersion) {
				ImGui::Begin("Update Available", &newVersion);

				ImGui::Text(std::string("New version(" + newestVersion + ") is available for download").c_str());

				ImGui::Text("Release notes:");
				ImGui::SameLine();
				ImGui::TextLink("https://alx-m24-github-io.pages.dev/LogicGateSim/Updates/ReleaseNotes.html");

				if (ImGui::Button("Dismiss")) newVersion = false;
				ImGui::SameLine();
				if (ImGui::Button("Download")) {
					ShellExecute(0, 0, L"https://alx-m24-github-io.pages.dev/LogicGateSim/hwapdosjizo07122024-/herfiuh/difoej1j/LogicGateSimulator.msi", 0, 0, SW_SHOW);
				}

				ImGui::End();
			}
		}

		static bool addElementcollapsed;
		float addElementHeight = (addElementcollapsed) ? (float)window.getSize().y - ImGui::GetFrameHeight() : (float)window.getSize().y - 100;
		{
			ImGui::SetNextWindowSize({ (float)window.getSize().x, 100 });
			ImGui::SetNextWindowPos(ImVec2(0, addElementHeight));

			ImGui::Begin("Add elements", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

			addElementcollapsed = ImGui::IsWindowCollapsed();

			if (!addElementcollapsed) {
				ImGui::Text("");

				ImGui::SameLine();
				if (ImGui::Button("Node")) {
					simulation.addNode(window);
				}

				std::vector<Gate> gates = simulation.getGates();

				for (Gate& gate : gates) {
					ImGui::SameLine();
					if (ImGui::Button(gate.name.c_str())) {
						simulation.addGate(gate.name, window);
					}
				}

				ImGui::Separator();
				ImGui::Spacing();
				ImGui::Spacing();

				std::vector<std::string> customGates = getCustomGates(path + "\\Saves\\");

				for (const std::string& str : customGates) {
					ImGui::SameLine();
					if (ImGui::Button(str.c_str())) {
						Components components = simulation.getComponents();
						loadasCustom(path + "\\Saves\\", str, components, simulation.spacing, simulation.arial);
						simulation.getComponents().gates.back()->position = (sf::Vector2f(window.getSize()) / 2.0f) + getRandomOffset(-50.0f, 50.0f);
					}
				}
			}
			ImGui::End();
		}

		{
			ImGui::SetNextWindowPos({ 0, 0 });
			ImGui::Begin("Circuit", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize);

			ImGui::Text(simulation.analyze().c_str());


			if (ImGui::Button("Import")) {
				std::string gate = ShowFileDialog(path);
				if (gate != "") {
					Components components = simulation.getComponents();
					loadFromPath(gate, components, simulation.spacing, simulation.arial);
				}
			}

			ImGui::SameLine();

			if (ImGui::Button("Clear")) {
				simulation.deleteAll();
			}

			ImGui::Text("FPS: %d", static_cast<int>(round(ImGui::GetIO().Framerate)));

			ImGui::End();
		}

		static bool chipPreviewcollapsed;
		float chipPreviewHeight = (chipPreviewcollapsed) ? addElementHeight - ImGui::GetFrameHeight() : addElementHeight - 150;
		{
			ImGui::SetNextWindowSize({ 175, 150 });
			ImGui::SetNextWindowPos({ (float)window.getSize().x - 175, chipPreviewHeight});
			ImGui::Begin("Chip preview", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

			ImGui::InputText("Name", &name, ImGuiInputTextFlags_::ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_::ImGuiInputTextFlags_CharsNoBlank);

			auto nodeNum = simulation.getNodeNum();

			ImGui::Text("Input Num: %d", nodeNum.first);
			ImGui::Text("Output Num: %d", nodeNum.second);

			if (ImGui::Button("Create")) {
				save(path + "\\Saves\\", name, simulation.getComponents(), color);
				name = "Untitled";
				color = { 1.0f, 1.0f, 1.0f, 1.0f };
			}

			ImGui::ColorEdit3("Color", &color.x);

			chipPreviewcollapsed = ImGui::IsWindowCollapsed();

			ImGui::End();
		}
#pragma endregion

#pragma region Display
		window.clear();

		simulation.draw(window);
		ImGui::SFML::Render(window);

		window.display();
#pragma endregion
	}
#pragma endregion

#pragma region Terminate
	ImGui::SFML::Shutdown();
	return EXIT_SUCCESS;
#pragma endregion
}