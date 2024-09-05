#include "Form.h"


// draws the function screen where the user can input clauses and save to file.
void Form::drawFunction() {
	ImGui::Begin("##", &open, flags);
	ImGui::SetWindowSize(ImVec2(config::windowX * .75f, config::windowY * .75f));
	ImVec2 window = ImGui::GetWindowSize();
	ImVec2 border = ImGui::GetStyle().WindowPadding;
	ImGui::SetWindowPos(ImVec2(config::windowX * .5f - window.x * .5f, config::windowY * .5f - window.y * .5f));
	//// header
	// back button

	if (ImGui::Button("Back", ImVec2{ window.x * .15f, 20.0f })) {
		current = PREP;
	}
	ImGui::SameLine();
	// create clause header
	ImGui::SetCursorPosX(window.x * .4f - (ImGui::CalcTextSize("Create a Clause").x * .5f));
	ImGui::PushFont(font);
	ImGui::Text("Create a Clause");
	ImGui::SameLine(window.x * .835f - ImGui::CalcTextSize(func->functionName).x);

	



	std::string functionFraction = (std::to_string(functionIndex + 1) + "/" + std::to_string(functionList.size()));
	ImGui::SetCursorPosX((window.x - ImGui::CalcTextSize(func->functionName).x) - (font->FontSize * 3.0f) - ImGui::CalcTextSize(functionFraction.c_str()).x - (ImGui::GetStyle().ItemSpacing.x * 4.0f) - ImGui::GetStyle().WindowPadding.x );
	ImGui::Text(func->functionName);
	ImGui::SameLine();
	ImGui::Text(functionFraction.c_str());
	ImGui::SameLine();
	if (ImGui::ArrowButton("##lefta", ImGuiDir_Left)) {
		if (functionIndex > 0) {
			functionIndex--;
			func = functionList.at(functionIndex);
			siblingFunctionIndex = 0;
			clauseIndex = 0;
			action = "Add Clause";
		}
		func->clause = func->siblingfunctionList[siblingFunctionIndex].at(0);
	}
	ImGui::SameLine();
	bool disable = func->parent == nullptr;
	if (disable) {
		ImGui::BeginDisabled();
	}
	if (ImGui::ArrowButton("##up", ImGuiDir_Up)) {
		func = func->parent;
		siblingFunctionIndex = 0;
		statusMessage = "Loaded predecessor function";
	}
	if (disable) {
		ImGui::EndDisabled();
	}
	if (ImGui::BeginItemTooltip()) {
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted("Go to the predecessor of this function");
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
	ImGui::SameLine();
	if (ImGui::ArrowButton("##righta", ImGuiDir_Right)) {
		if (functionIndex < functionList.size() - 1) {
			functionIndex++;
			func = functionList.at(functionIndex);
			siblingFunctionIndex = 0;
			clauseIndex = 0;
			action = "Add Clause";
		}
		func->clause = func->siblingfunctionList[siblingFunctionIndex].at(0);
	}

	




	// value setting field
	float posY = ImGui::GetCursorPosY();
	ImVec2 containerSize{ window.x * .65f, 80.0f };
	float tableSizeY = window.y - posY - (containerSize.y * 3.0f) - (ImGui::GetStyle().WindowPadding.y) - (ImGui::GetStyle().ItemSpacing.y * 4.0f);
	ImGui::BeginChild("##subwindow", ImVec2{ window.x - ImGui::GetStyle().WindowPadding.x * 2.0f, tableSizeY }, ImGuiChildFlags_None);
	ImGui::SetNextItemWidth(100);
	if (ImGui::BeginTable("##functiontable", func->attributeCount, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_ScrollX | ImGuiTableFlags_NoHostExtendX | ImGuiTableFlags_SizingFixedSame | ImGuiTableFlags_BordersOuterV)) {
		for (int a = 0; a < func->attributeCount; a++) {
			ImGui::TableSetupColumn(func->attributeNames.at(a), 160);
		}
		ImGui::TableHeadersRow();
		for (int a = 0; a < func->attributeCount; a++) {
			ImGui::TableNextColumn();
			for (int b = 0; b < func->kValues.at(a); b++) {
				if (func->clause != nullptr) {
					ImGui::RadioButton(std::to_string(b).append("##").append(std::to_string(a)).c_str(), &func->clause->data()[a], b);
				}
			}
		}
		ImGui::EndTable();
	}
	ImGui::EndChild();
	ImGui::PopFont();
	ImGui::Separator();
	//




	ImGui::BeginGroup();
		ImGuiTableFlags containerFlags = ImGuiTableFlags_ScrollX | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersInnerV;
		// clause list
		float top = ImGui::GetCursorPos().y; // for buttons later
		if (ImGui::BeginTable("##existing_clauses", (int)(func->siblingfunctionList[siblingFunctionIndex].size() + 1), containerFlags, containerSize)) {
			for (int a = 0; a < func->siblingfunctionList[siblingFunctionIndex].size(); a++) {
				ImGui::TableNextColumn();
				ImGui::SetNextItemWidth(100);
				ImGui::Text(std::string("Clause ").append(std::to_string(a + 1)).c_str());
				ImGui::SameLine(85);

				if (ImGui::Button(std::string("x##").append(std::to_string(a)).c_str(), ImVec2{ 15.0f, 15.0f })) {
					//delete(*(func->siblingfunctionList[siblingFunctionIndex].begin() + a)); // todo might need to delete when we remove.
					func->siblingfunctionList[siblingFunctionIndex].erase(func->siblingfunctionList[siblingFunctionIndex].begin() + a);
					statusMessage = std::string("Removed Clause ").append(std::to_string(a + 1));
					action = "Add Clause";
					clauseIndex--;
				}
				if (ImGui::Button(std::string("Load##").append(std::to_string(a)).c_str(), ImVec2{ 100.0f, 38.0f })) {
					func->clause = func->siblingfunctionList[siblingFunctionIndex].at(a);
					statusMessage = std::string("Loaded Clause ").append(std::to_string(a + 1));
					action = "Update Clause";
					clauseIndex = a;
				}
			}
			ImGui::EndTable();
		}

		// sibling functions list
		if (ImGui::BeginTable("##siblingfunction", (int)(func->siblingfunctionList.size() + 1), containerFlags, containerSize)) {
			for (int a = 0; a < func->siblingfunctionList.size(); a++) {
				//bool disabled = a == subfunctionIndex;
				ImGui::TableNextColumn();
				ImGui::SetNextItemWidth(100);
				ImGui::Text(std::string("Sibling-Func ").append(std::to_string(a + 1)).c_str());
				ImGui::SameLine(105);
				if (ImGui::Button(std::string("x##").append(std::to_string(a)).c_str(), ImVec2{ 15.0f, 15.0f })) {
					func->siblingfunctionList.erase(func->siblingfunctionList.begin() + a);
					statusMessage = std::string("Removed Sibling ").append(std::to_string(a + 1));
					if (siblingFunctionIndex >= func->siblingfunctionList.size()) {
						siblingFunctionIndex--;
					}
				}
				if (ImGui::Button(std::string("Load##").append(std::to_string(a)).c_str(), ImVec2{ 120.0f, 38.0f })) {
					statusMessage = std::string("Loaded Sibling ").append(std::to_string(a + 1));
					siblingFunctionIndex = a;
				}
			}
			ImGui::EndTable();
		}

		// sub function list
		if (ImGui::BeginTable("##subfunction", (int)(func->subfunctionList.size() + 1), containerFlags, containerSize)) {
			for (int a = 0; a < func->subfunctionList.size(); a++) {
				ImGui::TableNextColumn();
				ImGui::SetNextItemWidth(100);
				ImGui::Text(std::string("Sub-Func ").append(std::to_string(a + 1)).c_str());
				ImGui::SameLine(105);
				if (ImGui::Button(std::string("x##").append(std::to_string(a)).c_str(), ImVec2{ 15.0f, 15.0f })) {
					func->subfunctionList.erase(func->subfunctionList.begin() + a);
					statusMessage = std::string("Removed Sub ").append(std::to_string(a + 1));
				}
				if (ImGui::Button(std::string("Load##").append(std::to_string(a)).c_str(), ImVec2{ 120.0f, 38.0f })) {
					statusMessage = std::string("Loaded Sub ").append(std::to_string(a + 1));
					func = func->subfunctionList[a];
					siblingFunctionIndex = 0;
				}
			}
			ImGui::EndTable();
		}
	ImGui::EndGroup();





	//// options
	ImVec2 buttonSize{ window.x - containerSize.x - ImGui::GetStyle().WindowPadding.x * 2.0f - ImGui::GetStyle().ItemSpacing.x, window.y * .05f };
	ImGui::SetCursorPosX(window.x - buttonSize.x - border.x);
	ImGui::SetCursorPosY(top);
	ImGui::BeginGroup();
		if (ImGui::Button(action.c_str(), buttonSize)) {
			if (action == "Add Clause") {
				statusMessage = "Added Clause";

				if (func->siblingfunctionList.empty()) {
					func->siblingfunctionList.push_back({});
				}
				func->siblingfunctionList[siblingFunctionIndex].insert(func->siblingfunctionList[siblingFunctionIndex].end(), func->clause);
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

		if (ImGui::Button("Add a sibling function", buttonSize)) {
			std::vector<std::vector<int>*> subfunction;
			func->siblingfunctionList.insert(func->siblingfunctionList.end(), subfunction);
			siblingFunctionIndex++;
			statusMessage = "Added sibling function";
		}

		if (ImGui::Button("Add a sub function", buttonSize)) {
			// create a new function and set the old one as the new ones parent, and put the child function in the parents list.
			Function* parent = func;
			setNewFunc();
			func->parent = parent;
			parent->subfunctionList.push_back(func);

			// set a clause so the table isn't empty when sub function is loaded.
			func->clause = new std::vector<int>{};
			func->clause->resize(func->attributeCount);
			std::fill(func->clause->begin(), func->clause->end(), 0);
			siblingFunctionIndex = 0;
			statusMessage = "Added Sub Function";
			action = "Add Clause";
		}

		if (!statusMessage.empty()) {
			ImGui::PushFont(font);
			ImVec2 shift = ImGui::CalcTextSize(statusMessage.c_str());
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (window.x - border.x - ImGui::GetCursorPosX()) * .5f - (ImGui::CalcTextSize(statusMessage.c_str()).x * .5f) );
			ImGui::Text(statusMessage.c_str(), buttonSize);
			ImGui::PopFont();
		}
	ImGui::EndGroup();





	ImGui::SetCursorPos(ImVec2{window.x - buttonSize.x - border.x, window.y - buttonSize.y - border.y});
	if (ImGui::Button("Finish", buttonSize)) {
		statusMessage = "Pressed Finish Button";
		// TODO save to file screen
		CSVReader::saveToCSV(&functionList, basePath + "\\" + functionList[0]->functionName + ".csv");
		open = !open;

		//create hanselChainSet for function
		func->initializeHanselChains();

		// organize them and assign classes such that we can visualize
		func->setUpHanselChains();

		// create a model for the hanselChains
		addModel = true;
		config::drawIndex++;



		//files.clear();
		//for (auto a : std::filesystem::directory_iterator(basePath)) {
		//	files.push_back(a.path().string().erase(0, basePath.length() + 1));
		//}
	}

	//
	ImGui::End();
}