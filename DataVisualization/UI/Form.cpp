#include "Form.h"
#include <sstream>
#include <iostream>

Form::Form () {
	setNewFunc();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	font = io.Fonts->AddFontFromFileTTF("resources/fonts/JetBrainsMono-Medium.ttf", config::windowY / 42.0f);
}

Form::~Form () {
	for (auto a : functionList) {
		delete(a);
	}
}

void Form::draw () {
	if (open) {
		switch (current) {
			case PREP:
				drawPrep();
				break;
			case FUNCTION:
				drawFunction();
				break;
		}
	}
}

void Form::drawPrep () {
	// create the window with a slider at the top
	ImGui::Begin("Prep Window", &open, flags);
	ImGui::PushFont(font);
	ImGui::SetCursorPosX(ImGui::GetWindowSize().x * .5f - (ImGui::CalcTextSize("Amount of Attributes: ").x * .5f));
	ImGui::Text("Amount of Attributes: ");
	ImGui::SetNextItemWidth(ImGui::GetWindowSize().x * .95f);
	ImGui::SliderInt("##amtSlider", &func->attributeCount, 2, defaultAmount);
	ImGui::Separator();

	// create a subwindow that contains the attributes
	ImGui::BeginChild("##", ImVec2{ ImGui::GetWindowSize().x * .95f, ImGui::GetWindowSize().y * .69f }, ImGuiChildFlags_None, ImGuiWindowFlags_AlwaysVerticalScrollbar);
	for (int a = 0; a < func->attributeCount; a++) {
		ImGui::Text(std::string("Attribute ").append(std::to_string(a + 1)).c_str(),
			std::string("# of values for ").append(std::to_string(a + 1)).c_str());
		ImGui::SetNextItemWidth(ImGui::GetWindowSize().x * .66f);
		ImGui::SameLine(ImGui::GetWindowSize().x * .3f);
		ImGui::SliderInt(std::string("##").append(std::to_string(a)).c_str(), 
			&func->kValues.data()[a], 2, 10);
	}
	ImGui::PopFont();
	ImGui::EndChild();
	//

	// create the bottom section that contains the enter button.
	ImGui::Separator();
	ImGui::SetCursorPosX(ImGui::GetWindowSize().x * .78f);
	if (ImGui::Button("Enter", ImVec2{ ImGui::GetWindowSize().x * .2f, ImGui::GetWindowSize().y * .1f })) {
		current = FUNCTION;
		std::vector<int>* temp = new std::vector<int>;
		temp->resize(func->attributeCount);
		std::fill(temp->begin(), temp->end(), 0);
		func->clause = temp;
	}

	// set window size and position
	ImVec2 window = ImGui::GetWindowSize();
	ImGui::SetWindowSize(ImVec2(config::windowX * .5f, config::windowY * .5f));
	ImGui::SetWindowPos(ImVec2(window.x - (config::windowX * .25f), window.y - (config::windowY * .25f)));
	ImGui::End();
}

void Form::drawFunction () {
	ImGui::Begin("##", nullptr, flags);
	ImVec2 window = ImGui::GetWindowSize();
	if (ImGui::Button("Back", ImVec2{ window.x * .15f, window.y * .05f })) {
		current = PREP;
		draw();
		ImGui::End();
		return;
	}
	ImGui::SameLine();
	ImGui::SetCursorPosX(window.x * .5f - (ImGui::CalcTextSize("Create a Clause").x * .5f));

	// header
	ImGui::PushFont(font);
	ImGui::Text("Create a Clause");
	ImGui::SameLine(window.x * .835f);
	ImGui::Text((currentFunction + " " + std::to_string(functionIndex+1) + "/" + std::to_string(functionList.size())).c_str());
	ImGui::SameLine();
	if (ImGui::ArrowButton("##left", ImGuiDir_Left)) {
		if (functionIndex > 0) {
			functionIndex--;
			func = functionList.at(functionIndex);
		}
	}
	ImGui::SameLine();
	if (ImGui::ArrowButton("##right", ImGuiDir_Right)) {
		if (functionIndex < functionList.size() - 1) {
			functionIndex++;
			func = functionList.at(functionIndex);
		}
	}

	ImGui::Separator();

	// value setting field
	ImGui::BeginChild("##subwindow", ImVec2{ window.x * .95f, window.y * .58f }, ImGuiChildFlags_None);
	ImGui::SetNextItemWidth(100);
	ImGui::BeginTable("##functiontable", func->attributeCount, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_ScrollX | ImGuiTableFlags_NoHostExtendX | ImGuiTableFlags_SizingFixedSame | ImGuiTableFlags_BordersOuterV);
	
	for (int a = 0; a < func->attributeCount; a++) {
		ImGui::TableSetupColumn( std::string("x").append(std::to_string(a + 1)).c_str(), config::windowX * .2f );
	}
	ImGui::TableHeadersRow();
	for (int a = 0; a < func->attributeCount; a++) {
		ImGui::TableNextColumn();
		for (int b = 0; b < func->kValues.at(a); b++) {
			ImGui::RadioButton(std::to_string(b).append("##").append(std::to_string(a)).c_str(), &func->clause->data()[a], b);
		}
	}
	ImGui::EndTable();
	ImGui::EndChild();
	ImGui::PopFont();
	ImGui::Separator();
	//
	
	// clauses table
	ImGui::BeginTable("##existing_clauses", (int)(func->clauseList.size()+1), ImGuiTableFlags_ScrollX | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersInnerV, ImVec2{window.x * .6f, window.y * .19f});
	for (int a = 0; a < func->clauseList.size(); a++) {
		ImGui::TableSetupColumn(std::string("Clause: ").append(std::to_string(a + 1)).c_str(), config::windowX * .2f);
	}
	for (int a = 0; a < func->clauseList.size(); a++) {
		ImGui::TableNextColumn();
		ImGui::SetNextItemWidth(100);
		ImGui::Text(std::string("Clause ").append(std::to_string(a+1)).c_str());
		ImGui::SameLine(85);
		if (ImGui::Button(std::string("x##").append(std::to_string(a)).c_str(), ImVec2{15.0f, 15.0f})) {
			func->clauseList.erase(func->clauseList.begin()+a);
			statusMessage = std::string("Removed Clause ").append(std::to_string(a + 1));
			action = "Add Clause";
		}
		if (ImGui::Button(std::string("Load##").append(std::to_string(a)).c_str(), ImVec2{100, 38})) {
			func->clause = func->clauseList.at(a);
			statusMessage = std::string("Loaded Clause ").append(std::to_string(a+1));
			action = "Update Clause";
		}
	}
	ImGui::EndTable();

	//// options
	ImVec2 buttonSize{ window.x * .3f, window.y * .06f };

	// new function
	ImGui::SetCursorPosX(window.x * .99f - buttonSize.x);
	ImGui::SetCursorPosY(window.y * .98f - (4.0f * buttonSize.y) - window.y * .01f);
	if (ImGui::Button("Add a function", buttonSize)) {
		setNewFunc();
		functionIndex++;
		current = PREP;
	}

	// status message
	if (!statusMessage.empty()) {
		float shift = ImGui::CalcTextSize(statusMessage.c_str()).x;
		ImGui::SetCursorPosX(window.x * .99f - shift - (buttonSize.x - shift)/2.0f);
		ImGui::SetCursorPosY(window.y * .98f - (3 * buttonSize.y));
		ImGui::Text(statusMessage.c_str());
	}
	ImGui::SetCursorPosX(window.x * .99f - buttonSize.x);
	ImGui::SetCursorPosY(window.y * .98f - (2.0f * buttonSize.y) - window.y * .01f);

	// add clause button
	if (ImGui::Button(action.c_str(), buttonSize)) {
		if (action == "Add Clause") {
			statusMessage = "Added Clause";
			func->clauseList.insert(func->clauseList.end(), func->clause);
		}
		else if (action == "Update Clause") {
			statusMessage = "Updated Clause";
		}
		func->clause = new std::vector<int>{};
		func->clause->resize(func->attributeCount);
		std::fill(func->clause->begin(), func->clause->end(), 0);
		action = "Add Clause";
		// TODO check for dupes?
	}
	
	// finish button
	ImGui::SetCursorPosX(window.x * .99f - buttonSize.x);
	ImGui::SetCursorPosY(window.y * .98f - buttonSize.y);

	if (ImGui::Button("Finish", buttonSize)) {
		statusMessage = "Pressed Finish Button";
		// TODO save to file screen
		std::ofstream file("output.csv");

		for (int a = 0; a < func->clauseList.size(); a++) {
			for (int b = 0; b < func->clauseList.at(a)->size(); b++) {
				file << func->clauseList.at(a)->at(b) << ", ";
			}
			file << std::endl;
		}

		file.close();
	}


	//
	ImGui::SetWindowSize(ImVec2(config::windowX * .75f, config::windowY * .5f));
	ImGui::SetWindowPos(ImVec2(window.x - (config::windowX * .625f), window.y - (config::windowY * .25f)));
	ImGui::End();
}

void Form::openWindow () {
	open = true;
}

void Form::setNewFunc () {
	func = new Function();
	func->kValues.resize(defaultAmount);
	std::fill(func->kValues.begin(), func->kValues.end(), 1);
	functionList.insert(functionList.end(), func);
}
